static char *print_number( cJSON *item )
 {
 	char *str;
	double f, f2;
	int64_t i;
 
	str = (char*) cJSON_malloc( 64 );
	if ( str ) {
		f = item->valuefloat;
		i = f;
		f2 = i;
		if ( f2 == f && item->valueint >= LLONG_MIN && item->valueint <= LLONG_MAX )
			sprintf( str, "%lld", (long long) item->valueint );
		else
			sprintf( str, "%g", item->valuefloat );
 	}
 	return str;
 }
