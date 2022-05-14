 int CLASS ljpeg_start (struct jhead *jh, int info_only)
 {
  int c, tag;
  ushort len;
   uchar data[0x10000];
   const uchar *dp;
 
  memset (jh, 0, sizeof *jh);
  jh->restart = INT_MAX;
  fread (data, 2, 1, ifp);
  if (data[1] != 0xd8) return 0;
  do {
    fread (data, 2, 2, ifp);
    tag =  data[0] << 8 | data[1];
    len = (data[2] << 8 | data[3]) - 2;
    if (tag <= 0xff00) return 0;
    fread (data, 1, len, ifp);
    switch (tag) {
      case 0xffc3:
	jh->sraw = ((data[7] >> 4) * (data[7] & 15) - 1) & 3;
      case 0xffc0:
	jh->bits = data[0];
	jh->high = data[1] << 8 | data[2];
	jh->wide = data[3] << 8 | data[4];
	jh->clrs = data[5] + jh->sraw;
	if (len == 9 && !dng_version) getc(ifp);
	break;
      case 0xffc4:
	if (info_only) break;
	for (dp = data; dp < data+len && (c = *dp++) < 4; )
	  jh->free[c] = jh->huff[c] = make_decoder_ref (&dp);
	break;
      case 0xffda:
	jh->psv = data[1+data[0]*2];
	jh->bits -= data[3+data[0]*2] & 15;
	break;
      case 0xffdd:
	jh->restart = data[0] << 8 | data[1];
    }
  } while (tag != 0xffda);
  if (info_only) return 1;
  FORC(5) if (!jh->huff[c+1]) jh->huff[c+1] = jh->huff[c];
  if (jh->sraw) {
    FORC(4)        jh->huff[2+c] = jh->huff[1];
    FORC(jh->sraw) jh->huff[1+c] = jh->huff[0];
  }
  jh->row = (ushort *) calloc (jh->wide*jh->clrs, 4);
  merror (jh->row, "ljpeg_start()");
  return zero_after_ff = 1;
}
