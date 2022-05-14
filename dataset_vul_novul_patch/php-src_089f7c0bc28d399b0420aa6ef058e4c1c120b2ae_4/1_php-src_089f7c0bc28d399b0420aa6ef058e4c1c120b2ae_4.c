void * gdImageWBMPPtr (gdImagePtr im, int *size, int fg)
 {
 	void *rv;
 	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	gdImageWBMPCtx(im, fg, out);
	rv = gdDPExtractData(out, size);
 	out->gd_free(out);
 
 	return rv;
}
