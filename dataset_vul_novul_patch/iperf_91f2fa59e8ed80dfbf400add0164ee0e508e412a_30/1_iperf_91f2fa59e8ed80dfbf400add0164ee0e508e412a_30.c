void cJSON_ReplaceItemInObject( cJSON *object, const char *string, cJSON *newitem )
{
	int i = 0;
	cJSON *c = object->child;
	while ( c && cJSON_strcasecmp( c->string, string ) ) {
		++i;
		c = c->next;
	}
	if ( c ) {
		newitem->string = cJSON_strdup( string );
		cJSON_ReplaceItemInArray( object, i, newitem );
	}
}
