PHP_FUNCTION(imagepsencodefont)
{
	zval *fnt;
        char *enc, **enc_vector;
        int enc_len, *f_ind;
 
       if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rp", &fnt, &enc, &enc_len) == FAILURE) {
                return;
        }
	ZEND_FETCH_RESOURCE(f_ind, int *, &fnt, -1, "Type 1 font", le_ps_font);

	if ((enc_vector = T1_LoadEncoding(enc)) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't load encoding vector from %s", enc);
		RETURN_FALSE;
	}

	T1_DeleteAllSizes(*f_ind);
	if (T1_ReencodeFont(*f_ind, enc_vector)) {
		T1_DeleteEncoding(enc_vector);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't re-encode font");
		RETURN_FALSE;
	}

	zend_list_insert(enc_vector, le_ps_enc TSRMLS_CC);

	RETURN_TRUE;
}
