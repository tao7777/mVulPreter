static char *print_array( cJSON *item, int depth, int fmt )
 {
 	char **entries;
	char *out = 0, *ptr, *ret;
	int len = 5;
	cJSON *child = item->child;
	int numentries = 0, i = 0, fail = 0;
 	
 	/* How many entries in the array? */
	while ( child ) {
		++numentries;
		child = child->next;
	}
	/* Allocate an array to hold the values for each. */
	if ( ! ( entries = (char**) cJSON_malloc( numentries * sizeof(char*) ) ) )
		return 0;
	memset( entries, 0, numentries * sizeof(char*) );
	/* Retrieve all the results. */
	child = item->child;
	while ( child && ! fail ) {
		ret = print_value( child, depth + 1, fmt );
		entries[i++] = ret;
		if ( ret )
			len += strlen( ret ) + 2 + ( fmt ? 1 : 0 );
		else
			fail = 1;
		child = child -> next;
	}
	/* If we didn't fail, try to malloc the output string. */
	if ( ! fail ) {
		out = (char*) cJSON_malloc( len );
		if ( ! out )
			fail = 1;
	}
	/* Handle failure. */
	if ( fail ) {
		for ( i = 0; i < numentries; ++i )
			if ( entries[i] )
				cJSON_free( entries[i] );
		cJSON_free( entries );
		return 0;
 	}
	/* Compose the output array. */
	*out = '[';
	ptr = out + 1;
	*ptr = 0;
	for ( i = 0; i < numentries; ++i ) {
		strcpy( ptr, entries[i] );
		ptr += strlen( entries[i] );
		if ( i != numentries - 1 ) {
			*ptr++ = ',';
			if ( fmt )
				*ptr++ = ' ';
			*ptr = 0;
 		}
		cJSON_free( entries[i] );
 	}
	cJSON_free( entries );
	*ptr++ = ']';
	*ptr++ = 0;
 	return out;	
 }
