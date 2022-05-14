 static int append_key_value(smart_str* loc_name, HashTable* hash_arr, char* key_name)
{
	zval**	ele_value	= NULL;

	if(zend_hash_find(hash_arr , key_name , strlen(key_name) + 1 ,(void **)&ele_value ) == SUCCESS ) {
		if(Z_TYPE_PP(ele_value)!= IS_STRING ){
 			/* element value is not a string */
 			return FAILURE;
 		}
		if(strcmp(key_name, LOC_LANG_TAG) != 0 && 
 		   strcmp(key_name, LOC_GRANDFATHERED_LANG_TAG)!=0 ) {
 			/* not lang or grandfathered tag */
 			smart_str_appendl(loc_name, SEPARATOR , sizeof(SEPARATOR)-1);
		}
		smart_str_appendl(loc_name, Z_STRVAL_PP(ele_value) , Z_STRLEN_PP(ele_value));
		return SUCCESS;
	}

	return LOC_NOT_FOUND;
}
