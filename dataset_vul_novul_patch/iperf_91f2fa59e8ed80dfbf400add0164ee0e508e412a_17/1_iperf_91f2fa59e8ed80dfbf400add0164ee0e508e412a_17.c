void cJSON_DeleteItemFromObject( cJSON *object, const char *string )
{
	cJSON_Delete( cJSON_DetachItemFromObject( object, string ) );
}
