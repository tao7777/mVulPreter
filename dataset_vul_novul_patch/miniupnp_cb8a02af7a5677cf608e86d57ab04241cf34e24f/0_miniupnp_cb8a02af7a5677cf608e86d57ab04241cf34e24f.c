 static void copyIPv6IfDifferent(void * dest, const void * src)
 {
	if(dest != src && src != NULL) {
 		memcpy(dest, src, sizeof(struct in6_addr));
 	}
 }
