 void *jas_malloc(size_t size)
 {
 	void *result;
	JAS_DBGLOG(101, ("jas_malloc called with %zu\n", size));
 	result = malloc(size);
 	JAS_DBGLOG(100, ("jas_malloc(%zu) -> %p\n", size, result));
 	return result;
}
