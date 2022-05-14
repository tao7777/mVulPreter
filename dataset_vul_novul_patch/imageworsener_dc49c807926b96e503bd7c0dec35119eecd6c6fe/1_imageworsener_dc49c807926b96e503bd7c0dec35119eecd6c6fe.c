IW_IMPL(int) iw_get_input_density(struct iw_context *ctx,
   double *px, double *py, int *pcode)
 {
 	*px = 1.0;
 	*py = 1.0;
	*pcode = ctx->img1.density_code;
	if(ctx->img1.density_code!=IW_DENSITY_UNKNOWN) {
		*px = ctx->img1.density_x;
		*py = ctx->img1.density_y;
		return 1;
 	}
	return 0;
 }
