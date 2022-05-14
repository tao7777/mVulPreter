cJSON *cJSON_CreateStringArray( const char **strings, int count )
{
	int i;
	cJSON *n = 0, *p = 0, *a = cJSON_CreateArray();
	for ( i = 0; a && i < count; ++i ) {
		n = cJSON_CreateString( strings[i] );
		if ( ! i )
			a->child = n;
		else
			suffix_object( p, n );
		p = n;
	}
	return a;
 }
