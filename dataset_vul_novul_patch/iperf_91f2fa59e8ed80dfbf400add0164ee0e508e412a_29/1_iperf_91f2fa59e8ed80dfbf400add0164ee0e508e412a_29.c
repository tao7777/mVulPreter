void cJSON_ReplaceItemInArray( cJSON *array, int which, cJSON *newitem )
{
	cJSON *c = array->child;
	while ( c && which > 0 ) {
		c = c->next;
		--which;
	}
	if ( ! c )
		return;
	newitem->next = c->next;
	newitem->prev = c->prev;
	if ( newitem->next )
		newitem->next->prev = newitem;
	if ( c == array->child )
		array->child = newitem;
	else
		newitem->prev->next = newitem;
	c->next = c->prev = 0;
	cJSON_Delete( c );
}
