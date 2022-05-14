static char* cJSON_strdup( const char* str )
 {
	size_t len;
	char* copy;
 
	len = strlen( str ) + 1;
	if ( ! ( copy = (char*) cJSON_malloc( len ) ) )
		return 0;
	memcpy( copy, str, len );
	return copy;
 }
