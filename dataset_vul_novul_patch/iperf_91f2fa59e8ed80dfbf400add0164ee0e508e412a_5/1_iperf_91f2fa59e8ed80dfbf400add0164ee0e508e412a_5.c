cJSON *cJSON_CreateFalse( void )
{
	cJSON *item = cJSON_New_Item();
	if ( item )
		item->type = cJSON_False;
	return item;
}
