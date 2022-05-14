void cJSON_AddItemReferenceToArray( cJSON *array, cJSON *item )
{
	cJSON_AddItemToArray( array, create_reference( item ) );
}
