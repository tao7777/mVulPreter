static void get_icu_disp_value_src_php( char* tag_name, INTERNAL_FUNCTION_PARAMETERS) 
static void get_icu_disp_value_src_php( char* tag_name, INTERNAL_FUNCTION_PARAMETERS)
 {
 	const char* loc_name        	= NULL;
 	int         loc_name_len    	= 0;

	const char* disp_loc_name       = NULL;
	int         disp_loc_name_len   = 0;
	int         free_loc_name       = 0;

	UChar*      disp_name      	= NULL;
	int32_t     disp_name_len  	= 0;

	char*       mod_loc_name        = NULL;

	int32_t     buflen          	= 512;
	UErrorCode  status          	= U_ZERO_ERROR;

	char*       utf8value		= NULL;
	int         utf8value_len   	= 0;

  	char*       msg             	= NULL;
	int         grOffset    	= 0;

 	intl_error_reset( NULL TSRMLS_CC );
 
 	if(zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "s|s",
		&loc_name, &loc_name_len ,
 		&disp_loc_name ,&disp_loc_name_len ) == FAILURE)
 	{
 		spprintf(&msg , 0, "locale_get_display_%s : unable to parse input params", tag_name );
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,  msg , 1 TSRMLS_CC );
		efree(msg);
		RETURN_FALSE;
	}

    if(loc_name_len > ULOC_FULLNAME_CAPACITY) {
        /* See bug 67397: overlong locale names cause trouble in uloc_getDisplayName */
		spprintf(&msg , 0, "locale_get_display_%s : name too long", tag_name );
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,  msg , 1 TSRMLS_CC );
		efree(msg);
		RETURN_FALSE;
    }

	if(loc_name_len == 0) {
		loc_name = intl_locale_get_default(TSRMLS_C);
	}

	if( strcmp(tag_name, DISP_NAME) != 0 ){
		/* Handle grandfathered languages */
		grOffset = findOffset( LOC_GRANDFATHERED , loc_name );
		if( grOffset >= 0 ){
			if( strcmp(tag_name , LOC_LANG_TAG)==0 ){
				mod_loc_name = getPreferredTag( loc_name );
			} else {
				/* Since Grandfathered, no value, do nothing, retutn NULL */
				RETURN_FALSE;
			}
		}
	} /* end of if != LOC_CANONICAL_TAG */

 	if( mod_loc_name==NULL ){
 		mod_loc_name = estrdup( loc_name );
 	}

 	/* Check if disp_loc_name passed , if not use default locale */
 	if( !disp_loc_name){
 		disp_loc_name = estrdup(intl_locale_get_default(TSRMLS_C));
		free_loc_name = 1;
	}

    /* Get the disp_value for the given locale */
    do{
        disp_name = erealloc( disp_name , buflen * sizeof(UChar)  );
        disp_name_len = buflen;

		if( strcmp(tag_name , LOC_LANG_TAG)==0 ){
			buflen = uloc_getDisplayLanguage ( mod_loc_name , disp_loc_name , disp_name , disp_name_len , &status);
		} else if( strcmp(tag_name , LOC_SCRIPT_TAG)==0 ){
			buflen = uloc_getDisplayScript ( mod_loc_name , disp_loc_name , disp_name , disp_name_len , &status);
		} else if( strcmp(tag_name , LOC_REGION_TAG)==0 ){
			buflen = uloc_getDisplayCountry ( mod_loc_name , disp_loc_name , disp_name , disp_name_len , &status);
		} else if( strcmp(tag_name , LOC_VARIANT_TAG)==0 ){
			buflen = uloc_getDisplayVariant ( mod_loc_name , disp_loc_name , disp_name , disp_name_len , &status);
		} else if( strcmp(tag_name , DISP_NAME)==0 ){
			buflen = uloc_getDisplayName ( mod_loc_name , disp_loc_name , disp_name , disp_name_len , &status);
		}

		/* U_STRING_NOT_TERMINATED_WARNING is admissible here; don't look for it */
		if( U_FAILURE( status ) )
		{
			if( status == U_BUFFER_OVERFLOW_ERROR )
			{
				status = U_ZERO_ERROR;
				continue;
			}

			spprintf(&msg, 0, "locale_get_display_%s : unable to get locale %s", tag_name , tag_name );
			intl_error_set( NULL, status, msg , 1 TSRMLS_CC );
			efree(msg);
			if( disp_name){
				efree( disp_name );
			}
			if( mod_loc_name){
				efree( mod_loc_name );
			}
			if (free_loc_name) {
				efree((void *)disp_loc_name);
				disp_loc_name = NULL;
			}
			RETURN_FALSE;
		}
	} while( buflen > disp_name_len );

	if( mod_loc_name){
		efree( mod_loc_name );
	}
	if (free_loc_name) {
		efree((void *)disp_loc_name);
		disp_loc_name = NULL;
	}
	/* Convert display locale name from UTF-16 to UTF-8. */
	intl_convert_utf16_to_utf8( &utf8value, &utf8value_len, disp_name, buflen, &status );
	efree( disp_name );
	if( U_FAILURE( status ) )
	{
		spprintf(&msg, 0, "locale_get_display_%s :error converting display name for %s to UTF-8", tag_name , tag_name );
		intl_error_set( NULL, status, msg , 1 TSRMLS_CC );
		efree(msg);
		RETURN_FALSE;
	}

	RETVAL_STRINGL( utf8value, utf8value_len , FALSE);

}
