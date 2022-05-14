 void gdImageWBMPCtx (gdImagePtr image, int fg, gdIOCtx * out)
{
	_gdImageWBMPCtx(image, fg, out);
}

/* returns 0 on success, 1 on failure */
static int _gdImageWBMPCtx(gdImagePtr image, int fg, gdIOCtx *out)
 {
 	int x, y, pos;
 	Wbmp *wbmp;
 
 	/* create the WBMP */
 	if ((wbmp = createwbmp (gdImageSX (image), gdImageSY (image), WBMP_WHITE)) == NULL) {
 		gd_error("Could not create WBMP");
		return 1;
 	}
 
 	/* fill up the WBMP structure */
	pos = 0;
	for (y = 0; y < gdImageSY(image); y++) {
		for (x = 0; x < gdImageSX(image); x++) {
			if (gdImageGetPixel (image, x, y) == fg) {
				wbmp->bitmap[pos] = WBMP_BLACK;
			}
			pos++;
		}
	}
 
 	/* write the WBMP to a gd file descriptor */
 	if (writewbmp (wbmp, &gd_putout, out)) {
		freewbmp(wbmp);
 		gd_error("Could not save WBMP");
		return 1;
 	}
 	/* des submitted this bugfix: gdFree the memory. */
 	freewbmp(wbmp);
}
