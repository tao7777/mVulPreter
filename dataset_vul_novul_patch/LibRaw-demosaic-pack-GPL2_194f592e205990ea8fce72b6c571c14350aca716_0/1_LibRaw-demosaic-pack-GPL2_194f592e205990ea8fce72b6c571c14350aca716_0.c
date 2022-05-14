 void CLASS foveon_load_camf()
 {
   unsigned type, wide, high, i, j, row, col, diff;
  ushort huff[258], vpred[2][2] = {{512,512},{512,512}}, hpred[2];
 
   fseek (ifp, meta_offset, SEEK_SET);
   type = get4();  get4();  get4();
   wide = get4();
   high = get4();
   if (type == 2) {
     fread (meta_data, 1, meta_length, ifp);
     for (i=0; i < meta_length; i++) {
      high = (high * 1597 + 51749) % 244944;
      wide = high * (INT64) 301593171 >> 24;
      meta_data[i] ^= ((((high << 8) - wide) >> 1) + wide) >> 17;
    }
  } else if (type == 4) {
    free (meta_data);
    meta_data = (char *) malloc (meta_length = wide*high*3/2);
    merror (meta_data, "foveon_load_camf()");
    foveon_huff (huff);
    get4();
    getbits(-1);
    for (j=row=0; row < high; row++) {
      for (col=0; col < wide; col++) {
	diff = ljpeg_diff(huff);
	if (col < 2) hpred[col] = vpred[row & 1][col] += diff;
	else         hpred[col & 1] += diff;
	if (col & 1) {
	  meta_data[j++] = hpred[0] >> 4;
	  meta_data[j++] = hpred[0] << 4 | hpred[1] >> 8;
	  meta_data[j++] = hpred[1];
        }
      }
    }
  } else
    fprintf (stderr,_("%s has unknown CAMF type %d.\n"), ifname, type);
}
