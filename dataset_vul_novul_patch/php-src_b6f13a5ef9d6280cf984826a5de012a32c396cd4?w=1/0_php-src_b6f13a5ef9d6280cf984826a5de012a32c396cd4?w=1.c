PHP_FUNCTION(imagetruecolortopalette)
{
	zval *IM;
	zend_bool dither;
	long ncolors;
	gdImagePtr im;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rbl", &IM, &dither, &ncolors) == FAILURE)  {
		return;
	}
 
 	ZEND_FETCH_RESOURCE(im, gdImagePtr, &IM, -1, "Image", le_gd);
 
	if (ncolors <= 0 || ncolors > INT_MAX) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number of colors has to be greater than zero and no more than %d", INT_MAX);
 		RETURN_FALSE;
 	}
	gdImageTrueColorToPalette(im, dither, (int)ncolors);
 
 	RETURN_TRUE;
 }
