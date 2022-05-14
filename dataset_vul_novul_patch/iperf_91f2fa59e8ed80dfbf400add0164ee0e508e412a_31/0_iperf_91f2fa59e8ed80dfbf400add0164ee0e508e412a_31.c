static int cJSON_strcasecmp( const char *s1, const char *s2 )
static int cJSON_strcasecmp(const char *s1,const char *s2)
 {
	if (!s1) return (s1==s2)?0:1;if (!s2) return 1;
	for(; tolower(*s1) == tolower(*s2); ++s1, ++s2)	if(*s1 == 0)	return 0;
 	return tolower(*(const unsigned char *)s1) - tolower(*(const unsigned char *)s2);
 }
