SplashError Splash::fillImageMask(SplashImageMaskSource src, void *srcData,
				  int w, int h, SplashCoord *mat,
				  GBool glyphMode) {
  SplashPipe pipe;
  GBool rot;
  SplashCoord xScale, yScale, xShear, yShear, yShear1;
  int tx, tx2, ty, ty2, scaledWidth, scaledHeight, xSign, ySign;
  int ulx, uly, llx, lly, urx, ury, lrx, lry;
  int ulx1, uly1, llx1, lly1, urx1, ury1, lrx1, lry1;
  int xMin, xMax, yMin, yMax;
  SplashClipResult clipRes, clipRes2;
  int yp, yq, yt, yStep, lastYStep;
  int xp, xq, xt, xStep, xSrc;
  int k1, spanXMin, spanXMax, spanY;
  SplashColorPtr pixBuf, p;
  int pixAcc;
  int x, y, x1, x2, y2;
  SplashCoord y1;
  int n, m, i, j;

  if (debugMode) {
    printf("fillImageMask: w=%d h=%d mat=[%.2f %.2f %.2f %.2f %.2f %.2f]\n",
	   w, h, (double)mat[0], (double)mat[1], (double)mat[2],
	   (double)mat[3], (double)mat[4], (double)mat[5]);
  }

  if (w == 0 && h == 0) return splashErrZeroImage;

  if (splashAbs(mat[0] * mat[3] - mat[1] * mat[2]) < 0.000001) {
    return splashErrSingularMatrix;
  }

  rot = splashAbs(mat[1]) > splashAbs(mat[0]);
  if (rot) {
    xScale = -mat[1];
    yScale = mat[2] - (mat[0] * mat[3]) / mat[1];
    xShear = -mat[3] / yScale;
    yShear = -mat[0] / mat[1];
  } else {
    xScale = mat[0];
    yScale = mat[3] - (mat[1] * mat[2]) / mat[0];
    xShear = mat[2] / yScale;
    yShear = mat[1] / mat[0];
  }
  if (glyphMode) {
    if (xScale >= 0) {
      tx = splashRound(mat[4]);
      tx2 = splashRound(mat[4] + xScale) - 1;
    } else {
      tx = splashRound(mat[4]) - 1;
      tx2 = splashRound(mat[4] + xScale);
    }
  } else {
    if (xScale >= 0) {
      tx = splashFloor(mat[4] - 0.01);
      tx2 = splashFloor(mat[4] + xScale + 0.01);
    } else {
      tx = splashFloor(mat[4] + 0.01);
      tx2 = splashFloor(mat[4] + xScale - 0.01);
    }
  }
  scaledWidth = abs(tx2 - tx) + 1;
  if (glyphMode) {
    if (yScale >= 0) {
      ty = splashRound(mat[5]);
      ty2 = splashRound(mat[5] + yScale) - 1;
    } else {
      ty = splashRound(mat[5]) - 1;
      ty2 = splashRound(mat[5] + yScale);
    }
  } else {
    if (yScale >= 0) {
      ty = splashFloor(mat[5] - 0.01);
      ty2 = splashFloor(mat[5] + yScale + 0.01);
    } else {
      ty = splashFloor(mat[5] + 0.01);
      ty2 = splashFloor(mat[5] + yScale - 0.01);
    }
  }
  scaledHeight = abs(ty2 - ty) + 1;
  xSign = (xScale < 0) ? -1 : 1;
  ySign = (yScale < 0) ? -1 : 1;
  yShear1 = (SplashCoord)xSign * yShear;

  ulx1 = 0;
  uly1 = 0;
  urx1 = xSign * (scaledWidth - 1);
  ury1 = (int)(yShear * urx1);
  llx1 = splashRound(xShear * ySign * (scaledHeight - 1));
  lly1 = ySign * (scaledHeight - 1) + (int)(yShear * llx1);
  lrx1 = xSign * (scaledWidth - 1) +
           splashRound(xShear * ySign * (scaledHeight - 1));
  lry1 = ySign * (scaledHeight - 1) + (int)(yShear * lrx1);
  if (rot) {
    ulx = tx + uly1;    uly = ty - ulx1;
    urx = tx + ury1;    ury = ty - urx1;
    llx = tx + lly1;    lly = ty - llx1;
    lrx = tx + lry1;    lry = ty - lrx1;
  } else {
    ulx = tx + ulx1;    uly = ty + uly1;
    urx = tx + urx1;    ury = ty + ury1;
    llx = tx + llx1;    lly = ty + lly1;
    lrx = tx + lrx1;    lry = ty + lry1;
  }
  xMin = (ulx < urx) ? (ulx < llx) ? (ulx < lrx) ? ulx : lrx
                                   : (llx < lrx) ? llx : lrx
		     : (urx < llx) ? (urx < lrx) ? urx : lrx
                                   : (llx < lrx) ? llx : lrx;
  xMax = (ulx > urx) ? (ulx > llx) ? (ulx > lrx) ? ulx : lrx
                                   : (llx > lrx) ? llx : lrx
		     : (urx > llx) ? (urx > lrx) ? urx : lrx
                                   : (llx > lrx) ? llx : lrx;
  yMin = (uly < ury) ? (uly < lly) ? (uly < lry) ? uly : lry
                                   : (lly < lry) ? lly : lry
		     : (ury < lly) ? (ury < lry) ? ury : lry
                                   : (lly < lry) ? lly : lry;
  yMax = (uly > ury) ? (uly > lly) ? (uly > lry) ? uly : lry
                                   : (lly > lry) ? lly : lry
		     : (ury > lly) ? (ury > lry) ? ury : lry
                                   : (lly > lry) ? lly : lry;
  clipRes = state->clip->testRect(xMin, yMin, xMax, yMax);
  opClipRes = clipRes;

  yp = h / scaledHeight;
  yq = h % scaledHeight;
  xp = w / scaledWidth;
   xq = w % scaledWidth;
 
  pixBuf = (SplashColorPtr)gmallocn((yp + 1), w);
 
   pipeInit(&pipe, 0, 0, state->fillPattern, NULL, state->fillAlpha,
	   gTrue, gFalse);
  if (vectorAntialias) {
    drawAAPixelInit();
  }

  yt = 0;
  lastYStep = 1;

  for (y = 0; y < scaledHeight; ++y) {

    yStep = yp;
    yt += yq;
    if (yt >= scaledHeight) {
      yt -= scaledHeight;
      ++yStep;
    }

    n = (yp > 0) ? yStep : lastYStep;
    if (n > 0) {
      p = pixBuf;
      for (i = 0; i < n; ++i) {
	(*src)(srcData, p);
	p += w;
      }
    }
    lastYStep = yStep;

    k1 = splashRound(xShear * ySign * y);

    if (clipRes != splashClipAllInside &&
	!rot &&
	(int)(yShear * k1) ==
	  (int)(yShear * (xSign * (scaledWidth - 1) + k1))) {
      if (xSign > 0) {
	spanXMin = tx + k1;
	spanXMax = spanXMin + (scaledWidth - 1);
      } else {
	spanXMax = tx + k1;
	spanXMin = spanXMax - (scaledWidth - 1);
      }
      spanY = ty + ySign * y + (int)(yShear * k1);
      clipRes2 = state->clip->testSpan(spanXMin, spanXMax, spanY);
      if (clipRes2 == splashClipAllOutside) {
	continue;
      }
    } else {
      clipRes2 = clipRes;
    }

    xt = 0;
    xSrc = 0;

    x1 = k1;

    y1 = (SplashCoord)ySign * y + yShear * x1;
    if (yShear1 < 0) {
      y1 += 0.999;
    }

    n = yStep > 0 ? yStep : 1;

    for (x = 0; x < scaledWidth; ++x) {

      xStep = xp;
      xt += xq;
      if (xt >= scaledWidth) {
	xt -= scaledWidth;
	++xStep;
      }

      if (rot) {
	x2 = (int)y1;
	y2 = -x1;
      } else {
	x2 = x1;
	y2 = (int)y1;
      }

      m = xStep > 0 ? xStep : 1;
      p = pixBuf + xSrc;
      pixAcc = 0;
      for (i = 0; i < n; ++i) {
	for (j = 0; j < m; ++j) {
	  pixAcc += *p++;
	}
	p += w - m;
      }

      if (pixAcc != 0) {
	pipe.shape = (pixAcc == n * m)
	                 ? (SplashCoord)1
	                 : (SplashCoord)pixAcc / (SplashCoord)(n * m);
	if (vectorAntialias && clipRes2 != splashClipAllInside) {
	  drawAAPixel(&pipe, tx + x2, ty + y2);
	} else {
	  drawPixel(&pipe, tx + x2, ty + y2, clipRes2 == splashClipAllInside);
	}
      }

      xSrc += xStep;

      x1 += xSign;

      y1 += yShear1;
    }
  }

  gfree(pixBuf);

  return splashOk;
}
