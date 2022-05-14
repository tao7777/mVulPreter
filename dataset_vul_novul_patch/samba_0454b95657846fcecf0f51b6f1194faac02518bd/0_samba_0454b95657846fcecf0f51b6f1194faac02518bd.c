 char *ldb_dn_escape_value(TALLOC_CTX *mem_ctx, struct ldb_val value)
 {
        char *dst;
       size_t len;
        if (!value.length)
                return NULL;
 
	/* allocate destination string, it will be at most 3 times the source */
	dst = talloc_array(mem_ctx, char, value.length * 3 + 1);
	if ( ! dst) {
		talloc_free(dst);
                return NULL;
        }
 
       len = ldb_dn_escape_internal(dst, (const char *)value.data, value.length);
 
       dst = talloc_realloc(mem_ctx, dst, char, len + 1);
       if ( ! dst) {
               talloc_free(dst);
               return NULL;
       }
       dst[len] = '\0';
        return dst;
 }
