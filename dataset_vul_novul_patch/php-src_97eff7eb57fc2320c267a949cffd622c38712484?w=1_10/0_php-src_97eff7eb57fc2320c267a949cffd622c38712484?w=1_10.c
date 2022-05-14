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
