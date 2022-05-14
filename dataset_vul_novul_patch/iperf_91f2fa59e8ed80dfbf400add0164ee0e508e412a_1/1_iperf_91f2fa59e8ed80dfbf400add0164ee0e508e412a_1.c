void cJSON_AddItemToArray( cJSON *array, cJSON *item )
{
	cJSON *c = array->child;
	if ( ! item )
		return;
	if ( ! c ) {
		array->child = item;
	} else {
		while ( c && c->next )
			c = c->next;
		suffix_object( c, item );
	}
}
