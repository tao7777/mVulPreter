SPL_METHOD(SplFileObject, fputcsv)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char delimiter = intern->u.file.delimiter, enclosure = intern->u.file.enclosure, escape = intern->u.file.escape;
 	char *delim = NULL, *enclo = NULL, *esc = NULL;
 	int d_len = 0, e_len = 0, esc_len = 0, ret;
 	zval *fields = NULL;

 	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|sss", &fields, &delim, &d_len, &enclo, &e_len, &esc, &esc_len) == SUCCESS) {
 		switch(ZEND_NUM_ARGS())
 		{
		case 4:
			if (esc_len != 1) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "escape must be a character");
				RETURN_FALSE;
			}
			escape = esc[0];
			/* no break */
		case 3:
			if (e_len != 1) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "enclosure must be a character");
				RETURN_FALSE;
			}
			enclosure = enclo[0];
			/* no break */
		case 2:
			if (d_len != 1) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "delimiter must be a character");
				RETURN_FALSE;
			}
			delimiter = delim[0];
			/* no break */
		case 1:
		case 0:
			break;
		}
		ret = php_fputcsv(intern->u.file.stream, fields, delimiter, enclosure, escape TSRMLS_CC);
		RETURN_LONG(ret);
	}
}
