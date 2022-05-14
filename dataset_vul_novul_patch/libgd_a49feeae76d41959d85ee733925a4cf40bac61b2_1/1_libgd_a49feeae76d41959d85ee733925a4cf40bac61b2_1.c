BGD_DECLARE(void) gdImageWebpEx (gdImagePtr im, FILE * outFile, int quality)
{
	gdIOCtx *out = gdNewFileCtx(outFile);
 	if (out == NULL) {
 		return;
 	}
	gdImageWebpCtx(im, out, quality);
 	out->gd_free(out);
 }
