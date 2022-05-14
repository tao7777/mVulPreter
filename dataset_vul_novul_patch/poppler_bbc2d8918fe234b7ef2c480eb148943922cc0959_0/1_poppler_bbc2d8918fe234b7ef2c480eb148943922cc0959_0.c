void Splash::vertFlipImage(SplashBitmap *img, int width, int height,
			   int nComps) {
  Guchar *lineBuf;
  Guchar *p0, *p1;
  int w;

  w = width * nComps;
   Guchar *lineBuf;
   Guchar *p0, *p1;
   int w;
 
   w = width * nComps;
   lineBuf = (Guchar *)gmalloc(w);
	 p0 += width, p1 -= width) {
      memcpy(lineBuf, p0, width);
      memcpy(p0, p1, width);
      memcpy(p1, lineBuf, width);
    }
  }
