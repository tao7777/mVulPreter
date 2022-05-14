 static int pgx_gethdr(jas_stream_t *in, pgx_hdr_t *hdr)
 {
 	int c;
	jas_uchar buf[2];
 
 	if ((c = jas_stream_getc(in)) == EOF) {
 		goto error;
	}
	buf[0] = c;
	if ((c = jas_stream_getc(in)) == EOF) {
		goto error;
	}
	buf[1] = c;
	hdr->magic = buf[0] << 8 | buf[1];
	if (hdr->magic != PGX_MAGIC) {
		jas_eprintf("invalid PGX signature\n");
		goto error;
	}
	if ((c = pgx_getc(in)) == EOF || !isspace(c)) {
		goto error;
	}
	if (pgx_getbyteorder(in, &hdr->bigendian)) {
		jas_eprintf("cannot get byte order\n");
		goto error;
	}
	if (pgx_getsgnd(in, &hdr->sgnd)) {
		jas_eprintf("cannot get signedness\n");
		goto error;
	}
	if (pgx_getuint32(in, &hdr->prec)) {
		jas_eprintf("cannot get precision\n");
		goto error;
	}
	if (pgx_getuint32(in, &hdr->width)) {
		jas_eprintf("cannot get width\n");
		goto error;
	}
	if (pgx_getuint32(in, &hdr->height)) {
		jas_eprintf("cannot get height\n");
		goto error;
	}
	return 0;

error:
	return -1;
}
