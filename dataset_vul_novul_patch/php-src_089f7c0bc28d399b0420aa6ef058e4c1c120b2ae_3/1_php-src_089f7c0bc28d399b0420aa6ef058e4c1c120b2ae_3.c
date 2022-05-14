 void gdImageWBMPCtx (gdImagePtr image, int fg, gdIOCtx * out)
 {
 	int x, y, pos;
 	Wbmp *wbmp;
 
 	/* create the WBMP */
 	if ((wbmp = createwbmp (gdImageSX (image), gdImageSY (image), WBMP_WHITE)) == NULL) {
 		gd_error("Could not create WBMP");
		return;
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
 		gd_error("Could not save WBMP");
 	}
 	/* des submitted this bugfix: gdFree the memory. */
 	freewbmp(wbmp);
}
