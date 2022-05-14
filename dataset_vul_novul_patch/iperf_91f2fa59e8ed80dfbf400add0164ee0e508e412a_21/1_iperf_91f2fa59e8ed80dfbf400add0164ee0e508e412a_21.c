int cJSON_GetArraySize( cJSON *array )
{
	cJSON *c = array->child;
	int i = 0;
	while ( c ) {
		++i;
		c = c->next;
	}
	return i;
}
