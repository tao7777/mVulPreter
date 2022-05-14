PHP_FUNCTION(imageloadfont)
{
	char *file;
	int file_name, hdr_size = sizeof(gdFont) - sizeof(char *);
	int ind, body_size, n = 0, b, i, body_size_check;
        gdFontPtr font;
        php_stream *stream;
 
       if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p", &file, &file_name) == FAILURE) {
                return;
        }
 
	stream = php_stream_open_wrapper(file, "rb", IGNORE_PATH | IGNORE_URL_WIN | REPORT_ERRORS, NULL);
	if (stream == NULL) {
		RETURN_FALSE;
	}

	/* Only supports a architecture-dependent binary dump format
	 * at the moment.
	 * The file format is like this on machines with 32-byte integers:
	 *
	 * byte 0-3:   (int) number of characters in the font
	 * byte 4-7:   (int) value of first character in the font (often 32, space)
	 * byte 8-11:  (int) pixel width of each character
	 * byte 12-15: (int) pixel height of each character
	 * bytes 16-:  (char) array with character data, one byte per pixel
	 *                    in each character, for a total of
	 *                    (nchars*width*height) bytes.
	 */
	font = (gdFontPtr) emalloc(sizeof(gdFont));
	b = 0;
	while (b < hdr_size && (n = php_stream_read(stream, (char*)&font[b], hdr_size - b))) {
		b += n;
	}

	if (!n) {
		efree(font);
		if (php_stream_eof(stream)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "End of file while reading header");
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error while reading header");
		}
		php_stream_close(stream);
		RETURN_FALSE;
	}
	i = php_stream_tell(stream);
	php_stream_seek(stream, 0, SEEK_END);
	body_size_check = php_stream_tell(stream) - hdr_size;
	php_stream_seek(stream, i, SEEK_SET);

	body_size = font->w * font->h * font->nchars;
	if (body_size != body_size_check) {
		font->w = FLIPWORD(font->w);
		font->h = FLIPWORD(font->h);
		font->nchars = FLIPWORD(font->nchars);
		body_size = font->w * font->h * font->nchars;
	}

	if (overflow2(font->nchars, font->h)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error reading font, invalid font header");
		efree(font);
		php_stream_close(stream);
		RETURN_FALSE;
	}
	if (overflow2(font->nchars * font->h, font->w )) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error reading font, invalid font header");
		efree(font);
		php_stream_close(stream);
		RETURN_FALSE;
	}

	if (body_size != body_size_check) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error reading font");
		efree(font);
		php_stream_close(stream);
		RETURN_FALSE;
	}

	font->data = emalloc(body_size);
	b = 0;
	while (b < body_size && (n = php_stream_read(stream, &font->data[b], body_size - b))) {
		b += n;
	}

	if (!n) {
		efree(font->data);
		efree(font);
		if (php_stream_eof(stream)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "End of file while reading body");
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error while reading body");
		}
		php_stream_close(stream);
		RETURN_FALSE;
	}
	php_stream_close(stream);

	/* Adding 5 to the font index so we will never have font indices
	 * that overlap with the old fonts (with indices 1-5).  The first
	 * list index given out is always 1.
	 */
	ind = 5 + zend_list_insert(font, le_gd_font TSRMLS_CC);

	RETURN_LONG(ind);
}
