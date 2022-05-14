 char *ldb_dn_escape_value(TALLOC_CTX *mem_ctx, struct ldb_val value)
 {
        char *dst;
        if (!value.length)
                return NULL;
 
	/* allocate destination string, it will be at most 3 times the source */
	dst = talloc_array(mem_ctx, char, value.length * 3 + 1);
	if ( ! dst) {
		talloc_free(dst);
                return NULL;
        }
 
       ldb_dn_escape_internal(dst, (const char *)value.data, value.length);
       dst = talloc_realloc(mem_ctx, dst, char, strlen(dst) + 1);
 
        return dst;
 }
