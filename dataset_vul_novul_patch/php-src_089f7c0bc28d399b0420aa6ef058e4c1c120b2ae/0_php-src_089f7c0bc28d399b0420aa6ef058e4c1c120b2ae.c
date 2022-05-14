 void gdImageGifCtx(gdImagePtr im, gdIOCtxPtr out)
{
	_gdImageGifCtx(im, out);
}

/* returns 0 on success, 1 on failure */
static int _gdImageGifCtx(gdImagePtr im, gdIOCtxPtr out)
 {
 	gdImagePtr pim = 0, tim = im;
 	int interlace, BitsPerPixel;
	interlace = im->interlace;
	if (im->trueColor) {
		/* Expensive, but the only way that produces an
			acceptable result: mix down to a palette
 			based temporary image. */
 		pim = gdImageCreatePaletteFromTrueColor(im, 1, 256);
 		if (!pim) {
			return 1;
 		}
 		tim = pim;
 	}
	BitsPerPixel = colorstobpp(tim->colorsTotal);
	/* All set, let's do it. */
	GIFEncode(
		out, tim->sx, tim->sy, tim->interlace, 0, tim->transparent, BitsPerPixel,
		tim->red, tim->green, tim->blue, tim);
	if (pim) {
 		/* Destroy palette based temporary image. */
 		gdImageDestroy(	pim);
 	}

    return 0;
 }
