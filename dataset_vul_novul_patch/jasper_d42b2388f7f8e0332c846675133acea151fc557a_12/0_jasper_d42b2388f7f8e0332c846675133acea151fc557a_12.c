jas_image_t *jas_image_create(int numcmpts, jas_image_cmptparm_t *cmptparms,
   int clrspc)
 {
 	jas_image_t *image;
	size_t rawsize;
 	uint_fast32_t inmem;
 	int cmptno;
 	jas_image_cmptparm_t *cmptparm;
 
	image = 0;

	JAS_DBGLOG(100, ("jas_image_create(%d, %p, %d)\n", numcmpts, cmptparms,
	  clrspc));

 	if (!(image = jas_image_create0())) {
		goto error;
 	}
 
 	image->clrspc_ = clrspc;
 	image->maxcmpts_ = numcmpts;
////	image->inmem_ = true;
 
 	/* Allocate memory for the per-component information. */
 	if (!(image->cmpts_ = jas_alloc2(image->maxcmpts_,
 	  sizeof(jas_image_cmpt_t *)))) {
		goto error;
 	}
 	/* Initialize in case of failure. */
 	for (cmptno = 0; cmptno < image->maxcmpts_; ++cmptno) {
 		image->cmpts_[cmptno] = 0;
 	}
 
#if 0
 	/* Compute the approximate raw size of the image. */
 	rawsize = 0;
 	for (cmptno = 0, cmptparm = cmptparms; cmptno < numcmpts; ++cmptno,
	  ++cmptparm) {
		rawsize += cmptparm->width * cmptparm->height *
		  (cmptparm->prec + 7) / 8;
	}
 	/* Decide whether to buffer the image data in memory, based on the
 	  raw size of the image. */
 	inmem = (rawsize < JAS_IMAGE_INMEMTHRESH);
#endif
 
 	/* Create the individual image components. */
 	for (cmptno = 0, cmptparm = cmptparms; cmptno < numcmpts; ++cmptno,
 	  ++cmptparm) {
		if (!jas_safe_size_mul3(cmptparm->width, cmptparm->height,
		  (cmptparm->prec + 7), &rawsize)) {
			goto error;
		}
		rawsize /= 8;
		inmem = (rawsize < JAS_IMAGE_INMEMTHRESH);
 		if (!(image->cmpts_[cmptno] = jas_image_cmpt_create(cmptparm->tlx,
 		  cmptparm->tly, cmptparm->hstep, cmptparm->vstep,
 		  cmptparm->width, cmptparm->height, cmptparm->prec,
 		  cmptparm->sgnd, inmem))) {
			goto error;
 		}
 		++image->numcmpts_;
 	}

	/* Determine the bounding box for all of the components on the
	  reference grid (i.e., the image area) */
 	jas_image_setbbox(image);
 
 	return image;

error:
	if (image) {
		jas_image_destroy(image);
	}
	return 0;
 }
