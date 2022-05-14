IW_IMPL(int) iw_get_input_density(struct iw_context *ctx,
   double *px, double *py, int *pcode)
 {
 	*px = 1.0;
 	*py = 1.0;
	*pcode = IW_DENSITY_UNKNOWN;

	if(ctx->img1.density_code==IW_DENSITY_UNKNOWN) {
		return 0;
 	}
	if(!iw_is_valid_density(ctx->img1.density_x, ctx->img1.density_y,
		ctx->img1.density_code))
	{
		return 0;
	}
	*px = ctx->img1.density_x;
	*py = ctx->img1.density_y;
	*pcode = ctx->img1.density_code;
	return 1;
 }
