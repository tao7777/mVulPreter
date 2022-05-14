int treeRead(struct READER *reader, struct DATAOBJECT *data) {

	int i, j, err, olen, elements, size, x, y, z, b, e, dy, dz, sx, sy, sz, dzy,
		szy;
	char *input, *output;

	uint8_t node_type, node_level;
	uint16_t entries_used;
	uint32_t size_of_chunk;
	uint32_t filter_mask;
	uint64_t address_of_left_sibling, address_of_right_sibling, start[4],
		child_pointer, key, store;

	char buf[4];

	UNUSED(node_level);
	UNUSED(address_of_right_sibling);
	UNUSED(address_of_left_sibling);
	UNUSED(key);

	if (data->ds.dimensionality > 3) {
		log("TREE dimensions > 3");
		return MYSOFA_INVALID_FORMAT;
	}

	/* read signature */
	if (fread(buf, 1, 4, reader->fhd) != 4 || strncmp(buf, "TREE", 4)) {
		log("cannot read signature of TREE\n");
		return MYSOFA_INVALID_FORMAT;
	} log("%08lX %.4s\n", (uint64_t )ftell(reader->fhd) - 4, buf);

	node_type = (uint8_t)fgetc(reader->fhd);
	node_level = (uint8_t)fgetc(reader->fhd);
	entries_used = (uint16_t)readValue(reader, 2);
	if(entries_used>0x1000)
		return MYSOFA_UNSUPPORTED_FORMAT;
	address_of_left_sibling = readValue(reader,
					    reader->superblock.size_of_offsets);
	address_of_right_sibling = readValue(reader,
					     reader->superblock.size_of_offsets);

	elements = 1;
	for (j = 0; j < data->ds.dimensionality; j++)
		elements *= data->datalayout_chunk[j];
	dy = data->datalayout_chunk[1];
	dz = data->datalayout_chunk[2];
	sx = data->ds.dimension_size[0];
	sy = data->ds.dimension_size[1];
	sz = data->ds.dimension_size[2];
	dzy = dz * dy;
	szy = sz * sy;
	size = data->datalayout_chunk[data->ds.dimensionality];

	log("elements %d size %d\n",elements,size);

	if (!(output = malloc(elements * size))) {
		return MYSOFA_NO_MEMORY;
	}

	for (e = 0; e < entries_used * 2; e++) {
		if (node_type == 0) {
			key = readValue(reader, reader->superblock.size_of_lengths);
		} else {
			size_of_chunk = (uint32_t)readValue(reader, 4);
			filter_mask = (uint32_t)readValue(reader, 4);
			if (filter_mask) {
				log("TREE all filters must be enabled\n");
				free(output);
				return MYSOFA_INVALID_FORMAT;
			}

			for (j = 0; j < data->ds.dimensionality; j++) {
				start[j] = readValue(reader, 8);
				log("start %d %lu\n",j,start[j]);
			}

			if (readValue(reader, 8)) {
				break;
			}

			child_pointer = readValue(reader,
						  reader->superblock.size_of_offsets);
			log(" data at %lX len %u\n", child_pointer, size_of_chunk);

			/* read data */
			store = ftell(reader->fhd);
			if (fseek(reader->fhd, child_pointer, SEEK_SET)<0) {
				free(output);
				return errno;
			}

			if (!(input = malloc(size_of_chunk))) {
				free(output);
				return MYSOFA_NO_MEMORY;
			}
			if (fread(input, 1, size_of_chunk, reader->fhd) != size_of_chunk) {
				free(output);
				free(input);
				return MYSOFA_INVALID_FORMAT;
			}

			olen = elements * size;
			err = gunzip(size_of_chunk, input, &olen, output);
			free(input);

			log("   gunzip %d %d %d\n",err, olen, elements*size);
			if (err || olen != elements * size) {
				free(output);
				return MYSOFA_INVALID_FORMAT;
			}

			switch (data->ds.dimensionality) {
			case 1:
 				for (i = 0; i < olen; i++) {
 					b = i / elements;
 					x = i % elements + start[0];
					j = x * size + b;
					if (j>=0 && j < elements * size) {
 						((char*)data->data)[j] = output[i];
 					}
 				}
				break;
			case 2:
				for (i = 0; i < olen; i++) {
					b = i / elements;
 					x = i % elements;
 					y = x % dy + start[1];
 					x = x / dy + start[0];
					j = ((x * sy + y) * size) + b;
					if (j>=0 && j < elements * size) {
 						((char*)data->data)[j] = output[i];
 					}
 				}
				break;
			case 3:
				for (i = 0; i < olen; i++) {
					b = i / elements;
					x = i % elements;
 					z = x % dz + start[2];
 					y = (x / dz) % dy + start[1];
 					x = (x / dzy) + start[0];
					j = (x * szy + y * sz + z) * size + b;
					if (j>=0 && j < elements * size) {
 						((char*)data->data)[j] = output[i];
 					}
 				}
				break;
			default:
				log("invalid dim\n");
				return MYSOFA_INTERNAL_ERROR;
			}

			if(fseek(reader->fhd, store, SEEK_SET)<0) {
				free(output);
				return errno;
			}
		}
	}

	free(output);
	if(fseek(reader->fhd, 4, SEEK_CUR)<0) /* skip checksum */
		return errno;

	return MYSOFA_OK;
}
