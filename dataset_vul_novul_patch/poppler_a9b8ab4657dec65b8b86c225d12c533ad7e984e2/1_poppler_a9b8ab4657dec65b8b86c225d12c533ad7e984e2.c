void Splash::arbitraryTransformMask(SplashImageMaskSource src, void *srcData,
				    int srcWidth, int srcHeight,
				    SplashCoord *mat, GBool glyphMode) {
  SplashBitmap *scaledMask;
  SplashClipResult clipRes, clipRes2;
  SplashPipe pipe;
  int scaledWidth, scaledHeight, t0, t1;
  SplashCoord r00, r01, r10, r11, det, ir00, ir01, ir10, ir11;
  SplashCoord vx[4], vy[4];
  int xMin, yMin, xMax, yMax;
  ImageSection section[3];
  int nSections;
  int y, xa, xb, x, i, xx, yy;

  vx[0] = mat[4];                    vy[0] = mat[5];
  vx[1] = mat[2] + mat[4];           vy[1] = mat[3] + mat[5];
  vx[2] = mat[0] + mat[2] + mat[4];  vy[2] = mat[1] + mat[3] + mat[5];
  vx[3] = mat[0] + mat[4];           vy[3] = mat[1] + mat[5];

  xMin = imgCoordMungeLowerC(vx[0], glyphMode);
  xMax = imgCoordMungeUpperC(vx[0], glyphMode);
  yMin = imgCoordMungeLowerC(vy[0], glyphMode);
  yMax = imgCoordMungeUpperC(vy[0], glyphMode);
  for (i = 1; i < 4; ++i) {
    t0 = imgCoordMungeLowerC(vx[i], glyphMode);
    if (t0 < xMin) {
      xMin = t0;
    }
    t0 = imgCoordMungeUpperC(vx[i], glyphMode);
    if (t0 > xMax) {
      xMax = t0;
    }
    t1 = imgCoordMungeLowerC(vy[i], glyphMode);
    if (t1 < yMin) {
      yMin = t1;
    }
    t1 = imgCoordMungeUpperC(vy[i], glyphMode);
    if (t1 > yMax) {
      yMax = t1;
    }
  }
  clipRes = state->clip->testRect(xMin, yMin, xMax - 1, yMax - 1);
  opClipRes = clipRes;
  if (clipRes == splashClipAllOutside) {
    return;
  }

  if (mat[0] >= 0) {
    t0 = imgCoordMungeUpperC(mat[0] + mat[4], glyphMode) -
         imgCoordMungeLowerC(mat[4], glyphMode);
  } else {
    t0 = imgCoordMungeUpperC(mat[4], glyphMode) -
         imgCoordMungeLowerC(mat[0] + mat[4], glyphMode);
  }
  if (mat[1] >= 0) {
    t1 = imgCoordMungeUpperC(mat[1] + mat[5], glyphMode) -
         imgCoordMungeLowerC(mat[5], glyphMode);
  } else {
    t1 = imgCoordMungeUpperC(mat[5], glyphMode) -
         imgCoordMungeLowerC(mat[1] + mat[5], glyphMode);
  }
  scaledWidth = t0 > t1 ? t0 : t1;
  if (mat[2] >= 0) {
    t0 = imgCoordMungeUpperC(mat[2] + mat[4], glyphMode) -
         imgCoordMungeLowerC(mat[4], glyphMode);
  } else {
    t0 = imgCoordMungeUpperC(mat[4], glyphMode) -
         imgCoordMungeLowerC(mat[2] + mat[4], glyphMode);
  }
  if (mat[3] >= 0) {
    t1 = imgCoordMungeUpperC(mat[3] + mat[5], glyphMode) -
         imgCoordMungeLowerC(mat[5], glyphMode);
  } else {
    t1 = imgCoordMungeUpperC(mat[5], glyphMode) -
         imgCoordMungeLowerC(mat[3] + mat[5], glyphMode);
  }
  scaledHeight = t0 > t1 ? t0 : t1;
  if (scaledWidth == 0) {
    scaledWidth = 1;
  }
  if (scaledHeight == 0) {
    scaledHeight = 1;
  }

  r00 = mat[0] / scaledWidth;
  r01 = mat[1] / scaledWidth;
  r10 = mat[2] / scaledHeight;
  r11 = mat[3] / scaledHeight;
  det = r00 * r11 - r01 * r10;
  if (splashAbs(det) < 1e-6) {
    return;
  }
  ir00 = r11 / det;
  ir01 = -r01 / det;
  ir10 = -r10 / det;
  ir11 = r00 / det;

   scaledMask = scaleMask(src, srcData, srcWidth, srcHeight,
 			 scaledWidth, scaledHeight);
   if (scaledMask->data == NULL) {
    error(errInternal, -1, "scaledMask->data is NULL in Splash::scaleMaskYuXu");
     delete scaledMask;
     return;
   }

  i = (vy[2] <= vy[3]) ? 2 : 3;
  if (vy[1] <= vy[i]) {
    i = 1;
  }
  if (vy[0] < vy[i] || (i != 3 && vy[0] == vy[i])) {
    i = 0;
  }
  if (vy[i] == vy[(i+1) & 3]) {
    section[0].y0 = imgCoordMungeLowerC(vy[i], glyphMode);
    section[0].y1 = imgCoordMungeUpperC(vy[(i+2) & 3], glyphMode) - 1;
    if (vx[i] < vx[(i+1) & 3]) {
      section[0].ia0 = i;
      section[0].ia1 = (i+3) & 3;
      section[0].ib0 = (i+1) & 3;
      section[0].ib1 = (i+2) & 3;
    } else {
      section[0].ia0 = (i+1) & 3;
      section[0].ia1 = (i+2) & 3;
      section[0].ib0 = i;
      section[0].ib1 = (i+3) & 3;
    }
    nSections = 1;
  } else {
    section[0].y0 = imgCoordMungeLowerC(vy[i], glyphMode);
    section[2].y1 = imgCoordMungeUpperC(vy[(i+2) & 3], glyphMode) - 1;
    section[0].ia0 = section[0].ib0 = i;
    section[2].ia1 = section[2].ib1 = (i+2) & 3;
    if (vx[(i+1) & 3] < vx[(i+3) & 3]) {
      section[0].ia1 = section[2].ia0 = (i+1) & 3;
      section[0].ib1 = section[2].ib0 = (i+3) & 3;
    } else {
      section[0].ia1 = section[2].ia0 = (i+3) & 3;
      section[0].ib1 = section[2].ib0 = (i+1) & 3;
    }
    if (vy[(i+1) & 3] < vy[(i+3) & 3]) {
      section[1].y0 = imgCoordMungeLowerC(vy[(i+1) & 3], glyphMode);
      section[2].y0 = imgCoordMungeUpperC(vy[(i+3) & 3], glyphMode);
      if (vx[(i+1) & 3] < vx[(i+3) & 3]) {
	section[1].ia0 = (i+1) & 3;
	section[1].ia1 = (i+2) & 3;
	section[1].ib0 = i;
	section[1].ib1 = (i+3) & 3;
      } else {
	section[1].ia0 = i;
	section[1].ia1 = (i+3) & 3;
	section[1].ib0 = (i+1) & 3;
	section[1].ib1 = (i+2) & 3;
      }
    } else {
      section[1].y0 = imgCoordMungeLowerC(vy[(i+3) & 3], glyphMode);
      section[2].y0 = imgCoordMungeUpperC(vy[(i+1) & 3], glyphMode);
      if (vx[(i+1) & 3] < vx[(i+3) & 3]) {
	section[1].ia0 = i;
	section[1].ia1 = (i+1) & 3;
	section[1].ib0 = (i+3) & 3;
	section[1].ib1 = (i+2) & 3;
      } else {
	section[1].ia0 = (i+3) & 3;
	section[1].ia1 = (i+2) & 3;
	section[1].ib0 = i;
	section[1].ib1 = (i+1) & 3;
      }
    }
    section[0].y1 = section[1].y0 - 1;
    section[1].y1 = section[2].y0 - 1;
    nSections = 3;
  }
  for (i = 0; i < nSections; ++i) {
    section[i].xa0 = vx[section[i].ia0];
    section[i].ya0 = vy[section[i].ia0];
    section[i].xa1 = vx[section[i].ia1];
    section[i].ya1 = vy[section[i].ia1];
    section[i].xb0 = vx[section[i].ib0];
    section[i].yb0 = vy[section[i].ib0];
    section[i].xb1 = vx[section[i].ib1];
    section[i].yb1 = vy[section[i].ib1];
    section[i].dxdya = (section[i].xa1 - section[i].xa0) /
                       (section[i].ya1 - section[i].ya0);
    section[i].dxdyb = (section[i].xb1 - section[i].xb0) /
                       (section[i].yb1 - section[i].yb0);
  }

  pipeInit(&pipe, 0, 0, state->fillPattern, NULL,
	   (Guchar)splashRound(state->fillAlpha * 255), gTrue, gFalse);
  if (vectorAntialias) {
    drawAAPixelInit();
  }

  if (nSections == 1) {
    if (section[0].y0 == section[0].y1) {
      ++section[0].y1;
      clipRes = opClipRes = splashClipPartial;
    }
  } else {
    if (section[0].y0 == section[2].y1) {
      ++section[1].y1;
      clipRes = opClipRes = splashClipPartial;
    }
  }

  for (i = 0; i < nSections; ++i) {
    for (y = section[i].y0; y <= section[i].y1; ++y) {
      xa = imgCoordMungeLowerC(section[i].xa0 +
			         ((SplashCoord)y + 0.5 - section[i].ya0) *
			           section[i].dxdya,
			       glyphMode);
      xb = imgCoordMungeUpperC(section[i].xb0 +
			         ((SplashCoord)y + 0.5 - section[i].yb0) *
			           section[i].dxdyb,
			       glyphMode);
      if (xa == xb) {
	++xb;
      }
      if (clipRes != splashClipAllInside) {
	clipRes2 = state->clip->testSpan(xa, xb - 1, y);
      } else {
	clipRes2 = clipRes;
      }
      for (x = xa; x < xb; ++x) {
	xx = splashFloor(((SplashCoord)x + 0.5 - mat[4]) * ir00 +
			 ((SplashCoord)y + 0.5 - mat[5]) * ir10);
	yy = splashFloor(((SplashCoord)x + 0.5 - mat[4]) * ir01 +
			 ((SplashCoord)y + 0.5 - mat[5]) * ir11);
	if (xx < 0) {
	  xx = 0;
	} else if (xx >= scaledWidth) {
	  xx = scaledWidth - 1;
	}
	if (yy < 0) {
	  yy = 0;
	} else if (yy >= scaledHeight) {
	  yy = scaledHeight - 1;
	}
	pipe.shape = scaledMask->data[yy * scaledWidth + xx];
	if (vectorAntialias && clipRes2 != splashClipAllInside) {
	  drawAAPixel(&pipe, x, y);
	} else {
	  drawPixel(&pipe, x, y, clipRes2 == splashClipAllInside);
	}
      }
    }
  }

  delete scaledMask;
}
