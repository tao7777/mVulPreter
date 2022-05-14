static char *print_object( cJSON *item, int depth, int fmt )
 {
	char **entries = 0, **names = 0;
	char *out = 0, *ptr, *ret, *str;
	int len = 7, i = 0, j;
	cJSON *child = item->child;
	int numentries = 0, fail = 0;
 	/* Count the number of entries. */
	while ( child ) {
		++numentries;
		child = child->next;
	}
	/* Allocate space for the names and the objects. */
	if ( ! ( entries = (char**) cJSON_malloc( numentries * sizeof(char*) ) ) )
		return 0;
	if ( ! ( names = (char**) cJSON_malloc( numentries * sizeof(char*) ) ) ) {
		cJSON_free( entries );
		return 0;
	}
	memset( entries, 0, sizeof(char*) * numentries );
	memset( names, 0, sizeof(char*) * numentries );
	/* Collect all the results into our arrays. */
	child = item->child;
	++depth;
	if ( fmt )
		len += depth;
	while ( child ) {
		names[i] = str = print_string_ptr( child->string );
		entries[i++] = ret = print_value( child, depth, fmt );
		if ( str && ret )
			len += strlen( ret ) + strlen( str ) + 2 + ( fmt ? 2 + depth : 0 );
		else
			fail = 1;
		child = child->next;
	}
	/* Try to allocate the output string. */
	if ( ! fail ) {
		out = (char*) cJSON_malloc( len );
		if ( ! out )
			fail = 1;
	}
	/* Handle failure. */
	if ( fail ) {
		for ( i = 0; i < numentries; ++i ) {
			if ( names[i] )
				cJSON_free( names[i] );
			if ( entries[i] )
				cJSON_free( entries[i] );
 		}
		cJSON_free( names );
		cJSON_free( entries );
		return 0;
 	}
	/* Compose the output. */
	*out = '{';
	ptr = out + 1;
	if ( fmt )
		*ptr++ = '\n';
	*ptr = 0;
	for ( i = 0; i < numentries; ++i ) {
		if ( fmt )
			for ( j = 0; j < depth; ++j )
				*ptr++ = '\t';
		strcpy( ptr, names[i] );
		ptr += strlen( names[i] );
		*ptr++ = ':';
		if ( fmt )
			*ptr++ = '\t';
		strcpy( ptr, entries[i] );
		ptr += strlen( entries[i] );
		if ( i != numentries - 1 )
			*ptr++ = ',';
		if ( fmt )
			*ptr++ = '\n';
		*ptr = 0;
		cJSON_free( names[i] );
		cJSON_free( entries[i] );
 	}
	cJSON_free( names );
	cJSON_free( entries );
	if ( fmt )
		for ( i = 0; i < depth - 1; ++i )
			*ptr++ = '\t';
	*ptr++ = '}';
	*ptr++ = 0;
 	return out;	
 }
