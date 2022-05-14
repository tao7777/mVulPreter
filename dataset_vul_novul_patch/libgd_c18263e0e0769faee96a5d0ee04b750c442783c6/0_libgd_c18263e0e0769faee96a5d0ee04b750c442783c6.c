void gdImageWebpCtx (gdImagePtr im, gdIOCtx * outfile, int quantization)
{
	uint8_t *argb;
	int x, y;
	uint8_t *p;
	uint8_t *out;
	size_t out_size;

	if (im == NULL) {
		return;
	}

	if (!gdImageTrueColor(im)) {
		zend_error(E_ERROR, "Paletter image not supported by webp");
		return;
	}

	if (quantization == -1) {
 		quantization = 80;
 	}
 
	if (overflow2(gdImageSX(im), 4)) {
		return;
	}

	if (overflow2(gdImageSX(im) * 4, gdImageSY(im))) {
		return;
	}

 	argb = (uint8_t *)gdMalloc(gdImageSX(im) * 4 * gdImageSY(im));
 	if (!argb) {
 		return;
	}
	p = argb;
	for (y = 0; y < gdImageSY(im); y++) {
		for (x = 0; x < gdImageSX(im); x++) {
			register int c;
			register char a;
			c = im->tpixels[y][x];
			a = gdTrueColorGetAlpha(c);
			if (a == 127) {
				a = 0;
			} else {
				a = 255 - ((a << 1) + (a >> 6));
			}
			*(p++) = gdTrueColorGetRed(c);
			*(p++) = gdTrueColorGetGreen(c);
			*(p++) = gdTrueColorGetBlue(c); 
			*(p++) = a;
		}
	}
	out_size = WebPEncodeRGBA(argb, gdImageSX(im), gdImageSY(im), gdImageSX(im) * 4, quantization, &out);
	if (out_size == 0) {
		zend_error(E_ERROR, "gd-webp encoding failed");
		goto freeargb;
	}
	gdPutBuf(out, out_size, outfile);
	free(out);

freeargb:
	gdFree(argb);
}
