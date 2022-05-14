static const char *parse_string( cJSON *item, const char *str )
 {
	const char *ptr = str + 1;
	char *ptr2;
	char *out;
	int len = 0;
	unsigned uc, uc2;
	if ( *str != '\"' ) {
		/* Not a string! */
		ep = str;
		return 0;
	}
 	
	/* Skip escaped quotes. */
	while ( *ptr != '\"' && *ptr && ++len )
		if ( *ptr++ == '\\' )
			ptr++;
 	
	if ( ! ( out = (char*) cJSON_malloc( len + 1 ) ) )
		return 0;
 	
	ptr = str + 1;
	ptr2 = out;
	while ( *ptr != '\"' && *ptr ) {
		if ( *ptr != '\\' )
			*ptr2++ = *ptr++;
		else {
 			ptr++;
			switch ( *ptr ) {
				case 'b': *ptr2++ ='\b'; break;
				case 'f': *ptr2++ ='\f'; break;
				case 'n': *ptr2++ ='\n'; break;
				case 'r': *ptr2++ ='\r'; break;
				case 't': *ptr2++ ='\t'; break;
				case 'u':
					/* Transcode utf16 to utf8. */
					/* Get the unicode char. */
					sscanf( ptr + 1,"%4x", &uc );
					ptr += 4;
					/* Check for invalid. */
					if ( ( uc >= 0xDC00 && uc <= 0xDFFF ) || uc == 0 )
						break;
					/* UTF16 surrogate pairs. */
					if ( uc >= 0xD800 && uc <= 0xDBFF ) {
						if ( ptr[1] != '\\' || ptr[2] != 'u' )
							/* Missing second-half of surrogate. */
							break;
						sscanf( ptr + 3, "%4x", &uc2 );
						ptr += 6;
						if ( uc2 < 0xDC00 || uc2 > 0xDFFF )
							/* Invalid second-half of surrogate. */
							break;
						uc = 0x10000 | ( ( uc & 0x3FF ) << 10 ) | ( uc2 & 0x3FF );
 					}
 
					len = 4;
					if ( uc < 0x80 )
						len = 1;
					else if ( uc < 0x800 )
						len = 2;
					else if ( uc < 0x10000 )
						len = 3;
					ptr2 += len;
 					
					switch ( len ) {
						case 4: *--ptr2 = ( ( uc | 0x80) & 0xBF ); uc >>= 6;
						case 3: *--ptr2 = ( ( uc | 0x80) & 0xBF ); uc >>= 6;
						case 2: *--ptr2 = ( ( uc | 0x80) & 0xBF ); uc >>= 6;
						case 1: *--ptr2 = ( uc | firstByteMark[len] );
 					}
					ptr2 += len;
 					break;
				default:  *ptr2++ = *ptr; break;
 			}
			++ptr;
 		}
 	}
	*ptr2 = 0;
	if ( *ptr == '\"' )
		++ptr;
	item->valuestring = out;
	item->type = cJSON_String;
 	return ptr;
 }
