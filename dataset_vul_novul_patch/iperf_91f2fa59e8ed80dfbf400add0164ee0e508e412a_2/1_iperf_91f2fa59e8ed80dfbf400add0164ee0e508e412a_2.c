void cJSON_AddItemToObject( cJSON *object, const char *string, cJSON *item )
{
	if ( ! item )
		return;
	if ( item->string )
		cJSON_free( item->string );
	item->string = cJSON_strdup( string );
	cJSON_AddItemToArray( object, item );
}
