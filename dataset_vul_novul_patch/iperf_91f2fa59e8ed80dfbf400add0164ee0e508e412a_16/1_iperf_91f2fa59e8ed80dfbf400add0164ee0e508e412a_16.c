void cJSON_DeleteItemFromArray( cJSON *array, int which )
{
	cJSON_Delete( cJSON_DetachItemFromArray( array, which ) );
}
