jas_image_t *jpg_decode(jas_stream_t *in, char *optstr)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE *input_file;
	jpg_dest_t dest_mgr_buf;
	jpg_dest_t *dest_mgr = &dest_mgr_buf;
	JDIMENSION num_scanlines;
 	jas_image_t *image;
 	int ret;
 	jpg_dec_importopts_t opts;
	size_t num_samples;

	JAS_DBGLOG(100, ("jpg_decode(%p, \"%s\")\n", in, optstr));
 
 	if (jpg_dec_parseopts(optstr, &opts)) {
 		goto error;
	}

	memset(&cinfo, 0, sizeof(struct jpeg_decompress_struct));
	memset(dest_mgr, 0, sizeof(jpg_dest_t));

	dest_mgr->data = 0;

	image = 0;
	input_file = 0;
	if (!(input_file = tmpfile())) {
		jas_eprintf("cannot make temporary file\n");
		goto error;
	}
	if (jpg_copystreamtofile(input_file, in)) {
		jas_eprintf("cannot copy stream\n");
		goto error;
	}
	rewind(input_file);

	/* Allocate and initialize a JPEG decompression object. */
	JAS_DBGLOG(10, ("jpeg_std_error(%p)\n", &jerr));
	cinfo.err = jpeg_std_error(&jerr);
	JAS_DBGLOG(10, ("jpeg_create_decompress(%p)\n", &cinfo));
	jpeg_create_decompress(&cinfo);

	/* Specify the data source for decompression. */
	JAS_DBGLOG(10, ("jpeg_stdio_src(%p, %p)\n", &cinfo, input_file));
	jpeg_stdio_src(&cinfo, input_file);

	/* Read the file header to obtain the image information. */
	JAS_DBGLOG(10, ("jpeg_read_header(%p, TRUE)\n", &cinfo));
	ret = jpeg_read_header(&cinfo, TRUE);
	JAS_DBGLOG(10, ("jpeg_read_header return value %d\n", ret));
	if (ret != JPEG_HEADER_OK) {
		jas_eprintf("jpeg_read_header did not return JPEG_HEADER_OK\n");
	}
	JAS_DBGLOG(10, (
	  "header: image_width %d; image_height %d; num_components %d\n",
 	  cinfo.image_width, cinfo.image_height, cinfo.num_components)
 	  );
 
	if (opts.max_samples > 0) {
		if (!jas_safe_size_mul3(cinfo.image_width, cinfo.image_height,
		  cinfo.num_components, &num_samples)) {
			goto error;
		}
		if (num_samples > opts.max_samples) {
			jas_eprintf("image is too large (%zu > %zu)\n", num_samples,
			  opts.max_samples);
			goto error;
		}
	}

 	/* Start the decompressor. */
 	JAS_DBGLOG(10, ("jpeg_start_decompress(%p)\n", &cinfo));
 	ret = jpeg_start_decompress(&cinfo);
	JAS_DBGLOG(10, ("jpeg_start_decompress return value %d\n", ret));
	JAS_DBGLOG(10, (
	  "header: output_width %d; output_height %d; output_components %d\n",
 	  cinfo.output_width, cinfo.output_height, cinfo.output_components)
 	  );
 
 	/* Create an image object to hold the decoded data. */
 	if (!(image = jpg_mkimage(&cinfo))) {
 		jas_eprintf("jpg_mkimage failed\n");
		goto error;
	}

	/* Initialize the data sink object. */
	dest_mgr->image = image;
	if (!(dest_mgr->data = jas_matrix_create(1, cinfo.output_width))) {
		jas_eprintf("jas_matrix_create failed\n");
		goto error;
	}
	dest_mgr->start_output = jpg_start_output;
	dest_mgr->put_pixel_rows = jpg_put_pixel_rows;
	dest_mgr->finish_output = jpg_finish_output;
    dest_mgr->buffer = (*cinfo.mem->alloc_sarray)
      ((j_common_ptr) &cinfo, JPOOL_IMAGE,
       cinfo.output_width * cinfo.output_components, (JDIMENSION) 1);
	dest_mgr->buffer_height = 1;
	dest_mgr->error = 0;

	/* Process the compressed data. */
	(*dest_mgr->start_output)(&cinfo, dest_mgr);
	while (cinfo.output_scanline < cinfo.output_height) {
		JAS_DBGLOG(10, ("jpeg_read_scanlines(%p, %p, %lu)\n", &cinfo,
		  dest_mgr->buffer, JAS_CAST(unsigned long, dest_mgr->buffer_height)));
		num_scanlines = jpeg_read_scanlines(&cinfo, dest_mgr->buffer,
		  dest_mgr->buffer_height);
		JAS_DBGLOG(10, ("jpeg_read_scanlines return value %lu\n",
		  JAS_CAST(unsigned long, num_scanlines)));
		(*dest_mgr->put_pixel_rows)(&cinfo, dest_mgr, num_scanlines);
	}
	(*dest_mgr->finish_output)(&cinfo, dest_mgr);

	/* Complete the decompression process. */
	JAS_DBGLOG(10, ("jpeg_finish_decompress(%p)\n", &cinfo));
	jpeg_finish_decompress(&cinfo);

	/* Destroy the JPEG decompression object. */
	JAS_DBGLOG(10, ("jpeg_destroy_decompress(%p)\n", &cinfo));
	jpeg_destroy_decompress(&cinfo);

	jas_matrix_destroy(dest_mgr->data);

	JAS_DBGLOG(10, ("fclose(%p)\n", input_file));
	fclose(input_file);
	input_file = 0;

	if (dest_mgr->error) {
		jas_eprintf("error during decoding\n");
		goto error;
	}

	return image;

error:
	if (dest_mgr->data) {
		jas_matrix_destroy(dest_mgr->data);
	}
	if (image) {
		jas_image_destroy(image);
	}
	if (input_file) {
		fclose(input_file);
	}
	return 0;
}
