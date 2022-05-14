BGD_DECLARE(void) gdImageWebp (gdImagePtr im, FILE * outFile)
{
	gdIOCtx *out = gdNewFileCtx(outFile);
 	if (out == NULL) {
 		return;
 	}
	gdImageWebpCtx(im, out, -1);
 	out->gd_free(out);
 }
