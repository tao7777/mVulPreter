cJSON *cJSON_CreateTrue( void )
{
	cJSON *item = cJSON_New_Item();
	if ( item )
		item->type = cJSON_True;
	return item;
}
