static void _php_image_convert(INTERNAL_FUNCTION_PARAMETERS, int image_type )
{
	char *f_org, *f_dest;
	int f_org_len, f_dest_len;
	long height, width, threshold;
	gdImagePtr im_org, im_dest, im_tmp;
	char *fn_org = NULL;
	char *fn_dest = NULL;
	FILE *org, *dest;
	int dest_height = -1;
	int dest_width = -1;
	int org_height, org_width;
	int white, black;
	int color, color_org, median;
	int int_threshold;
 	int x, y;
 	float x_ratio, y_ratio;
     long ignore_warning;
 	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "pplll", &f_org, &f_org_len, &f_dest, &f_dest_len, &height, &width, &threshold) == FAILURE) {
 		return;
 	}

	fn_org  = f_org;
	fn_dest = f_dest;
	dest_height = height;
	dest_width = width;
	int_threshold = threshold;

	/* Check threshold value */
	if (int_threshold < 0 || int_threshold > 8) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid threshold value '%d'", int_threshold);
		RETURN_FALSE;
	}

	/* Check origin file */
	PHP_GD_CHECK_OPEN_BASEDIR(fn_org, "Invalid origin filename");

	/* Check destination file */
	PHP_GD_CHECK_OPEN_BASEDIR(fn_dest, "Invalid destination filename");

	/* Open origin file */
	org = VCWD_FOPEN(fn_org, "rb");
	if (!org) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open '%s' for reading", fn_org);
		RETURN_FALSE;
	}

	/* Open destination file */
	dest = VCWD_FOPEN(fn_dest, "wb");
	if (!dest) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open '%s' for writing", fn_dest);
		RETURN_FALSE;
	}

	switch (image_type) {
		case PHP_GDIMG_TYPE_GIF:
			im_org = gdImageCreateFromGif(org);
			if (im_org == NULL) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open '%s' Not a valid GIF file", fn_dest);
				RETURN_FALSE;
			}
			break;

#ifdef HAVE_GD_JPG
		case PHP_GDIMG_TYPE_JPG:
			ignore_warning = INI_INT("gd.jpeg_ignore_warning");
			im_org = gdImageCreateFromJpegEx(org, ignore_warning);
			if (im_org == NULL) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open '%s' Not a valid JPEG file", fn_dest);
				RETURN_FALSE;
			}
			break;
#endif /* HAVE_GD_JPG */

#ifdef HAVE_GD_PNG
		case PHP_GDIMG_TYPE_PNG:
			im_org = gdImageCreateFromPng(org);
			if (im_org == NULL) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open '%s' Not a valid PNG file", fn_dest);
				RETURN_FALSE;
			}
			break;
#endif /* HAVE_GD_PNG */

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Format not supported");
			RETURN_FALSE;
			break;
	}

	org_width  = gdImageSX (im_org);
	org_height = gdImageSY (im_org);

	x_ratio = (float) org_width / (float) dest_width;
	y_ratio = (float) org_height / (float) dest_height;

	if (x_ratio > 1 && y_ratio > 1) {
		if (y_ratio > x_ratio) {
			x_ratio = y_ratio;
		} else {
			y_ratio = x_ratio;
		}
		dest_width = (int) (org_width / x_ratio);
		dest_height = (int) (org_height / y_ratio);
	} else {
		x_ratio = (float) dest_width / (float) org_width;
		y_ratio = (float) dest_height / (float) org_height;

		if (y_ratio < x_ratio) {
			x_ratio = y_ratio;
		} else {
			y_ratio = x_ratio;
		}
		dest_width = (int) (org_width * x_ratio);
		dest_height = (int) (org_height * y_ratio);
	}

	im_tmp = gdImageCreate (dest_width, dest_height);
	if (im_tmp == NULL ) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to allocate temporary buffer");
		RETURN_FALSE;
	}

	gdImageCopyResized (im_tmp, im_org, 0, 0, 0, 0, dest_width, dest_height, org_width, org_height);

	gdImageDestroy(im_org);

	fclose(org);

	im_dest = gdImageCreate(dest_width, dest_height);
	if (im_dest == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to allocate destination buffer");
		RETURN_FALSE;
	}

	white = gdImageColorAllocate(im_dest, 255, 255, 255);
	if (white == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to allocate the colors for the destination buffer");
		RETURN_FALSE;
	}

	black = gdImageColorAllocate(im_dest, 0, 0, 0);
	if (black == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to allocate the colors for the destination buffer");
		RETURN_FALSE;
	}

	int_threshold = int_threshold * 32;

	for (y = 0; y < dest_height; y++) {
		for (x = 0; x < dest_width; x++) {
			color_org = gdImageGetPixel (im_tmp, x, y);
			median = (im_tmp->red[color_org] + im_tmp->green[color_org] + im_tmp->blue[color_org]) / 3;
			if (median < int_threshold) {
				color = black;
			} else {
				color = white;
			}
			gdImageSetPixel (im_dest, x, y, color);
		}
	}

	gdImageDestroy (im_tmp );

	gdImageWBMP(im_dest, black , dest);

	fflush(dest);
	fclose(dest);

	gdImageDestroy(im_dest);

	RETURN_TRUE;
}
