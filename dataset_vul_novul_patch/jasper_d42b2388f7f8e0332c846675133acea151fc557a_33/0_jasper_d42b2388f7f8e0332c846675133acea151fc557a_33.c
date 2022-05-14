jas_image_t *jp2_decode(jas_stream_t *in, char *optstr)
{
	jp2_box_t *box;
	int found;
	jas_image_t *image;
	jp2_dec_t *dec;
	bool samedtype;
	int dtype;
	unsigned int i;
	jp2_cmap_t *cmapd;
	jp2_pclr_t *pclrd;
	jp2_cdef_t *cdefd;
	unsigned int channo;
	int newcmptno;
	int_fast32_t *lutents;
#if 0
	jp2_cdefchan_t *cdefent;
	int cmptno;
#endif
	jp2_cmapent_t *cmapent;
	jas_icchdr_t icchdr;
	jas_iccprof_t *iccprof;

	dec = 0;
	box = 0;
	image = 0;

	if (!(dec = jp2_dec_create())) {
		goto error;
	}

	/* Get the first box.  This should be a JP box. */
	if (!(box = jp2_box_get(in))) {
		jas_eprintf("error: cannot get box\n");
		goto error;
	}
	if (box->type != JP2_BOX_JP) {
		jas_eprintf("error: expecting signature box\n");
		goto error;
	}
	if (box->data.jp.magic != JP2_JP_MAGIC) {
		jas_eprintf("incorrect magic number\n");
		goto error;
	}
	jp2_box_destroy(box);
	box = 0;

	/* Get the second box.  This should be a FTYP box. */
	if (!(box = jp2_box_get(in))) {
		goto error;
	}
	if (box->type != JP2_BOX_FTYP) {
		jas_eprintf("expecting file type box\n");
		goto error;
	}
	jp2_box_destroy(box);
	box = 0;

	/* Get more boxes... */
	found = 0;
	while ((box = jp2_box_get(in))) {
		if (jas_getdbglevel() >= 1) {
			jas_eprintf("got box type %s\n", box->info->name);
		}
		switch (box->type) {
		case JP2_BOX_JP2C:
			found = 1;
			break;
		case JP2_BOX_IHDR:
			if (!dec->ihdr) {
				dec->ihdr = box;
				box = 0;
			}
			break;
		case JP2_BOX_BPCC:
			if (!dec->bpcc) {
				dec->bpcc = box;
				box = 0;
			}
			break;
		case JP2_BOX_CDEF:
			if (!dec->cdef) {
				dec->cdef = box;
				box = 0;
			}
			break;
		case JP2_BOX_PCLR:
			if (!dec->pclr) {
				dec->pclr = box;
				box = 0;
			}
			break;
		case JP2_BOX_CMAP:
			if (!dec->cmap) {
				dec->cmap = box;
				box = 0;
			}
			break;
		case JP2_BOX_COLR:
			if (!dec->colr) {
				dec->colr = box;
				box = 0;
			}
			break;
		}
		if (box) {
			jp2_box_destroy(box);
			box = 0;
		}
		if (found) {
			break;
		}
	}

	if (!found) {
		jas_eprintf("error: no code stream found\n");
		goto error;
	}

	if (!(dec->image = jpc_decode(in, optstr))) {
		jas_eprintf("error: cannot decode code stream\n");
		goto error;
	}

	/* An IHDR box must be present. */
	if (!dec->ihdr) {
		jas_eprintf("error: missing IHDR box\n");
		goto error;
	}
 
 	/* Does the number of components indicated in the IHDR box match
 	  the value specified in the code stream? */
	if (dec->ihdr->data.ihdr.numcmpts != JAS_CAST(jas_uint,
 	  jas_image_numcmpts(dec->image))) {
 		jas_eprintf("warning: number of components mismatch\n");
 	}

	/* At least one component must be present. */
	if (!jas_image_numcmpts(dec->image)) {
		jas_eprintf("error: no components\n");
		goto error;
	}

 	/* Determine if all components have the same data type. */
 	samedtype = true;
 	dtype = jas_image_cmptdtype(dec->image, 0);
	for (i = 1; i < JAS_CAST(jas_uint, jas_image_numcmpts(dec->image)); ++i) {
 		if (jas_image_cmptdtype(dec->image, i) != dtype) {
 			samedtype = false;
 			break;
		}
	}

	/* Is the component data type indicated in the IHDR box consistent
	  with the data in the code stream? */
	if ((samedtype && dec->ihdr->data.ihdr.bpc != JP2_DTYPETOBPC(dtype)) ||
	  (!samedtype && dec->ihdr->data.ihdr.bpc != JP2_IHDR_BPCNULL)) {
		jas_eprintf("warning: component data type mismatch\n");
	}

	/* Is the compression type supported? */
	if (dec->ihdr->data.ihdr.comptype != JP2_IHDR_COMPTYPE) {
		jas_eprintf("error: unsupported compression type\n");
		goto error;
	}

 	if (dec->bpcc) {
 		/* Is the number of components indicated in the BPCC box
 		  consistent with the code stream data? */
		if (dec->bpcc->data.bpcc.numcmpts != JAS_CAST(jas_uint, jas_image_numcmpts(
 		  dec->image))) {
 			jas_eprintf("warning: number of components mismatch\n");
 		}
 		/* Is the component data type information indicated in the BPCC
 		  box consistent with the code stream data? */
 		if (!samedtype) {
			for (i = 0; i < JAS_CAST(jas_uint, jas_image_numcmpts(dec->image));
 			  ++i) {
 				if (jas_image_cmptdtype(dec->image, i) !=
 				  JP2_BPCTODTYPE(dec->bpcc->data.bpcc.bpcs[i])) {
					jas_eprintf("warning: component data type mismatch\n");
				}
			}
		} else {
			jas_eprintf("warning: superfluous BPCC box\n");
		}
	}

	/* A COLR box must be present. */
	if (!dec->colr) {
		jas_eprintf("error: no COLR box\n");
		goto error;
	}

	switch (dec->colr->data.colr.method) {
	case JP2_COLR_ENUM:
		jas_image_setclrspc(dec->image, jp2_getcs(&dec->colr->data.colr));
		break;
	case JP2_COLR_ICC:
		iccprof = jas_iccprof_createfrombuf(dec->colr->data.colr.iccp,
		  dec->colr->data.colr.iccplen);
		if (!iccprof) {
			jas_eprintf("error: failed to parse ICC profile\n");
			goto error;
		}
		jas_iccprof_gethdr(iccprof, &icchdr);
		jas_eprintf("ICC Profile CS %08x\n", icchdr.colorspc);
		jas_image_setclrspc(dec->image, fromiccpcs(icchdr.colorspc));
		dec->image->cmprof_ = jas_cmprof_createfromiccprof(iccprof);
		assert(dec->image->cmprof_);
		jas_iccprof_destroy(iccprof);
		break;
	}

	/* If a CMAP box is present, a PCLR box must also be present. */
	if (dec->cmap && !dec->pclr) {
		jas_eprintf("warning: missing PCLR box or superfluous CMAP box\n");
		jp2_box_destroy(dec->cmap);
		dec->cmap = 0;
	}

	/* If a CMAP box is not present, a PCLR box must not be present. */
	if (!dec->cmap && dec->pclr) {
		jas_eprintf("warning: missing CMAP box or superfluous PCLR box\n");
		jp2_box_destroy(dec->pclr);
		dec->pclr = 0;
	}

 	/* Determine the number of channels (which is essentially the number
 	  of components after any palette mappings have been applied). */
 	dec->numchans = dec->cmap ? dec->cmap->data.cmap.numchans :
	  JAS_CAST(jas_uint, jas_image_numcmpts(dec->image));
 
 	/* Perform a basic sanity check on the CMAP box if present. */
 	if (dec->cmap) {
 		for (i = 0; i < dec->numchans; ++i) {
 			/* Is the component number reasonable? */
			if (dec->cmap->data.cmap.ents[i].cmptno >= JAS_CAST(jas_uint,
 			  jas_image_numcmpts(dec->image))) {
 				jas_eprintf("error: invalid component number in CMAP box\n");
 				goto error;
			}
			/* Is the LUT index reasonable? */
			if (dec->cmap->data.cmap.ents[i].pcol >=
			  dec->pclr->data.pclr.numchans) {
				jas_eprintf("error: invalid CMAP LUT index\n");
				goto error;
			}
		}
	}

	/* Allocate space for the channel-number to component-number LUT. */
	if (!(dec->chantocmptlut = jas_alloc2(dec->numchans,
	  sizeof(uint_fast16_t)))) {
		jas_eprintf("error: no memory\n");
		goto error;
	}

	if (!dec->cmap) {
		for (i = 0; i < dec->numchans; ++i) {
			dec->chantocmptlut[i] = i;
		}
	} else {
		cmapd = &dec->cmap->data.cmap;
		pclrd = &dec->pclr->data.pclr;
		cdefd = &dec->cdef->data.cdef;
		for (channo = 0; channo < cmapd->numchans; ++channo) {
			cmapent = &cmapd->ents[channo];
			if (cmapent->map == JP2_CMAP_DIRECT) {
				dec->chantocmptlut[channo] = channo;
			} else if (cmapent->map == JP2_CMAP_PALETTE) {
				lutents = jas_alloc2(pclrd->numlutents, sizeof(int_fast32_t));
				for (i = 0; i < pclrd->numlutents; ++i) {
					lutents[i] = pclrd->lutdata[cmapent->pcol + i * pclrd->numchans];
				}
				newcmptno = jas_image_numcmpts(dec->image);
				jas_image_depalettize(dec->image, cmapent->cmptno,
				  pclrd->numlutents, lutents,
				  JP2_BPCTODTYPE(pclrd->bpc[cmapent->pcol]), newcmptno);
				dec->chantocmptlut[channo] = newcmptno;
				jas_free(lutents);
#if 0
				if (dec->cdef) {
					cdefent = jp2_cdef_lookup(cdefd, channo);
					if (!cdefent) {
						abort();
					}
				jas_image_setcmpttype(dec->image, newcmptno, jp2_getct(jas_image_clrspc(dec->image), cdefent->type, cdefent->assoc));
				} else {
				jas_image_setcmpttype(dec->image, newcmptno, jp2_getct(jas_image_clrspc(dec->image), 0, channo + 1));
				}
#endif
			}
		}
	}
 
 	/* Mark all components as being of unknown type. */
 
	for (i = 0; i < JAS_CAST(jas_uint, jas_image_numcmpts(dec->image)); ++i) {
 		jas_image_setcmpttype(dec->image, i, JAS_IMAGE_CT_UNKNOWN);
 	}
 
	/* Determine the type of each component. */
	if (dec->cdef) {
		for (i = 0; i < dec->numchans; ++i) {
			/* Is the channel number reasonable? */
			if (dec->cdef->data.cdef.ents[i].channo >= dec->numchans) {
				jas_eprintf("error: invalid channel number in CDEF box\n");
				goto error;
			}
			jas_image_setcmpttype(dec->image,
			  dec->chantocmptlut[dec->cdef->data.cdef.ents[i].channo],
			  jp2_getct(jas_image_clrspc(dec->image),
			  dec->cdef->data.cdef.ents[i].type,
			  dec->cdef->data.cdef.ents[i].assoc));
		}
	} else {
		for (i = 0; i < dec->numchans; ++i) {
			jas_image_setcmpttype(dec->image, dec->chantocmptlut[i],
			  jp2_getct(jas_image_clrspc(dec->image), 0, i + 1));
		}
	}

	/* Delete any components that are not of interest. */
	for (i = jas_image_numcmpts(dec->image); i > 0; --i) {
		if (jas_image_cmpttype(dec->image, i - 1) == JAS_IMAGE_CT_UNKNOWN) {
			jas_image_delcmpt(dec->image, i - 1);
		}
	}

	/* Ensure that some components survived. */
	if (!jas_image_numcmpts(dec->image)) {
		jas_eprintf("error: no components\n");
		goto error;
	}
#if 0
jas_eprintf("no of components is %d\n", jas_image_numcmpts(dec->image));
#endif

	/* Prevent the image from being destroyed later. */
	image = dec->image;
	dec->image = 0;

	jp2_dec_destroy(dec);

	return image;

error:
	if (box) {
		jp2_box_destroy(box);
	}
	if (dec) {
		jp2_dec_destroy(dec);
	}
	return 0;
}
