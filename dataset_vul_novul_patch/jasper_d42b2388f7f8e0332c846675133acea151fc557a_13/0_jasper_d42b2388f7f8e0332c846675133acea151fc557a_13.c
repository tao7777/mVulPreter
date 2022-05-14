 jas_image_t *jas_image_create0()
{
	jas_image_t *image;

	if (!(image = jas_malloc(sizeof(jas_image_t)))) {
		return 0;
	}

	image->tlx_ = 0;
	image->tly_ = 0;
	image->brx_ = 0;
	image->bry_ = 0;
	image->clrspc_ = JAS_CLRSPC_UNKNOWN;
 	image->numcmpts_ = 0;
 	image->maxcmpts_ = 0;
 	image->cmpts_ = 0;
////	image->inmem_ = true;
 	image->cmprof_ = 0;
 
 	return image;
}
