static int bmpr_read_rle(struct iwbmprcontext *rctx)
{
	int retval = 0;

	if(!(rctx->compression==IWBMP_BI_RLE8 && rctx->bitcount==8) &&
 		!(rctx->compression==IWBMP_BI_RLE4 && rctx->bitcount==4))
 	{
 		iw_set_error(rctx->ctx,"Compression type incompatible with image type");
 	}
 
 	if(rctx->topdown) {
 		iw_set_error(rctx->ctx,"Compression not allowed with top-down images");
 	}
 
	rctx->img->imgtype = IW_IMGTYPE_RGBA;
	rctx->img->bit_depth = 8;
	rctx->img->bpr = iw_calc_bytesperrow(rctx->width,32);

	rctx->img->pixels = (iw_byte*)iw_malloc_large(rctx->ctx,rctx->img->bpr,rctx->img->height);
	if(!rctx->img->pixels) goto done;

	if(!bmpr_read_rle_internal(rctx)) goto done;

	if(!bmpr_has_transparency(rctx->img)) {
		bmpr_strip_alpha(rctx->img);
	}

	retval = 1;
done:
	return retval;
}
