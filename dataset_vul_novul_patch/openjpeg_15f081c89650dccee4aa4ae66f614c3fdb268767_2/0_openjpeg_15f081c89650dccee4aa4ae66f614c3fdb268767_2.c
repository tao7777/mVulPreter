static void sycc444_to_rgb(opj_image_t *img)
 {
 	int *d0, *d1, *d2, *r, *g, *b;
 	const int *y, *cb, *cr;
	size_t maxw, maxh, max, i;
 	int offset, upb;
 
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
 
	for(i = 0U; i < max; ++i)
	{
		sycc_to_rgb(offset, upb, *y, *cb, *cr, r, g, b);
		++y; ++cb; ++cr; ++r; ++g; ++b;
	}
 	free(img->comps[0].data); img->comps[0].data = d0;
 	free(img->comps[1].data); img->comps[1].data = d1;
 	free(img->comps[2].data); img->comps[2].data = d2;
	img->color_space = OPJ_CLRSPC_SRGB;
 	return;
 
 fails:
	free(r);
	free(g);
	free(b);
 }/* sycc444_to_rgb() */
