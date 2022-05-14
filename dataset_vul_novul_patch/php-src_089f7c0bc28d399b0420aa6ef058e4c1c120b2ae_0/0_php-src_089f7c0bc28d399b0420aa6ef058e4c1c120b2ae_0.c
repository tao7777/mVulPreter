 void * gdImageGifPtr (gdImagePtr im, int *size)
 {
   void *rv;
   gdIOCtx *out = gdNewDynamicCtx (2048, NULL);
	if (!_gdImageGifCtx(im, out)) {
		rv = gdDPExtractData(out, size);
	} else {
		rv = NULL;
	}
   out->gd_free (out);
   return rv;
 }
