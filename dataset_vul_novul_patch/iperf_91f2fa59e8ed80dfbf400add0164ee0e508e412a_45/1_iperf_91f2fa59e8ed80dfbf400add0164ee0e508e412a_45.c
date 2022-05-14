static char *print_value( cJSON *item, int depth, int fmt )
{
	char *out = 0;
	if ( ! item )
		return 0;
	switch ( ( item->type ) & 255 ) {
		case cJSON_NULL:   out = cJSON_strdup( "null" ); break;
		case cJSON_False:  out = cJSON_strdup( "false" ); break;
		case cJSON_True:   out = cJSON_strdup( "true" ); break;
		case cJSON_Number: out = print_number( item ); break;
		case cJSON_String: out = print_string( item ); break;
		case cJSON_Array:  out = print_array( item, depth, fmt ); break;
		case cJSON_Object: out = print_object( item, depth, fmt ); break;
 	}
 	return out;
 }
