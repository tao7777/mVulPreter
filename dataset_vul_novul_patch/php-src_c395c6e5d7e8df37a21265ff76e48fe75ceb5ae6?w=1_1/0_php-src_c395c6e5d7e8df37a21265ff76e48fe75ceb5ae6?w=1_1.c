void gdImageCopyResized (gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int dstW, int dstH, int srcW, int srcH)
{
	int c;
	int x, y;
	int tox, toy;
	int ydest;
	int i;
 	int colorMap[gdMaxColors];
 	/* Stretch vectors */
 	int *stx, *sty;

 	if (overflow2(sizeof(int), srcW)) {
 		return;
 	}
	if (overflow2(sizeof(int), srcH)) {
		return;
	}

	stx = (int *) gdMalloc (sizeof (int) * srcW);
	sty = (int *) gdMalloc (sizeof (int) * srcH);

	/* Fixed by Mao Morimoto 2.0.16 */
	for (i = 0; (i < srcW); i++) {
		stx[i] = dstW * (i+1) / srcW - dstW * i / srcW ;
	}
	for (i = 0; (i < srcH); i++) {
		sty[i] = dstH * (i+1) / srcH - dstH * i / srcH ;
	}
	for (i = 0; (i < gdMaxColors); i++) {
		colorMap[i] = (-1);
	}
	toy = dstY;
	for (y = srcY; (y < (srcY + srcH)); y++) {
		for (ydest = 0; (ydest < sty[y - srcY]); ydest++) {
			tox = dstX;
			for (x = srcX; (x < (srcX + srcW)); x++) {
				int nc = 0;
				int mapTo;
				if (!stx[x - srcX]) {
					continue;
				}
				if (dst->trueColor) {
					/* 2.0.9: Thorben Kundinger: Maybe the source image is not a truecolor image */
					if (!src->trueColor) {
					  	int tmp = gdImageGetPixel (src, x, y);
		  				mapTo = gdImageGetTrueColorPixel (src, x, y);
					  	if (gdImageGetTransparent (src) == tmp) {
							/* 2.0.21, TK: not tox++ */
							tox += stx[x - srcX];
					  		continue;
					  	}
					} else {
						/* TK: old code follows */
					  	mapTo = gdImageGetTrueColorPixel (src, x, y);
						/* Added 7/24/95: support transparent copies */
						if (gdImageGetTransparent (src) == mapTo) {
							/* 2.0.21, TK: not tox++ */
							tox += stx[x - srcX];
							continue;
						}
					}
				} else {
					c = gdImageGetPixel (src, x, y);
					/* Added 7/24/95: support transparent copies */
					if (gdImageGetTransparent (src) == c) {
					      tox += stx[x - srcX];
					      continue;
					}
					if (src->trueColor) {
					      /* Remap to the palette available in the destination image. This is slow and works badly. */
					      mapTo = gdImageColorResolveAlpha(dst, gdTrueColorGetRed(c),
					      					    gdTrueColorGetGreen(c),
					      					    gdTrueColorGetBlue(c),
					      					    gdTrueColorGetAlpha (c));
					} else {
						/* Have we established a mapping for this color? */
						if (colorMap[c] == (-1)) {
							/* If it's the same image, mapping is trivial */
							if (dst == src) {
								nc = c;
							} else {
								/* Find or create the best match */
								/* 2.0.5: can't use gdTrueColorGetRed, etc with palette */
								nc = gdImageColorResolveAlpha(dst, gdImageRed(src, c),
												   gdImageGreen(src, c),
												   gdImageBlue(src, c),
												   gdImageAlpha(src, c));
							}
							colorMap[c] = nc;
						}
						mapTo = colorMap[c];
					}
				}
				for (i = 0; (i < stx[x - srcX]); i++) {
					gdImageSetPixel (dst, tox, toy, mapTo);
					tox++;
				}
			}
			toy++;
		}
	}
	gdFree (stx);
	gdFree (sty);
}
