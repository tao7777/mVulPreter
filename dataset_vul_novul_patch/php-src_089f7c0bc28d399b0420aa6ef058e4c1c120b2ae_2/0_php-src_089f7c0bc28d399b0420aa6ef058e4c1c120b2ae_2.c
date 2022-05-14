void *gdImageJpegPtr (gdImagePtr im, int *size, int quality)
 {
 	void *rv;
 	gdIOCtx *out = gdNewDynamicCtx (2048, NULL);
	if (!_gdImageJpegCtx(im, out, quality)) {
		rv = gdDPExtractData(out, size);
	} else {
		rv = NULL;
	}
 	out->gd_free (out);
 
 	return rv;
}
