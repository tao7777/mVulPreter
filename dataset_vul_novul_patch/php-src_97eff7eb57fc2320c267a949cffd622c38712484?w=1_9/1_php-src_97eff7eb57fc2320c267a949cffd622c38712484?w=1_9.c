PHP_FUNCTION(locale_get_all_variants)
{
	const char*  	loc_name        = NULL;
	int    		loc_name_len    = 0;

	int	result		= 0;
	char*	token		= NULL;
	char*	variant		= NULL;
 	char*	saved_ptr	= NULL;
 
 	intl_error_reset( NULL TSRMLS_CC );
 	if(zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "s",
 	&loc_name, &loc_name_len ) == FAILURE)
 	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
	     "locale_parse: unable to parse input params", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	if(loc_name_len == 0) {
		loc_name = intl_locale_get_default(TSRMLS_C);
	}


 	array_init( return_value );
 
 	/* If the locale is grandfathered, stop, no variants */
	if( findOffset( LOC_GRANDFATHERED , loc_name ) >=  0 ){ 
 		/* ("Grandfathered Tag. No variants."); */
 	}
	else {	
 	/* Call ICU variant */
 		variant = get_icu_value_internal( loc_name , LOC_VARIANT_TAG , &result ,0);
 		if( result > 0 && variant){
 			/* Tokenize on the "_" or "-" */
			token = php_strtok_r( variant , DELIMITER , &saved_ptr);	
 			add_next_index_stringl( return_value, token , strlen(token) ,TRUE );
 			/* tokenize on the "_" or "-" and stop  at singleton if any	*/
 			while( (token = php_strtok_r(NULL , DELIMITER, &saved_ptr)) && (strlen(token)>1) ){
 				add_next_index_stringl( return_value, token , strlen(token) ,TRUE );
			}
		}
		if( variant ){
 			efree( variant );
 		}
 	}
 
 }
