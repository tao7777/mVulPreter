 static char* getPreferredTag(const char* gf_tag)
{ 
 	char* result = NULL;
 	int grOffset = 0;
 
	grOffset = findOffset( LOC_GRANDFATHERED ,gf_tag);
	if(grOffset < 0) {
		return NULL;
	}
	if( grOffset < LOC_PREFERRED_GRANDFATHERED_LEN ){
		/* return preferred tag */
		result = estrdup( LOC_PREFERRED_GRANDFATHERED[grOffset] );
	} else {
		/* Return correct grandfathered language tag */
		result = estrdup( LOC_GRANDFATHERED[grOffset] );
	}
	return result;
 }
