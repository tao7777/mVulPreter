static void suffix_object( cJSON *prev, cJSON *item )
{
	prev->next = item;
	item->prev = prev;
}
