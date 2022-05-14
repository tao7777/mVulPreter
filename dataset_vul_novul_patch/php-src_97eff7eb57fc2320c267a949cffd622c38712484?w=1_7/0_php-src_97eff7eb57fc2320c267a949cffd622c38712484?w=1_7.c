 PHP_FUNCTION( locale_get_keywords )
 {
     UEnumeration*   e        = NULL;
    UErrorCode      status   = U_ZERO_ERROR;

    const char*	 	kw_key        = NULL;
    int32_t         kw_key_len    = 0;

     const char*       	loc_name        = NULL;
     int        	 	loc_name_len    = 0;
 
/*
	ICU expects the buffer to be allocated  before calling the function
	and so the buffer size has been explicitly specified
	ICU uloc.h #define 	ULOC_KEYWORD_AND_VALUES_CAPACITY   100
 	hence the kw_value buffer size is 100
 */
 	char*	 	kw_value        = NULL;
    int32_t     kw_value_len    = 100;

    intl_error_reset( NULL TSRMLS_CC );

    if(zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "s",
        &loc_name, &loc_name_len ) == FAILURE)
    {
        intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
             "locale_get_keywords: unable to parse input params", 0 TSRMLS_CC );

        RETURN_FALSE;
    }

    if(loc_name_len == 0) {
        loc_name = intl_locale_get_default(TSRMLS_C);
    }

	/* Get the keywords */
    e = uloc_openKeywords( loc_name, &status );
    if( e != NULL )
    {
		/* Traverse it, filling the return array. */
    	array_init( return_value );

    	while( ( kw_key = uenum_next( e, &kw_key_len, &status ) ) != NULL ){
			kw_value = ecalloc( 1 , kw_value_len  );

			/* Get the keyword value for each keyword */
			kw_value_len=uloc_getKeywordValue( loc_name,kw_key, kw_value, kw_value_len ,  &status );
			if (status == U_BUFFER_OVERFLOW_ERROR) {
				status = U_ZERO_ERROR;
				kw_value = erealloc( kw_value , kw_value_len+1);
 				kw_value_len=uloc_getKeywordValue( loc_name,kw_key, kw_value, kw_value_len+1 ,  &status );
 			} else if(!U_FAILURE(status)) {
 				kw_value = erealloc( kw_value , kw_value_len+1);
			}
 			if (U_FAILURE(status)) {
 	        		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "locale_get_keywords: Error encountered while getting the keyword  value for the  keyword", 0 TSRMLS_CC );
 				if( kw_value){
					efree( kw_value );
				}
				zval_dtor(return_value);
        		RETURN_FALSE;
			}

       		add_assoc_stringl( return_value, (char *)kw_key, kw_value , kw_value_len, 0);
		} /* end of while */

	} /* end of if e!=NULL */

    uenum_close( e );
 }
