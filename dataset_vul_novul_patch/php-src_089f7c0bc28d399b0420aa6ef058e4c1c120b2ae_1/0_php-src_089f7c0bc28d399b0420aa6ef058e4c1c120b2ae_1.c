 void gdImageJpegCtx (gdImagePtr im, gdIOCtx * outfile, int quality)
{
	_gdImageJpegCtx(im, outfile, quality);
}

/* returns 0 on success, 1 on failure */
static int _gdImageJpegCtx(gdImagePtr im, gdIOCtx *outfile, int quality)
 {
 	struct jpeg_compress_struct cinfo;
 	struct jpeg_error_mgr jerr;
	int i, j, jidx;
	/* volatile so we can gdFree it on return from longjmp */
	volatile JSAMPROW row = 0;
	JSAMPROW rowptr[1];
	jmpbuf_wrapper jmpbufw;
	JDIMENSION nlines;
	char comment[255];

	memset (&cinfo, 0, sizeof (cinfo));
	memset (&jerr, 0, sizeof (jerr));

	cinfo.err = jpeg_std_error (&jerr);
	cinfo.client_data = &jmpbufw;
	if (setjmp (jmpbufw.jmpbuf) != 0) {
		/* we're here courtesy of longjmp */
 		if (row) {
 			gdFree (row);
 		}
		return 1;
 	}
 
 	cinfo.err->error_exit = fatal_jpeg_error;

	jpeg_create_compress (&cinfo);

	cinfo.image_width = im->sx;
	cinfo.image_height = im->sy;
	cinfo.input_components = 3;	/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB;	/* colorspace of input image */
	jpeg_set_defaults (&cinfo);

	cinfo.density_unit = 1;
	cinfo.X_density = im->res_x;
	cinfo.Y_density = im->res_y;

	if (quality >= 0) {
		jpeg_set_quality (&cinfo, quality, TRUE);
	}

	/* If user requests interlace, translate that to progressive JPEG */
	if (gdImageGetInterlaced (im)) {
		jpeg_simple_progression (&cinfo);
	}

	jpeg_gdIOCtx_dest (&cinfo, outfile);

	row = (JSAMPROW) safe_emalloc(cinfo.image_width * cinfo.input_components, sizeof(JSAMPLE), 0);
	memset(row, 0, cinfo.image_width * cinfo.input_components * sizeof(JSAMPLE));
	rowptr[0] = row;

	jpeg_start_compress (&cinfo, TRUE);

	if (quality >= 0) {
		snprintf(comment, sizeof(comment)-1, "CREATOR: gd-jpeg v%s (using IJG JPEG v%d), quality = %d\n", GD_JPEG_VERSION, JPEG_LIB_VERSION, quality);
	} else {
		snprintf(comment, sizeof(comment)-1, "CREATOR: gd-jpeg v%s (using IJG JPEG v%d), default quality\n", GD_JPEG_VERSION, JPEG_LIB_VERSION);
	}
	jpeg_write_marker (&cinfo, JPEG_COM, (unsigned char *) comment, (unsigned int) strlen (comment));
	if (im->trueColor) {

#if BITS_IN_JSAMPLE == 12
		gd_error("gd-jpeg: error: jpeg library was compiled for 12-bit precision. This is mostly useless, because JPEGs on the web are 8-bit and such versions of the jpeg library won't read or write them. GD doesn't support these unusual images. Edit your jmorecfg.h file to specify the correct precision and completely 'make clean' and 'make install' libjpeg again. Sorry");
		goto error;
#endif /* BITS_IN_JSAMPLE == 12 */

		for (i = 0; i < im->sy; i++) {
			for (jidx = 0, j = 0; j < im->sx; j++) {
				int val = im->tpixels[i][j];

				row[jidx++] = gdTrueColorGetRed (val);
				row[jidx++] = gdTrueColorGetGreen (val);
				row[jidx++] = gdTrueColorGetBlue (val);
			}

			nlines = jpeg_write_scanlines (&cinfo, rowptr, 1);
			if (nlines != 1) {
				gd_error_ex(GD_WARNING, "gd_jpeg: warning: jpeg_write_scanlines returns %u -- expected 1", nlines);
			}
		}
	} else {
		for (i = 0; i < im->sy; i++) {
			for (jidx = 0, j = 0; j < im->sx; j++) {
				int idx = im->pixels[i][j];

				/* NB: Although gd RGB values are ints, their max value is
				 * 255 (see the documentation for gdImageColorAllocate())
				 * -- perfect for 8-bit JPEG encoding (which is the norm)
				 */
#if BITS_IN_JSAMPLE == 8
				row[jidx++] = im->red[idx];
				row[jidx++] = im->green[idx];
				row[jidx++] = im->blue[idx];
#elif BITS_IN_JSAMPLE == 12
				row[jidx++] = im->red[idx] << 4;
				row[jidx++] = im->green[idx] << 4;
				row[jidx++] = im->blue[idx] << 4;
#else
#error IJG JPEG library BITS_IN_JSAMPLE value must be 8 or 12
#endif
			}

			nlines = jpeg_write_scanlines (&cinfo, rowptr, 1);
			if (nlines != 1) {
				gd_error_ex(GD_WARNING, "gd_jpeg: warning: jpeg_write_scanlines returns %u -- expected 1", nlines);
			}
		}
	}

 	jpeg_finish_compress (&cinfo);
 	jpeg_destroy_compress (&cinfo);
 	gdFree (row);
	return 0;
 }
