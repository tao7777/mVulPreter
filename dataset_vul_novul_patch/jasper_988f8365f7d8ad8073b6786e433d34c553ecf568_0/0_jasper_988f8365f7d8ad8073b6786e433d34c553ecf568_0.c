 void *jas_realloc(void *ptr, size_t size)
 {
 	void *result;
	JAS_DBGLOG(101, ("jas_realloc(%x, %zu)\n", ptr, size));
 	result = realloc(ptr, size);
 	JAS_DBGLOG(100, ("jas_realloc(%p, %zu) -> %p\n", ptr, size, result));
 	return result;
}
