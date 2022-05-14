 PHP_FUNCTION(locale_lookup)
 {
	char*      	fallback_loc  		= NULL;
	int        	fallback_loc_len	= 0;
	const char*    	loc_range      		= NULL;
	int        	loc_range_len  		= 0;

	zval*		arr				= NULL;
	HashTable*	hash_arr		= NULL;
	zend_bool	boolCanonical	= 0;
	char*	 	result			=NULL;

	intl_error_reset( NULL TSRMLS_CC );

	if(zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "as|bs", &arr, &loc_range, &loc_range_len,
		&boolCanonical,	&fallback_loc, &fallback_loc_len) == FAILURE) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,	"locale_lookup: unable to parse input params", 0 TSRMLS_CC );
		RETURN_FALSE;
	}

	if(loc_range_len == 0) {
		loc_range = intl_locale_get_default(TSRMLS_C);
	}

	hash_arr = HASH_OF(arr);
 
 	if( !hash_arr || zend_hash_num_elements( hash_arr ) == 0 ) {
 		RETURN_EMPTY_STRING();
	} 
 	result = lookup_loc_range(loc_range, hash_arr, boolCanonical TSRMLS_CC);
 	if(result == NULL || result[0] == '\0') {
 		if( fallback_loc ) {
			result = estrndup(fallback_loc, fallback_loc_len);
		} else {
			RETURN_EMPTY_STRING();
		}
	}

	RETVAL_STRINGL(result, strlen(result), 0);
}
