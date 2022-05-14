opj_image_t* bmptoimage(const char *filename, opj_cparameters_t *parameters)
{
	opj_image_cmptparm_t cmptparm[4];	/* maximum of 4 components */
	OPJ_UINT8 lut_R[256], lut_G[256], lut_B[256];
	OPJ_UINT8 const* pLUT[3];
	opj_image_t * image = NULL;
	FILE *IN;
	OPJ_BITMAPFILEHEADER File_h;
	OPJ_BITMAPINFOHEADER Info_h;
	OPJ_UINT32 i, palette_len, numcmpts = 1U;
	OPJ_BOOL l_result = OPJ_FALSE;
	OPJ_UINT8* pData = NULL;
	OPJ_UINT32 stride;
	
	pLUT[0] = lut_R; pLUT[1] = lut_G; pLUT[2] = lut_B;
	
	IN = fopen(filename, "rb");
	if (!IN)
	{
		fprintf(stderr, "Failed to open %s for reading !!\n", filename);
		return NULL;
	}

	if (!bmp_read_file_header(IN, &File_h)) {
		fclose(IN);
		return NULL;
	}
	if (!bmp_read_info_header(IN, &Info_h)) {
		fclose(IN);
		return NULL;
	}
	
	/* Load palette */
	if (Info_h.biBitCount <= 8U)
	{
		memset(&lut_R[0], 0, sizeof(lut_R));
		memset(&lut_G[0], 0, sizeof(lut_G));
		memset(&lut_B[0], 0, sizeof(lut_B));
		
		palette_len = Info_h.biClrUsed;
		if((palette_len == 0U) && (Info_h.biBitCount <= 8U)) {
			palette_len = (1U << Info_h.biBitCount);
		}
		if (palette_len > 256U) {
			palette_len = 256U;
		}
		if (palette_len > 0U) {
			OPJ_UINT8 has_color = 0U;
			for (i = 0U; i < palette_len; i++) {
				lut_B[i] = (OPJ_UINT8)getc(IN);
				lut_G[i] = (OPJ_UINT8)getc(IN);
				lut_R[i] = (OPJ_UINT8)getc(IN);
				(void)getc(IN); /* padding */
				has_color |= (lut_B[i] ^ lut_G[i]) | (lut_G[i] ^ lut_R[i]);
			}
			if(has_color) {
				numcmpts = 3U;
			}
		}
	} else {
		numcmpts = 3U;
		if ((Info_h.biCompression == 3) && (Info_h.biAlphaMask != 0U)) {
			numcmpts++;
 		}
 	}
 	
 	stride = ((Info_h.biWidth * Info_h.biBitCount + 31U) / 32U) * 4U; /* rows are aligned on 32bits */
 	if (Info_h.biBitCount == 4 && Info_h.biCompression == 2) { /* RLE 4 gets decoded as 8 bits data for now... */
 		stride = ((Info_h.biWidth * 8U + 31U) / 32U) * 4U;
 	}
 	pData = (OPJ_UINT8 *) calloc(1, stride * Info_h.biHeight * sizeof(OPJ_UINT8));
 	if (pData == NULL) {
 		fclose(IN);
		return NULL;
	}
	/* Place the cursor at the beginning of the image information */
	fseek(IN, 0, SEEK_SET);
	fseek(IN, (long)File_h.bfOffBits, SEEK_SET);
	
	switch (Info_h.biCompression) {
		case 0:
		case 3:
			/* read raw data */
			l_result = bmp_read_raw_data(IN, pData, stride, Info_h.biWidth, Info_h.biHeight);
			break;
		case 1:
			/* read rle8 data */
			l_result = bmp_read_rle8_data(IN, pData, stride, Info_h.biWidth, Info_h.biHeight);
			break;
		case 2:
			/* read rle4 data */
			l_result = bmp_read_rle4_data(IN, pData, stride, Info_h.biWidth, Info_h.biHeight);
			break;
  default:
			fprintf(stderr, "Unsupported BMP compression\n");
			l_result = OPJ_FALSE;
			break;
	}
	if (!l_result) {
		free(pData);
		fclose(IN);
		return NULL;
	}
	
	/* create the image */
	memset(&cmptparm[0], 0, sizeof(cmptparm));
	for(i = 0; i < 4U; i++)
	{
		cmptparm[i].prec = 8;
		cmptparm[i].bpp  = 8;
		cmptparm[i].sgnd = 0;
		cmptparm[i].dx   = (OPJ_UINT32)parameters->subsampling_dx;
		cmptparm[i].dy   = (OPJ_UINT32)parameters->subsampling_dy;
		cmptparm[i].w    = Info_h.biWidth;
		cmptparm[i].h    = Info_h.biHeight;
	}

	image = opj_image_create(numcmpts, &cmptparm[0], (numcmpts == 1U) ? OPJ_CLRSPC_GRAY : OPJ_CLRSPC_SRGB);
	if(!image) {
		fclose(IN);
		free(pData);
		return NULL;
	}
	if (numcmpts == 4U) {
		image->comps[3].alpha = 1;
	}
	
	/* set image offset and reference grid */
	image->x0 = (OPJ_UINT32)parameters->image_offset_x0;
	image->y0 = (OPJ_UINT32)parameters->image_offset_y0;
	image->x1 =	image->x0 + (Info_h.biWidth  - 1U) * (OPJ_UINT32)parameters->subsampling_dx + 1U;
	image->y1 = image->y0 + (Info_h.biHeight - 1U) * (OPJ_UINT32)parameters->subsampling_dy + 1U;
	
	/* Read the data */
	if (Info_h.biBitCount == 24 && Info_h.biCompression == 0) { /*RGB */
		bmp24toimage(pData, stride, image);
	}
	else if (Info_h.biBitCount == 8 && Info_h.biCompression == 0) { /* RGB 8bpp Indexed */
		bmp8toimage(pData, stride, image, pLUT);
	}
	else if (Info_h.biBitCount == 8 && Info_h.biCompression == 1) { /*RLE8*/
		bmp8toimage(pData, stride, image, pLUT);
	}
	else if (Info_h.biBitCount == 4 && Info_h.biCompression == 2) { /*RLE4*/
		bmp8toimage(pData, stride, image, pLUT); /* RLE 4 gets decoded as 8 bits data for now */
	}
	else if (Info_h.biBitCount == 32 && Info_h.biCompression == 0) { /* RGBX */
		bmpmask32toimage(pData, stride, image, 0x00FF0000U, 0x0000FF00U, 0x000000FFU, 0x00000000U);
	}
	else if (Info_h.biBitCount == 32 && Info_h.biCompression == 3) { /* bitmask */
		bmpmask32toimage(pData, stride, image, Info_h.biRedMask, Info_h.biGreenMask, Info_h.biBlueMask, Info_h.biAlphaMask);
	}
	else if (Info_h.biBitCount == 16 && Info_h.biCompression == 0) { /* RGBX */
		bmpmask16toimage(pData, stride, image, 0x7C00U, 0x03E0U, 0x001FU, 0x0000U);
	}
	else if (Info_h.biBitCount == 16 && Info_h.biCompression == 3) { /* bitmask */
		if ((Info_h.biRedMask == 0U) && (Info_h.biGreenMask == 0U) && (Info_h.biBlueMask == 0U)) {
			Info_h.biRedMask   = 0xF800U;
			Info_h.biGreenMask = 0x07E0U;
			Info_h.biBlueMask  = 0x001FU;
		}
		bmpmask16toimage(pData, stride, image, Info_h.biRedMask, Info_h.biGreenMask, Info_h.biBlueMask, Info_h.biAlphaMask);
	}
	else {
		opj_image_destroy(image);
		image = NULL;
		fprintf(stderr, "Other system than 24 bits/pixels or 8 bits (no RLE coding) is not yet implemented [%d]\n", Info_h.biBitCount);
	}
	free(pData);
	fclose(IN);
	return image;
}
