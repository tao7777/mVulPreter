cJSON *cJSON_CreateArray( void )
{
	cJSON *item = cJSON_New_Item();
	if ( item )
		item->type = cJSON_Array;
	return item;
}
