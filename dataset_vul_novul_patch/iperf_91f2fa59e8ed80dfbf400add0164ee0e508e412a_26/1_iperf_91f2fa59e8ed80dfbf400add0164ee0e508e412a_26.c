cJSON *cJSON_Parse( const char *value )
 {
	cJSON *c;
	ep = 0;
	if ( ! ( c = cJSON_New_Item() ) )
		return 0;	/* memory fail */
 
	if ( ! parse_value( c, skip( value ) ) ) {
		cJSON_Delete( c );
		return 0;
	}
 	return c;
 }
