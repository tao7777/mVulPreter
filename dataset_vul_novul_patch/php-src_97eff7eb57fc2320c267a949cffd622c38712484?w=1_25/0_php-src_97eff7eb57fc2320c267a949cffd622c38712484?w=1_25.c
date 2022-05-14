 static char* lookup_loc_range(const char* loc_range, HashTable* hash_arr, int canonicalize  TSRMLS_DC)
{
	int	i = 0;
	int	cur_arr_len = 0;
	int result = 0;

	char* lang_tag = NULL;
	zval** ele_value = NULL;
	char** cur_arr = NULL;

	char* cur_loc_range	= NULL;
	char* can_loc_range	= NULL;
	int	saved_pos = 0;

	char* return_value = NULL;

	cur_arr = ecalloc(zend_hash_num_elements(hash_arr)*2, sizeof(char *));
	/* convert the array to lowercase , also replace hyphens with the underscore and store it in cur_arr */
 	for(zend_hash_internal_pointer_reset(hash_arr);
 		zend_hash_has_more_elements(hash_arr) == SUCCESS;
 		zend_hash_move_forward(hash_arr)) {

 		if (zend_hash_get_current_data(hash_arr, (void**)&ele_value) == FAILURE) {
 			/* Should never actually fail since the key is known to exist.*/
 			continue;
		}
		if(Z_TYPE_PP(ele_value)!= IS_STRING) {
 			/* element value is not a string */
 			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "lookup_loc_range: locale array element is not a string", 0 TSRMLS_CC);
 			LOOKUP_CLEAN_RETURN(NULL);
		}
 		cur_arr[cur_arr_len*2] = estrndup(Z_STRVAL_PP(ele_value), Z_STRLEN_PP(ele_value));
 		result = strToMatch(Z_STRVAL_PP(ele_value), cur_arr[cur_arr_len*2]);
 		if(result == 0) {
 			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "lookup_loc_range: unable to canonicalize lang_tag", 0 TSRMLS_CC);
 			LOOKUP_CLEAN_RETURN(NULL);
 		}
 		cur_arr[cur_arr_len*2+1] = Z_STRVAL_PP(ele_value);
		cur_arr_len++ ;
 	} /* end of for */
 
 	/* Canonicalize array elements */
 	if(canonicalize) {
		for(i=0; i<cur_arr_len; i++) {
 			lang_tag = get_icu_value_internal(cur_arr[i*2], LOC_CANONICALIZE_TAG, &result, 0);
 			if(result != 1 || lang_tag == NULL || !lang_tag[0]) {
 				if(lang_tag) {
					efree(lang_tag);
				}
				intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "lookup_loc_range: unable to canonicalize lang_tag" , 0 TSRMLS_CC);
 				LOOKUP_CLEAN_RETURN(NULL);
 			}
 			cur_arr[i*2] = erealloc(cur_arr[i*2], strlen(lang_tag)+1);
			result = strToMatch(lang_tag, cur_arr[i*2]);
 			efree(lang_tag);
 			if(result == 0) {
 				intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "lookup_loc_range: unable to canonicalize lang_tag" , 0 TSRMLS_CC);
				LOOKUP_CLEAN_RETURN(NULL);
			}
		}

	}

	if(canonicalize) {
		/* Canonicalize the loc_range */
		can_loc_range = get_icu_value_internal(loc_range, LOC_CANONICALIZE_TAG, &result , 0);
		if( result != 1 || can_loc_range == NULL || !can_loc_range[0]) {
			/* Error */
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "lookup_loc_range: unable to canonicalize loc_range" , 0 TSRMLS_CC );
			if(can_loc_range) {
				efree(can_loc_range);
			}
			LOOKUP_CLEAN_RETURN(NULL);
 		} else {
 			loc_range = can_loc_range;
 		}
	}
 
 	cur_loc_range = ecalloc(1, strlen(loc_range)+1);
 	/* convert to lower and replace hyphens */
	result = strToMatch(loc_range, cur_loc_range);
 	if(can_loc_range) {
 		efree(can_loc_range);
 	}
	if(result == 0) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "lookup_loc_range: unable to canonicalize lang_tag" , 0 TSRMLS_CC);
		LOOKUP_CLEAN_RETURN(NULL);
	}

 	/* Lookup for the lang_tag match */
 	saved_pos = strlen(cur_loc_range);
 	while(saved_pos > 0) {
		for(i=0; i< cur_arr_len; i++){
			if(cur_arr[i*2] != NULL && strlen(cur_arr[i*2]) == saved_pos && strncmp(cur_loc_range, cur_arr[i*2], saved_pos) == 0) {
 				/* Match found */
 				return_value = estrdup(canonicalize?cur_arr[i*2]:cur_arr[i*2+1]);
 				efree(cur_loc_range);
				LOOKUP_CLEAN_RETURN(return_value);
			}
		}
		saved_pos = getStrrtokenPos(cur_loc_range, saved_pos);
	}

	/* Match not found */
	efree(cur_loc_range);
	LOOKUP_CLEAN_RETURN(NULL);
 }
