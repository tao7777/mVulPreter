void color_cmyk_to_rgb(opj_image_t *image)
{
	float C, M, Y, K;
	float sC, sM, sY, sK;
	unsigned int w, h, max, i;

 	w = image->comps[0].w;
 	h = image->comps[0].h;
 
	if (
			(image->numcomps < 4)
		 || (image->comps[0].dx != image->comps[1].dx) || (image->comps[0].dx != image->comps[2].dx) || (image->comps[0].dx != image->comps[3].dx)
		 || (image->comps[0].dy != image->comps[1].dy) || (image->comps[0].dy != image->comps[2].dy) || (image->comps[0].dy != image->comps[3].dy)
			) {
		fprintf(stderr,"%s:%d:color_cmyk_to_rgb\n\tCAN NOT CONVERT\n", __FILE__,__LINE__);
		return;
	}
 
 	max = w * h;
 	
	sC = 1.0F / (float)((1 << image->comps[0].prec) - 1);
	sM = 1.0F / (float)((1 << image->comps[1].prec) - 1);
	sY = 1.0F / (float)((1 << image->comps[2].prec) - 1);
	sK = 1.0F / (float)((1 << image->comps[3].prec) - 1);

	for(i = 0; i < max; ++i)
	{
		/* CMYK values from 0 to 1 */
		C = (float)(image->comps[0].data[i]) * sC;
		M = (float)(image->comps[1].data[i]) * sM;
		Y = (float)(image->comps[2].data[i]) * sY;
		K = (float)(image->comps[3].data[i]) * sK;
		
		/* Invert all CMYK values */
		C = 1.0F - C;
		M = 1.0F - M;
		Y = 1.0F - Y;
		K = 1.0F - K;

		/* CMYK -> RGB : RGB results from 0 to 255 */
		image->comps[0].data[i] = (int)(255.0F * C * K); /* R */
		image->comps[1].data[i] = (int)(255.0F * M * K); /* G */
		image->comps[2].data[i] = (int)(255.0F * Y * K); /* B */
	}

	free(image->comps[3].data); image->comps[3].data = NULL;
	image->comps[0].prec = 8;
	image->comps[1].prec = 8;
	image->comps[2].prec = 8;
	image->numcomps -= 1;
	image->color_space = OPJ_CLRSPC_SRGB;
	
	for (i = 3; i < image->numcomps; ++i) {
		memcpy(&(image->comps[i]), &(image->comps[i+1]), sizeof(image->comps[i]));
	}

}/* color_cmyk_to_rgb() */
