BGD_DECLARE(void *) gdImageWebpPtr (gdImagePtr im, int *size)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
 	if (out == NULL) {
 		return NULL;
 	}
	if (_gdImageWebpCtx(im, out, -1)) {
		rv = NULL;
	} else {
		rv = gdDPExtractData(out, size);
	}
 	out->gd_free(out);
 
 	return rv;
}
