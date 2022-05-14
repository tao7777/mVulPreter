 static int iw_process_rows_intermediate_to_final(struct iw_context *ctx, int intermed_channel,
 	const struct iw_csdescr *out_csdescr)
 {
	int i,j;
	int z;
	int k;
	int retval=0;
	iw_tmpsample tmpsamp;
	iw_tmpsample alphasamp = 0.0;
	iw_tmpsample *inpix_tofree = NULL; // Used if we need a separate temp buffer for input samples
	iw_tmpsample *outpix_tofree = NULL; // Used if we need a separate temp buffer for output samples
	int using_errdiffdither = 0;
	int output_channel;
	int is_alpha_channel;
	int bkgd_has_transparency;
	double tmpbkgdalpha=0.0;
	int alt_bkgd = 0; // Nonzero if we should use bkgd2 for this sample
	struct iw_resize_settings *rs = NULL;
	int ditherfamily, dithersubtype;
	struct iw_channelinfo_intermed *int_ci;
	struct iw_channelinfo_out *out_ci;

	iw_tmpsample *in_pix = NULL;
 	iw_tmpsample *out_pix = NULL;
 	int num_in_pix;
 	int num_out_pix;
	struct iw_channelinfo_out default_ci_out;
 
 	num_in_pix = ctx->intermed_canvas_width;
 	num_out_pix = ctx->img2.width;
 
 	int_ci = &ctx->intermed_ci[intermed_channel];
 	output_channel = int_ci->corresponding_output_channel;
	if(output_channel>=0) {
		out_ci = &ctx->img2_ci[output_channel];
	}
	else {
		// If there is no output channelinfo struct, create a temporary one to
		// use.
		// TODO: This is admittedly ugly, but we use these settings for a few
		// things even when there is no corresponding output channel, and I
		// don't remember exactly why.
		iw_zeromem(&default_ci_out, sizeof(struct iw_channelinfo_out));
		default_ci_out.channeltype = IW_CHANNELTYPE_NONALPHA;
		out_ci = &default_ci_out;
	}

 	is_alpha_channel = (int_ci->channeltype==IW_CHANNELTYPE_ALPHA);
 	bkgd_has_transparency = iw_bkgd_has_transparency(ctx);
 
	inpix_tofree = (iw_tmpsample*)iw_malloc(ctx, num_in_pix * sizeof(iw_tmpsample));
	in_pix = inpix_tofree;

	outpix_tofree = (iw_tmpsample*)iw_malloc(ctx, num_out_pix * sizeof(iw_tmpsample));
	if(!outpix_tofree) goto done;
	out_pix = outpix_tofree;

	if(ctx->nearest_color_table && !is_alpha_channel &&
	   out_ci->ditherfamily==IW_DITHERFAMILY_NONE &&
	   out_ci->color_count==0)
	{
		out_ci->use_nearest_color_table = 1;
	}
	else {
		out_ci->use_nearest_color_table = 0;
	}

	ditherfamily = out_ci->ditherfamily;
	dithersubtype = out_ci->dithersubtype;
	if(ditherfamily==IW_DITHERFAMILY_RANDOM) {
		if(dithersubtype==IW_DITHERSUBTYPE_SAMEPATTERN && out_ci->channeltype!=IW_CHANNELTYPE_ALPHA)
		{
			iwpvt_prng_set_random_seed(ctx->prng,ctx->random_seed);
		}
		else {
			iwpvt_prng_set_random_seed(ctx->prng,ctx->random_seed+out_ci->channeltype);
		}
	}

	if(output_channel>=0 && out_ci->ditherfamily==IW_DITHERFAMILY_ERRDIFF) {
		using_errdiffdither = 1;
		for(i=0;i<ctx->img2.width;i++) {
			for(k=0;k<IW_DITHER_MAXROWS;k++) {
				ctx->dither_errors[k][i] = 0.0;
			}
		}
	}

	rs=&ctx->resize_settings[IW_DIMENSION_H];

	if(!rs->rrctx) {
		rs->rrctx = iwpvt_resize_rows_init(ctx,rs,int_ci->channeltype,
			num_in_pix, num_out_pix);
		if(!rs->rrctx) goto done;
	}

	for(j=0;j<ctx->intermed_canvas_height;j++) {

		if(is_alpha_channel) {
			for(i=0;i<num_in_pix;i++) {
				inpix_tofree[i] = ctx->intermediate_alpha32[((size_t)j)*ctx->intermed_canvas_width+i];
			}
		}
		else {
			for(i=0;i<num_in_pix;i++) {
				inpix_tofree[i] = ctx->intermediate32[((size_t)j)*ctx->intermed_canvas_width+i];
			}
		}

		iwpvt_resize_row_main(rs->rrctx,in_pix,out_pix);

		if(ctx->intclamp)
			clamp_output_samples(ctx,out_pix,num_out_pix);

		if(is_alpha_channel && outpix_tofree && ctx->final_alpha32) {
			for(i=0;i<num_out_pix;i++) {
				ctx->final_alpha32[((size_t)j)*ctx->img2.width+i] = (iw_float32)outpix_tofree[i];
			}
		}


		if(output_channel == -1) {
			goto here;
		}

		for(z=0;z<ctx->img2.width;z++) {
			if(using_errdiffdither && (j%2))
				i=ctx->img2.width-1-z;
			else
				i=z;

			tmpsamp = out_pix[i];

			if(ctx->bkgd_checkerboard) {
				alt_bkgd = (((ctx->bkgd_check_origin[IW_DIMENSION_H]+i)/ctx->bkgd_check_size)%2) !=
					(((ctx->bkgd_check_origin[IW_DIMENSION_V]+j)/ctx->bkgd_check_size)%2);
			}

			if(bkgd_has_transparency) {
				tmpbkgdalpha = alt_bkgd ? ctx->bkgd2alpha : ctx->bkgd1alpha;
			}

			if(int_ci->need_unassoc_alpha_processing) {
				alphasamp = ctx->final_alpha32[((size_t)j)*ctx->img2.width + i];

				if(alphasamp!=0.0) {
					tmpsamp /= alphasamp;
				}

				if(ctx->apply_bkgd && ctx->apply_bkgd_strategy==IW_BKGD_STRATEGY_LATE) {
					double bkcolor;
					bkcolor = alt_bkgd ? out_ci->bkgd2_color_lin : out_ci->bkgd1_color_lin;

					if(bkgd_has_transparency) {
						tmpsamp = tmpsamp*alphasamp + bkcolor*tmpbkgdalpha*(1.0-alphasamp);
					}
					else {
						tmpsamp = tmpsamp*alphasamp + bkcolor*(1.0-alphasamp);
					}
				}
			}
			else if(is_alpha_channel && bkgd_has_transparency) {
				tmpsamp = tmpsamp + tmpbkgdalpha*(1.0-tmpsamp);
			}

			if(ctx->img2.sampletype==IW_SAMPLETYPE_FLOATINGPOINT)
				put_sample_convert_from_linear_flt(ctx,tmpsamp,i,j,output_channel,out_csdescr);
			else
				put_sample_convert_from_linear(ctx,tmpsamp,i,j,output_channel,out_csdescr);

		}

		if(using_errdiffdither) {
			for(i=0;i<ctx->img2.width;i++) {
				for(k=0;k<IW_DITHER_MAXROWS-1;k++) {
					ctx->dither_errors[k][i] = ctx->dither_errors[k+1][i];
				}
				ctx->dither_errors[IW_DITHER_MAXROWS-1][i] = 0.0;
			}
		}

here:
		;
	}

	retval=1;

done:
	if(rs && rs->disable_rrctx_cache && rs->rrctx) {
		iwpvt_resize_rows_done(rs->rrctx);
		rs->rrctx = NULL;
	}
	if(inpix_tofree) iw_free(ctx,inpix_tofree);
	if(outpix_tofree) iw_free(ctx,outpix_tofree);

	return retval;
}
