 static char *pool_strdup(const char *s)
 {
	char *r = pool_alloc(strlen(s) + 1);
	strcpy(r, s);
 	return r;
 }
