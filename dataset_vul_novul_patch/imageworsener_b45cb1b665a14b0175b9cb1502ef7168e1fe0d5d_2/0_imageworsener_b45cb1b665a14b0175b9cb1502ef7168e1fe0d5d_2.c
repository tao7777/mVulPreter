static void iwjpeg_scan_exif_ifd(struct iwjpegrcontext *rctx,
	struct iw_exif_state *e, iw_uint32 ifd)
{
	unsigned int tag_count;
	unsigned int i;
	unsigned int tag_pos;
	unsigned int tag_id;
 	unsigned int v;
 	double v_dbl;
 
	if(ifd<8 || e->d_len<18 || ifd>e->d_len-18) return;
 
	tag_count = get_exif_ui16(e, ifd);
 	if(tag_count>1000) return; // Sanity check.
 
 	for(i=0;i<tag_count;i++) {
 		tag_pos = ifd+2+i*12;
 		if(tag_pos+12 > e->d_len) return; // Avoid overruns.
		tag_id = get_exif_ui16(e, tag_pos);
 
 		switch(tag_id) {
 		case 274: // 274 = Orientation
			if(get_exif_tag_int_value(e,tag_pos,&v)) {
				rctx->exif_orientation = v;
			}
			break;

		case 296: // 296 = ResolutionUnit
			if(get_exif_tag_int_value(e,tag_pos,&v)) {
				rctx->exif_density_unit = v;
			}
			break;

		case 282: // 282 = XResolution
			if(get_exif_tag_dbl_value(e,tag_pos,&v_dbl)) {
				rctx->exif_density_x = v_dbl;
			}
			break;

		case 283: // 283 = YResolution
			if(get_exif_tag_dbl_value(e,tag_pos,&v_dbl)) {
				rctx->exif_density_y = v_dbl;
			}
			break;
		}
	}
}
