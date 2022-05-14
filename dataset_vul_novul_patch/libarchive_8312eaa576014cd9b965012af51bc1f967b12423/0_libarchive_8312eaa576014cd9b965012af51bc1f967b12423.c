parse_rockridge(struct archive_read *a, struct file_info *file,
     const unsigned char *p, const unsigned char *end)
 {
 	struct iso9660 *iso9660;
	int entry_seen = 0;
 
 	iso9660 = (struct iso9660 *)(a->format->data);
 
	while (p + 4 <= end  /* Enough space for another entry. */
	    && p[0] >= 'A' && p[0] <= 'Z' /* Sanity-check 1st char of name. */
	    && p[1] >= 'A' && p[1] <= 'Z' /* Sanity-check 2nd char of name. */
	    && p[2] >= 4 /* Sanity-check length. */
	    && p + p[2] <= end) { /* Sanity-check length. */
		const unsigned char *data = p + 4;
		int data_length = p[2] - 4;
		int version = p[3];

		switch(p[0]) {
		case 'C':
			if (p[1] == 'E') {
				if (version == 1 && data_length == 24) {
					/*
					 * CE extension comprises:
					 *   8 byte sector containing extension
					 *   8 byte offset w/in above sector
					 *   8 byte length of continuation
					 */
					int32_t location =
					    archive_le32dec(data);
					file->ce_offset =
					    archive_le32dec(data+8);
					file->ce_size =
					    archive_le32dec(data+16);
					if (register_CE(a, location, file)
					    != ARCHIVE_OK)
						return (ARCHIVE_FATAL);
				}
			}
			else if (p[1] == 'L') {
				if (version == 1 && data_length == 8) {
					file->cl_offset = (uint64_t)
					    iso9660->logical_block_size *
					    (uint64_t)archive_le32dec(data);
					iso9660->seenRockridge = 1;
				}
			}
			break;
		case 'N':
			if (p[1] == 'M') {
				if (version == 1) {
					parse_rockridge_NM1(file,
					    data, data_length);
					iso9660->seenRockridge = 1;
				}
			}
			break;
		case 'P':
			/*
			 * PD extension is padding;
			 * contents are always ignored.
			 *
			 * PL extension won't appear;
			 * contents are always ignored.
			 */
			if (p[1] == 'N') {
				if (version == 1 && data_length == 16) {
					file->rdev = toi(data,4);
					file->rdev <<= 32;
					file->rdev |= toi(data + 8, 4);
					iso9660->seenRockridge = 1;
				}
			}
			else if (p[1] == 'X') {
				/*
				 * PX extension comprises:
				 *   8 bytes for mode,
				 *   8 bytes for nlinks,
				 *   8 bytes for uid,
				 *   8 bytes for gid,
				 *   8 bytes for inode.
				 */
				if (version == 1) {
					if (data_length >= 8)
						file->mode
						    = toi(data, 4);
					if (data_length >= 16)
						file->nlinks
						    = toi(data + 8, 4);
					if (data_length >= 24)
						file->uid
						    = toi(data + 16, 4);
					if (data_length >= 32)
						file->gid
						    = toi(data + 24, 4);
					if (data_length >= 40)
						file->number
						    = toi(data + 32, 4);
					iso9660->seenRockridge = 1;
				}
			}
			break;
		case 'R':
			if (p[1] == 'E' && version == 1) {
				file->re = 1;
				iso9660->seenRockridge = 1;
			}
			else if (p[1] == 'R' && version == 1) {
				/*
				 * RR extension comprises:
				 *    one byte flag value
				 * This extension is obsolete,
				 * so contents are always ignored.
				 */
			}
			break;
		case 'S':
			if (p[1] == 'L') {
				if (version == 1) {
					parse_rockridge_SL1(file,
					    data, data_length);
					iso9660->seenRockridge = 1;
				}
			}
			else if (p[1] == 'T'
			    && data_length == 0 && version == 1) {
				/*
				 * ST extension marks end of this
				 * block of SUSP entries.
				 *
				 * It allows SUSP to coexist with
				 * non-SUSP uses of the System
				 * Use Area by placing non-SUSP data
				 * after SUSP data.
				 */
				iso9660->seenSUSP = 0;
				iso9660->seenRockridge = 0;
				return (ARCHIVE_OK);
			}
			break;
		case 'T':
			if (p[1] == 'F') {
				if (version == 1) {
					parse_rockridge_TF1(file,
					    data, data_length);
					iso9660->seenRockridge = 1;
				}
			}
			break;
		case 'Z':
			if (p[1] == 'F') {
				if (version == 1)
					parse_rockridge_ZF1(file,
					    data, data_length);
			}
			break;
		default:
			break;
 		}
 
 		p += p[2];
		entry_seen = 1;
	}

	if (entry_seen)
		return (ARCHIVE_OK);
	else {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
				  "Tried to parse Rockridge extensions, but none found");
		return (ARCHIVE_WARN);
 	}
 }
