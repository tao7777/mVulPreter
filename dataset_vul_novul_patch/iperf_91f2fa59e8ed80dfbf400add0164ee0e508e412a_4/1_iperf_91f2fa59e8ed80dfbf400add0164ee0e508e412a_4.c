cJSON *cJSON_CreateBool( int b )
{
	cJSON *item = cJSON_New_Item();
	if ( item )
		item->type = b ? cJSON_True : cJSON_False;
	return item;
}
