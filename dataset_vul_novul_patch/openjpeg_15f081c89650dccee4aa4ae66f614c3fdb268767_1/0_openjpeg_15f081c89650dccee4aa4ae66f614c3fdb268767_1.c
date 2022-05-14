 static void sycc422_to_rgb(opj_image_t *img)
 {	
 	int *d0, *d1, *d2, *r, *g, *b;
 	const int *y, *cb, *cr;
	size_t maxw, maxh, max, offx, loopmaxw;
 	int offset, upb;
	size_t i;
 
 	upb = (int)img->comps[0].prec;
 	offset = 1<<(upb - 1); upb = (1<<upb)-1;
 
	maxw = (size_t)img->comps[0].w; maxh = (size_t)img->comps[0].h;
 	max = maxw * maxh;
 
 	y = img->comps[0].data;
 	cb = img->comps[1].data;
 	cr = img->comps[2].data;
 
	d0 = r = (int*)malloc(sizeof(int) * max);
	d1 = g = (int*)malloc(sizeof(int) * max);
	d2 = b = (int*)malloc(sizeof(int) * max);
 
 	if(r == NULL || g == NULL || b == NULL) goto fails;
 
	/* if img->x0 is odd, then first column shall use Cb/Cr = 0 */
	offx = img->x0 & 1U;
	loopmaxw = maxw - offx;
	
 	for(i=0U; i < maxh; ++i)
 	{
		size_t j;
		
		if (offx > 0U) {
			sycc_to_rgb(offset, upb, *y, 0, 0, r, g, b);
			++y; ++r; ++g; ++b;
		}
		
		for(j=0U; j < (loopmaxw & ~(size_t)1U); j += 2U)
 		{
 			sycc_to_rgb(offset, upb, *y, *cb, *cr, r, g, b);
 			++y; ++r; ++g; ++b;
 			sycc_to_rgb(offset, upb, *y, *cb, *cr, r, g, b);
 			++y; ++r; ++g; ++b; ++cb; ++cr;
 		}
		if (j < loopmaxw) {
 			sycc_to_rgb(offset, upb, *y, *cb, *cr, r, g, b);
 			++y; ++r; ++g; ++b; ++cb; ++cr;
 		}
 	}
	
 	free(img->comps[0].data); img->comps[0].data = d0;
 	free(img->comps[1].data); img->comps[1].data = d1;
 	free(img->comps[2].data); img->comps[2].data = d2;
 
	img->comps[1].w = img->comps[2].w = img->comps[0].w;
	img->comps[1].h = img->comps[2].h = img->comps[0].h;
	img->comps[1].dx = img->comps[2].dx = img->comps[0].dx;
	img->comps[1].dy = img->comps[2].dy = img->comps[0].dy;
	img->color_space = OPJ_CLRSPC_SRGB;
 	return;
 
 fails:
	free(r);
	free(g);
	free(b);
 }/* sycc422_to_rgb() */
