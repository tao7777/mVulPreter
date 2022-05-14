alloc_limit_assert (char *fn_name, size_t size)
 {
     if (alloc_limit && size > alloc_limit)
     {
	alloc_limit_failure (fn_name, size);
	exit (-1);
     }
 }
