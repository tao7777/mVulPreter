BGD_DECLARE(gdImagePtr) gdImageCreate (int sx, int sy)
{
	int i;
	gdImagePtr im;

 	if (overflow2(sx, sy)) {
 		return NULL;
 	}
 	if (overflow2(sizeof (unsigned char *), sy)) {
 		return NULL;
 	}
	if (overflow2(sizeof (unsigned char), sx)) {
		return NULL;
	}

	im = (gdImage *) gdCalloc(1, sizeof(gdImage));
	if (!im) {
		return NULL;
	}

	/* Row-major ever since gd 1.3 */
	im->pixels = (unsigned char **) gdMalloc (sizeof (unsigned char *) * sy);
	if (!im->pixels) {
		gdFree(im);
		return NULL;
	}

	im->polyInts = 0;
	im->polyAllocated = 0;
	im->brush = 0;
	im->tile = 0;
	im->style = 0;
	for (i = 0; (i < sy); i++) {
		/* Row-major ever since gd 1.3 */
		im->pixels[i] = (unsigned char *) gdCalloc (sx, sizeof (unsigned char));
		if (!im->pixels[i]) {
			for (--i ; i >= 0; i--) {
				gdFree(im->pixels[i]);
			}
			gdFree(im->pixels);
			gdFree(im);
			return NULL;
		}

	}
	im->sx = sx;
	im->sy = sy;
	im->colorsTotal = 0;
	im->transparent = (-1);
	im->interlace = 0;
	im->thick = 1;
	im->AA = 0;
	for (i = 0; (i < gdMaxColors); i++) {
		im->open[i] = 1;
	};
	im->trueColor = 0;
	im->tpixels = 0;
	im->cx1 = 0;
	im->cy1 = 0;
	im->cx2 = im->sx - 1;
	im->cy2 = im->sy - 1;
	im->res_x = GD_RESOLUTION;
	im->res_y = GD_RESOLUTION;
	im->interpolation = NULL;
	im->interpolation_id = GD_BILINEAR_FIXED;
	return im;
}
