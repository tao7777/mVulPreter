 jpc_streamlist_t *jpc_ppmstabtostreams(jpc_ppxstab_t *tab)
 {
 	jpc_streamlist_t *streams;
	jas_uchar *dataptr;
 	uint_fast32_t datacnt;
 	uint_fast32_t tpcnt;
 	jpc_ppxstabent_t *ent;
	int entno;
	jas_stream_t *stream;
	int n;

	if (!(streams = jpc_streamlist_create())) {
		goto error;
	}

	if (!tab->numents) {
		return streams;
	}

	entno = 0;
	ent = tab->ents[entno];
	dataptr = ent->data;
	datacnt = ent->len;
	for (;;) {

		/* Get the length of the packet header data for the current
		  tile-part. */
		if (datacnt < 4) {
			goto error;
		}
		if (!(stream = jas_stream_memopen(0, 0))) {
			goto error;
		}
		if (jpc_streamlist_insert(streams, jpc_streamlist_numstreams(streams),
		  stream)) {
			goto error;
		}
		tpcnt = (dataptr[0] << 24) | (dataptr[1] << 16) | (dataptr[2] << 8)
		  | dataptr[3];
		datacnt -= 4;
		dataptr += 4;

		/* Get the packet header data for the current tile-part. */
		while (tpcnt) {
			if (!datacnt) {
				if (++entno >= tab->numents) {
					goto error;
				}
				ent = tab->ents[entno];
				dataptr = ent->data;
				datacnt = ent->len;
			}
			n = JAS_MIN(tpcnt, datacnt);
			if (jas_stream_write(stream, dataptr, n) != n) {
				goto error;
			}
			tpcnt -= n;
			dataptr += n;
			datacnt -= n;
		}
		jas_stream_rewind(stream);
		if (!datacnt) {
			if (++entno >= tab->numents) {
				break;
			}
			ent = tab->ents[entno];
			dataptr = ent->data;
			datacnt = ent->len;
		}
	}

	return streams;

error:
	if (streams) {
		jpc_streamlist_destroy(streams);
	}
	return 0;
}
