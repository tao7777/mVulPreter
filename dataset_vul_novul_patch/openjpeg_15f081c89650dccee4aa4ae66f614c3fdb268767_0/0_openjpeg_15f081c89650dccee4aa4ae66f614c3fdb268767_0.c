 static void sycc420_to_rgb(opj_image_t *img)
 {
 	int *d0, *d1, *d2, *r, *g, *b, *nr, *ng, *nb;
 	const int *y, *cb, *cr, *ny;
	size_t maxw, maxh, max, offx, loopmaxw, offy, loopmaxh;
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
	
	if (r == NULL || g == NULL || b == NULL) goto fails;
	
	/* if img->x0 is odd, then first column shall use Cb/Cr = 0 */
	offx = img->x0 & 1U;
	loopmaxw = maxw - offx;
	/* if img->y0 is odd, then first line shall use Cb/Cr = 0 */
	offy = img->y0 & 1U;
	loopmaxh = maxh - offy;
	
	if (offy > 0U) {
		size_t j;
		
		for(j=0; j < maxw; ++j)
		{
			sycc_to_rgb(offset, upb, *y, 0, 0, r, g, b);
			++y; ++r; ++g; ++b;
		}
	}
 
	for(i=0U; i < (loopmaxh & ~(size_t)1U); i += 2U)
 	{
		size_t j;
		
 		ny = y + maxw;
 		nr = r + maxw; ng = g + maxw; nb = b + maxw;
		
		if (offx > 0U) {
			sycc_to_rgb(offset, upb, *y, 0, 0, r, g, b);
			++y; ++r; ++g; ++b;
			sycc_to_rgb(offset, upb, *ny, *cb, *cr, nr, ng, nb);
			++ny; ++nr; ++ng; ++nb;
		}
 
		for(j=0; j < (loopmaxw & ~(size_t)1U); j += 2U)
 		{
 			sycc_to_rgb(offset, upb, *y, *cb, *cr, r, g, b);
 			++y; ++r; ++g; ++b;
			sycc_to_rgb(offset, upb, *y, *cb, *cr, r, g, b);
			++y; ++r; ++g; ++b;

			sycc_to_rgb(offset, upb, *ny, *cb, *cr, nr, ng, nb);
			++ny; ++nr; ++ng; ++nb;
 			sycc_to_rgb(offset, upb, *ny, *cb, *cr, nr, ng, nb);
 			++ny; ++nr; ++ng; ++nb; ++cb; ++cr;
 		}
		if(j < loopmaxw)
 		{
 			sycc_to_rgb(offset, upb, *y, *cb, *cr, r, g, b);
 			++y; ++r; ++g; ++b;

			sycc_to_rgb(offset, upb, *ny, *cb, *cr, nr, ng, nb);
			++ny; ++nr; ++ng; ++nb; ++cb; ++cr;
 		}
 		y += maxw; r += maxw; g += maxw; b += maxw;
 	}
	if(i < loopmaxh)
 	{
		size_t j;
		
		for(j=0U; j < (maxw & ~(size_t)1U); j += 2U)
 		{
 			sycc_to_rgb(offset, upb, *y, *cb, *cr, r, g, b);
 
			++y; ++r; ++g; ++b;

			sycc_to_rgb(offset, upb, *y, *cb, *cr, r, g, b);

			++y; ++r; ++g; ++b; ++cb; ++cr;
		}
		if(j < maxw)
		{
			sycc_to_rgb(offset, upb, *y, *cb, *cr, r, g, b);
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
 }/* sycc420_to_rgb() */
