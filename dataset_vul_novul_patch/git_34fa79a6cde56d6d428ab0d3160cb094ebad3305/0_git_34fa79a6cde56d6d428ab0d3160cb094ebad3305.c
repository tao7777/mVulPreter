 static char *pool_strdup(const char *s)
 {
	size_t len = strlen(s) + 1;
	char *r = pool_alloc(len);
	memcpy(r, s, len);
 	return r;
 }
