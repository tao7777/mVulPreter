static cJSON *create_reference( cJSON *item )
{
	cJSON *ref;
	if ( ! ( ref = cJSON_New_Item() ) )
		return 0;
	memcpy( ref, item, sizeof(cJSON) );
	ref->string = 0;
	ref->type |= cJSON_IsReference;
	ref->next = ref->prev = 0;
	return ref;
}
