cJSON *cJSON_DetachItemFromObject( cJSON *object, const char *string )
{
	int i = 0;
	cJSON *c = object->child;
	while ( c && cJSON_strcasecmp( c->string, string ) ) {
		++i;
		c = c->next;
	}
	if ( c )
		return cJSON_DetachItemFromArray( object, i );
	return 0;
}
