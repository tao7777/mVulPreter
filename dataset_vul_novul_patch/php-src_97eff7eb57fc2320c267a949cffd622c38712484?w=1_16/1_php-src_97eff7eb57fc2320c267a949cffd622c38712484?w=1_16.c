 static int append_multiple_key_values(smart_str* loc_name, HashTable* hash_arr, char* key_name TSRMLS_DC)
{
	zval**	ele_value    	= NULL;
	int 	i 		= 0;
	int 	isFirstSubtag 	= 0;
	int 	max_value 	= 0;

	/* Variant/ Extlang/Private etc. */
	if( zend_hash_find( hash_arr , key_name , strlen(key_name) + 1 ,(void **)&ele_value ) == SUCCESS ) {
		if( Z_TYPE_PP(ele_value) == IS_STRING ){
			add_prefix( loc_name , key_name);

			smart_str_appendl(loc_name, SEPARATOR , sizeof(SEPARATOR)-1);
			smart_str_appendl(loc_name, Z_STRVAL_PP(ele_value) , Z_STRLEN_PP(ele_value));
			return SUCCESS;
		} else if(Z_TYPE_PP(ele_value) == IS_ARRAY ) {
			HashPosition pos;
			HashTable *arr = HASH_OF(*ele_value);
			zval **data = NULL;

			zend_hash_internal_pointer_reset_ex(arr, &pos);
			while(zend_hash_get_current_data_ex(arr, (void **)&data, &pos) != FAILURE) {
				if(Z_TYPE_PP(data) != IS_STRING) {
					return FAILURE;
				}
				if (isFirstSubtag++ == 0){
					add_prefix(loc_name , key_name);
				}
				smart_str_appendl(loc_name, SEPARATOR , sizeof(SEPARATOR)-1);
				smart_str_appendl(loc_name, Z_STRVAL_PP(data) , Z_STRLEN_PP(data));
				zend_hash_move_forward_ex(arr, &pos);
			}
			return SUCCESS;
		} else {
			return FAILURE;
		}
	} else {
		char cur_key_name[31];
		/* Decide the max_value: the max. no. of elements allowed */
		if( strcmp(key_name , LOC_VARIANT_TAG) ==0 ){
			max_value  = MAX_NO_VARIANT;
		}
		if( strcmp(key_name , LOC_EXTLANG_TAG) ==0 ){
			max_value  = MAX_NO_EXTLANG;
		}
		if( strcmp(key_name , LOC_PRIVATE_TAG) ==0 ){
			max_value  = MAX_NO_PRIVATE;
		}
 
 		/* Multiple variant values as variant0, variant1 ,variant2 */
 		isFirstSubtag = 0;
		for( i=0 ; i< max_value; i++ ){  
			snprintf( cur_key_name , 30, "%s%d", key_name , i);	
 			if( zend_hash_find( hash_arr , cur_key_name , strlen(cur_key_name) + 1,(void **)&ele_value ) == SUCCESS ){
 				if( Z_TYPE_PP(ele_value)!= IS_STRING ){
 					/* variant is not a string */
					return FAILURE;
				}
				/* Add the contents */
				if (isFirstSubtag++ == 0){
					add_prefix(loc_name , cur_key_name);
				}
				smart_str_appendl(loc_name, SEPARATOR , sizeof(SEPARATOR)-1);
				smart_str_appendl(loc_name, Z_STRVAL_PP(ele_value) , Z_STRLEN_PP(ele_value));
			}
		} /* end of for */
	} /* end of else */

	return SUCCESS;
}
