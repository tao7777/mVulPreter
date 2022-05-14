static const char *parse_object( cJSON *item, const char *value )
 {
 	cJSON *child;
	if ( *value != '{' ) {
		/* Not an object! */
		ep = value;
		return 0;
	}
 	
	item->type = cJSON_Object;
	value =skip( value + 1 );
	if ( *value == '}' )
		return value + 1;	/* empty array. */
 	
	if ( ! ( item->child = child = cJSON_New_Item() ) )
		return 0;
	if ( ! ( value = skip( parse_string( child, skip( value ) ) ) ) )
		return 0;
	child->string = child->valuestring;
	child->valuestring = 0;
	if ( *value != ':' ) {
		/* Fail! */
		ep = value;
		return 0;
	}
	if ( ! ( value = skip( parse_value( child, skip( value + 1 ) ) ) ) )
		return 0;
 	
	while ( *value == ',' ) {
 		cJSON *new_item;
		if ( ! ( new_item = cJSON_New_Item() ) )
			return 0;	/* memory fail */
		child->next = new_item;
		new_item->prev = child;
		child = new_item;
		if ( ! ( value = skip( parse_string( child, skip( value + 1 ) ) ) ) )
			return 0;
		child->string = child->valuestring;
		child->valuestring = 0;
		if ( *value != ':' ) {
			/* Fail! */
			ep = value;
			return 0;
		}
		if ( ! ( value = skip( parse_value( child, skip( value + 1 ) ) ) ) )
			return 0;
 	}
 	
	if ( *value == '}' )
