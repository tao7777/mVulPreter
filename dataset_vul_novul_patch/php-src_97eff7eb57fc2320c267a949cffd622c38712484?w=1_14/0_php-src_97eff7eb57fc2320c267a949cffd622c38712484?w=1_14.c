static int add_array_entry(const char* loc_name, zval* hash_arr, char* key_name TSRMLS_DC)
{
	char*   key_value 	= NULL;
	char*   cur_key_name	= NULL;
	char*   token        	= NULL;
	char*   last_ptr  	= NULL;

	int	result		= 0;
	int 	cur_result  	= 0;
	int 	cnt  		= 0;


	if( strcmp(key_name , LOC_PRIVATE_TAG)==0 ){
		key_value = get_private_subtags( loc_name );
		result = 1;
 	} else {
 		key_value = get_icu_value_internal( loc_name , key_name , &result,1 );
 	}
	if( (strcmp(key_name , LOC_PRIVATE_TAG)==0) ||
 		( strcmp(key_name , LOC_VARIANT_TAG)==0) ){
 		if( result > 0 && key_value){
 			/* Tokenize on the "_" or "-"  */
			token = php_strtok_r( key_value , DELIMITER ,&last_ptr);
 			if( cur_key_name ){
 				efree( cur_key_name);
 			}
 			cur_key_name = (char*)ecalloc( 25,  25);
			sprintf( cur_key_name , "%s%d", key_name , cnt++);
 			add_assoc_string( hash_arr, cur_key_name , token ,TRUE );
 			/* tokenize on the "_" or "-" and stop  at singleton if any */
 			while( (token = php_strtok_r(NULL , DELIMITER , &last_ptr)) && (strlen(token)>1) ){
				sprintf( cur_key_name , "%s%d", key_name , cnt++);
 				add_assoc_string( hash_arr, cur_key_name , token , TRUE );
 			}
 /*
			if( strcmp(key_name, LOC_PRIVATE_TAG) == 0 ){
			}
*/
		}
	} else {
		if( result == 1 ){
			add_assoc_string( hash_arr, key_name , key_value , TRUE );
			cur_result = 1;
		}
	}

	if( cur_key_name ){
		efree( cur_key_name);
 	}
 	/*if( key_name != LOC_PRIVATE_TAG && key_value){*/
 	if( key_value){
		efree(key_value);
 	}
 	return cur_result;
 }
 /* }}} */
 
/* {{{ proto static array Locale::parseLocale($locale)
 * parses a locale-id into an array the different parts of it
  }}} */
/* {{{ proto static array parse_locale($locale)
 * parses a locale-id into an array the different parts of it
 */
 PHP_FUNCTION(locale_parse)
{
    const char* loc_name        = NULL;
    int         loc_name_len    = 0;
    int         grOffset    	= 0;

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

	grOffset =  findOffset( LOC_GRANDFATHERED , loc_name );
	if( grOffset >= 0 ){
		add_assoc_string( return_value , LOC_GRANDFATHERED_LANG_TAG , estrdup(loc_name) ,FALSE );
	}
	else{
		/* Not grandfathered */
		add_array_entry( loc_name , return_value , LOC_LANG_TAG TSRMLS_CC);
		add_array_entry( loc_name , return_value , LOC_SCRIPT_TAG TSRMLS_CC);
		add_array_entry( loc_name , return_value , LOC_REGION_TAG TSRMLS_CC);
		add_array_entry( loc_name , return_value , LOC_VARIANT_TAG TSRMLS_CC);
		add_array_entry( loc_name , return_value , LOC_PRIVATE_TAG TSRMLS_CC);
	}
}
/* }}} */

/* {{{ proto static array Locale::getAllVariants($locale)
* gets an array containing the list of variants, or null
 }}} */
/* {{{ proto static array locale_get_all_variants($locale)
* gets an array containing the list of variants, or null
*/
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
 /* }}} */

/*{{{
* Converts to lower case and also replaces all hyphens with the underscore
*/
static int strToMatch(const char* str ,char *retstr)
{
	char* 	anchor 	= NULL;
	const char* 	anchor1 = NULL;
	int 	result 	= 0;

    if( (!str) || str[0] == '\0'){
        return result;
    } else {
	anchor = retstr;
	anchor1 = str;
        while( (*str)!='\0' ){
		if( *str == '-' ){
			*retstr =  '_';
		} else {
			*retstr = tolower(*str);
		}
            str++;
            retstr++;
	}
	*retstr = '\0';
	retstr=  anchor;
	str=  anchor1;
	result = 1;
    }

    return(result);
}
 /* }}} */
 
 /* {{{ proto static boolean Locale::filterMatches(string $langtag, string $locale[, bool $canonicalize])
* Checks if a $langtag filter matches with $locale according to RFC 4647's basic filtering algorithm
 */
 /* }}} */
 /* {{{ proto boolean locale_filter_matches(string $langtag, string $locale[, bool $canonicalize])
* Checks if a $langtag filter matches with $locale according to RFC 4647's basic filtering algorithm
 */
 PHP_FUNCTION(locale_filter_matches)
 {
	char*       	lang_tag        = NULL;
	int         	lang_tag_len    = 0;
	const char*     loc_range       = NULL;
	int         	loc_range_len   = 0;

	int		result		= 0;
	char*		token		= 0;
	char*		chrcheck	= NULL;

	char*       	can_lang_tag    = NULL;
	char*       	can_loc_range   = NULL;

 	char*       	cur_lang_tag    = NULL;
 	char*       	cur_loc_range   = NULL;
 
	zend_bool 	boolCanonical 	= 0;
 	UErrorCode	status		= U_ZERO_ERROR;
 
 	intl_error_reset( NULL TSRMLS_CC );

 	if(zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "ss|b",
		&lang_tag, &lang_tag_len , &loc_range , &loc_range_len ,
 		&boolCanonical) == FAILURE)
 	{
 		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
		"locale_filter_matches: unable to parse input params", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	if(loc_range_len == 0) {
		loc_range = intl_locale_get_default(TSRMLS_C);
	}

	if( strcmp(loc_range,"*")==0){
		RETURN_TRUE;
	}

	if( boolCanonical ){
 		/* canonicalize loc_range */
 		can_loc_range=get_icu_value_internal( loc_range , LOC_CANONICALIZE_TAG , &result , 0);
 		if( result ==0) {
			intl_error_set( NULL, status,
 				"locale_filter_matches : unable to canonicalize loc_range" , 0 TSRMLS_CC );
 			RETURN_FALSE;
 		}
 
 		/* canonicalize lang_tag */
 		can_lang_tag = get_icu_value_internal( lang_tag , LOC_CANONICALIZE_TAG , &result ,  0);
 		if( result ==0) {
			intl_error_set( NULL, status,
 				"locale_filter_matches : unable to canonicalize lang_tag" , 0 TSRMLS_CC );
 			RETURN_FALSE;
 		}

		/* Convert to lower case for case-insensitive comparison */
		cur_lang_tag = ecalloc( 1, strlen(can_lang_tag) + 1);

		/* Convert to lower case for case-insensitive comparison */
		result = strToMatch( can_lang_tag , cur_lang_tag);
		if( result == 0) {
			efree( cur_lang_tag );
			efree( can_lang_tag );
			RETURN_FALSE;
		}

		cur_loc_range = ecalloc( 1, strlen(can_loc_range) + 1);
		result = strToMatch( can_loc_range , cur_loc_range );
		if( result == 0) {
			efree( cur_lang_tag );
			efree( can_lang_tag );
			efree( cur_loc_range );
			efree( can_loc_range );
			RETURN_FALSE;
		}
 
 		/* check if prefix */
 		token 	= strstr( cur_lang_tag , cur_loc_range );

 		if( token && (token==cur_lang_tag) ){
 			/* check if the char. after match is SEPARATOR */
 			chrcheck = token + (strlen(cur_loc_range));
			if( isIDSeparator(*chrcheck) || isEndOfTag(*chrcheck) ){
 				if( cur_lang_tag){
 					efree( cur_lang_tag );
 				}
				if( cur_loc_range){
					efree( cur_loc_range );
				}
				if( can_lang_tag){
					efree( can_lang_tag );
				}
				if( can_loc_range){
					efree( can_loc_range );
				}
				RETURN_TRUE;
			}
		}

		/* No prefix as loc_range */
		if( cur_lang_tag){
			efree( cur_lang_tag );
		}
		if( cur_loc_range){
			efree( cur_loc_range );
		}
		if( can_lang_tag){
			efree( can_lang_tag );
		}
		if( can_loc_range){
			efree( can_loc_range );
		}
		RETURN_FALSE;

	} /* end of if isCanonical */
 	else{
 		/* Convert to lower case for case-insensitive comparison */
 		cur_lang_tag = ecalloc( 1, strlen(lang_tag ) + 1);

 		result = strToMatch( lang_tag , cur_lang_tag);
 		if( result == 0) {
 			efree( cur_lang_tag );
			RETURN_FALSE;
		}
		cur_loc_range = ecalloc( 1, strlen(loc_range ) + 1);
		result = strToMatch( loc_range , cur_loc_range );
		if( result == 0) {
			efree( cur_lang_tag );
			efree( cur_loc_range );
			RETURN_FALSE;
		}
 
 		/* check if prefix */
 		token 	= strstr( cur_lang_tag , cur_loc_range );

 		if( token && (token==cur_lang_tag) ){
 			/* check if the char. after match is SEPARATOR */
 			chrcheck = token + (strlen(cur_loc_range));
			if( isIDSeparator(*chrcheck) || isEndOfTag(*chrcheck) ){
 				if( cur_lang_tag){
 					efree( cur_lang_tag );
 				}
				if( cur_loc_range){
					efree( cur_loc_range );
				}
				RETURN_TRUE;
			}
		}

		/* No prefix as loc_range */
		if( cur_lang_tag){
			efree( cur_lang_tag );
		}
		if( cur_loc_range){
			efree( cur_loc_range );
		}
		RETURN_FALSE;

	}
}
/* }}} */

 static void array_cleanup( char* arr[] , int arr_size)
 {
 	int i=0;
