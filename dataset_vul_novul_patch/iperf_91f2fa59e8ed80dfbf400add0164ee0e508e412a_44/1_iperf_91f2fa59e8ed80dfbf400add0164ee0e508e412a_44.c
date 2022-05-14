static char *print_string_ptr( const char *str )
 {
	const char *ptr;
	char *ptr2, *out;
	int len = 0;
	unsigned char token;
 	
	if ( ! str )
		return cJSON_strdup( "" );
	ptr = str;
	while ( ( token = *ptr ) && ++len ) {
		if ( strchr( "\"\\\b\f\n\r\t", token ) )
			++len;
		else if ( token < 32 )
			len += 5;
		++ptr;
 	}
 	
	if ( ! ( out = (char*) cJSON_malloc( len + 3 ) ) )
		return 0;
	ptr2 = out;
	ptr = str;
	*ptr2++ = '\"';
	while ( *ptr ) {
		if ( (unsigned char) *ptr > 31 && *ptr != '\"' && *ptr != '\\' )
			*ptr2++ = *ptr++;
		else {
			*ptr2++ = '\\';
			switch ( token = *ptr++ ) {
				case '\\': *ptr2++ = '\\'; break;
				case '\"': *ptr2++ = '\"'; break;
				case '\b': *ptr2++ = 'b'; break;
				case '\f': *ptr2++ = 'f'; break;
				case '\n': *ptr2++ = 'n'; break;
				case '\r': *ptr2++ = 'r'; break;
				case '\t': *ptr2++ = 't'; break;
				default:
				/* Escape and print. */
				sprintf( ptr2, "u%04x", token );
				ptr2 += 5;
				break;
 			}
 		}
 	}
	*ptr2++ = '\"';
	*ptr2++ = 0;
 	return out;
 }
