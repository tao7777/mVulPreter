static void get_icu_value_src_php( char* tag_name, INTERNAL_FUNCTION_PARAMETERS) 
 {
 
 	const char* loc_name        	= NULL;
	int         loc_name_len    	= 0;

	char*       tag_value		= NULL;
	char*       empty_result	= "";

	int         result    		= 0;
	char*       msg        		= NULL;

	UErrorCode  status          	= U_ZERO_ERROR;

	intl_error_reset( NULL TSRMLS_CC );

	if(zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "s",
	&loc_name ,&loc_name_len ) == FAILURE) {
		spprintf(&msg , 0, "locale_get_%s : unable to parse input params", tag_name );
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,  msg , 1 TSRMLS_CC );
		efree(msg);

		RETURN_FALSE;
    }

	if(loc_name_len == 0) {
		loc_name = intl_locale_get_default(TSRMLS_C);
	}

	/* Call ICU get */
	tag_value = get_icu_value_internal( loc_name , tag_name , &result ,0);

	/* No value found */
	if( result == -1 ) {
		if( tag_value){
			efree( tag_value);
		}
		RETURN_STRING( empty_result , TRUE);
	}

	/* value found */
	if( tag_value){
		RETURN_STRING( tag_value , FALSE);
	}

	/* Error encountered while fetching the value */
	if( result ==0) {
		spprintf(&msg , 0, "locale_get_%s : unable to get locale %s", tag_name , tag_name );
		intl_error_set( NULL, status, msg , 1 TSRMLS_CC );
		efree(msg);
		RETURN_NULL();
	}

 }
