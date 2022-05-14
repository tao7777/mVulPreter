cJSON *cJSON_GetArrayItem( cJSON *array, int item )
{
	cJSON *c = array->child;
	while ( c && item > 0 ) {
		--item;
		c = c->next;
	}
	return c;
}
