static int iwgif_read_image(struct iwgifrcontext *rctx)
{
	int retval=0;
	struct lzwdeccontext d;
	size_t subblocksize;
	int has_local_ct;
	int local_ct_size;

	unsigned int root_codesize;

	if(!iwgif_read(rctx,rctx->rbuf,9)) goto done;

	rctx->image_left = (int)iw_get_ui16le(&rctx->rbuf[0]);
	rctx->image_top = (int)iw_get_ui16le(&rctx->rbuf[2]);
 
 	rctx->image_width = (int)iw_get_ui16le(&rctx->rbuf[4]);
 	rctx->image_height = (int)iw_get_ui16le(&rctx->rbuf[6]);
	if(rctx->image_width<1 || rctx->image_height<1) {
		iw_set_error(rctx->ctx, "Invalid image dimensions");
		goto done;
	}
 
 	rctx->interlaced = (int)((rctx->rbuf[8]>>6)&0x01);
 
	has_local_ct = (int)((rctx->rbuf[8]>>7)&0x01);
	if(has_local_ct) {
		local_ct_size = (int)(rctx->rbuf[8]&0x07);
		rctx->colortable.num_entries = 1<<(1+local_ct_size);
	}

	if(has_local_ct) {
		if(!iwgif_read_color_table(rctx,&rctx->colortable)) goto done;
	}

	if(rctx->has_transparency) {
	    rctx->colortable.entry[rctx->trans_color_index].a = 0;
	}

	if(!iwgif_read(rctx,rctx->rbuf,1)) goto done;
	root_codesize = (unsigned int)rctx->rbuf[0];

	if(root_codesize<2 || root_codesize>11) {
		iw_set_error(rctx->ctx,"Invalid LZW minimum code size");
		goto done;
	}

	if(!iwgif_init_screen(rctx)) goto done;

	rctx->total_npixels = (size_t)rctx->image_width * (size_t)rctx->image_height;

	if(!iwgif_make_row_pointers(rctx)) goto done;

	lzw_init(&d,root_codesize);
	lzw_clear(&d);

	while(1) {
		if(!iwgif_read(rctx,rctx->rbuf,1)) goto done;
		subblocksize = (size_t)rctx->rbuf[0];
		if(subblocksize==0) break;

		if(!iwgif_read(rctx,rctx->rbuf,subblocksize)) goto done;
		if(!lzw_process_bytes(rctx,&d,rctx->rbuf,subblocksize)) goto done;

		if(d.eoi_flag) break;

		if(rctx->pixels_set >= rctx->total_npixels) break;
	}

	retval=1;

done:
	return retval;
}
