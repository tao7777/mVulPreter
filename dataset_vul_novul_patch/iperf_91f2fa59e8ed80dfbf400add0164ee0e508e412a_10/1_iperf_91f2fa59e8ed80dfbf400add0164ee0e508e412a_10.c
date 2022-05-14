cJSON *cJSON_CreateNull( void )
{
	cJSON *item = cJSON_New_Item();
	if ( item )
		item->type = cJSON_NULL;
	return item;
}
