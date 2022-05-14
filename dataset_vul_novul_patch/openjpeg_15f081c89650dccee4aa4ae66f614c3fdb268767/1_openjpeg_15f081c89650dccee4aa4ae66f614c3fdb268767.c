 void color_sycc_to_rgb(opj_image_t *img)
{
	if(img->numcomps < 3)
	{
		img->color_space = OPJ_CLRSPC_GRAY;
		return;
	}

	if((img->comps[0].dx == 1)
	&& (img->comps[1].dx == 2)
	&& (img->comps[2].dx == 2)
	&& (img->comps[0].dy == 1)
	&& (img->comps[1].dy == 2)
	&& (img->comps[2].dy == 2))/* horizontal and vertical sub-sample */
  {
		sycc420_to_rgb(img);
  }
	else
	if((img->comps[0].dx == 1)
	&& (img->comps[1].dx == 2)
	&& (img->comps[2].dx == 2)
	&& (img->comps[0].dy == 1)
	&& (img->comps[1].dy == 1)
	&& (img->comps[2].dy == 1))/* horizontal sub-sample only */
  {
		sycc422_to_rgb(img);
  }
	else
	if((img->comps[0].dx == 1)
	&& (img->comps[1].dx == 1)
	&& (img->comps[2].dx == 1)
	&& (img->comps[0].dy == 1)
	&& (img->comps[1].dy == 1)
	&& (img->comps[2].dy == 1))/* no sub-sample */
  {
		sycc444_to_rgb(img);
  }
	else
  {
 		fprintf(stderr,"%s:%d:color_sycc_to_rgb\n\tCAN NOT CONVERT\n", __FILE__,__LINE__);
 		return;
   }
	img->color_space = OPJ_CLRSPC_SRGB;
 }/* color_sycc_to_rgb() */
