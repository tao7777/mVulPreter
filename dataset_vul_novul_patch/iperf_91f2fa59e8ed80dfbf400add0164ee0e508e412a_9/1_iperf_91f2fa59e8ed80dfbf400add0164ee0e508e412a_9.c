cJSON *cJSON_CreateIntArray( int64_t *numbers, int count )
{
	int i;
	cJSON *n = 0, *p = 0, *a = cJSON_CreateArray();
	for ( i = 0; a && i < count; ++i ) {
		n = cJSON_CreateInt( numbers[i] );
		if ( ! i )
			a->child = n;
		else
			suffix_object( p, n );
		p = n;
	}
	return a;
}
