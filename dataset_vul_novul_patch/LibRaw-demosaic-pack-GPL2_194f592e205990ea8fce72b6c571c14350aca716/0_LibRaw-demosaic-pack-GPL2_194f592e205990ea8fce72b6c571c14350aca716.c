 void CLASS foveon_dp_load_raw()
 {
   unsigned c, roff[4], row, col, diff;
  ushort huff[1024], vpred[2][2], hpred[2];
 
   fseek (ifp, 8, SEEK_CUR);
   foveon_huff (huff);
  roff[0] = 48;
  FORC3 roff[c+1] = -(-(roff[c] + get4()) & -16);
  FORC3 {
    fseek (ifp, data_offset+roff[c], SEEK_SET);
    getbits(-1);
    vpred[0][0] = vpred[0][1] = vpred[1][0] = vpred[1][1] = 512;
    for (row=0; row < height; row++) {
#ifdef LIBRAW_LIBRARY_BUILD
    checkCancel();
#endif
      for (col=0; col < width; col++) {
	diff = ljpeg_diff(huff);
	if (col < 2) hpred[col] = vpred[row & 1][col] += diff;
	else hpred[col & 1] += diff;
	image[row*width+col][c] = hpred[col & 1];
      }
    }
  }
}
