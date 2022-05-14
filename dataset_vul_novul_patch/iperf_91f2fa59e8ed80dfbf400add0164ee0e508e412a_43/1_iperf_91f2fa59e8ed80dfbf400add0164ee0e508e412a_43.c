static char *print_string( cJSON *item )
{
	return print_string_ptr( item->valuestring );
}
