 alloc_limit_failure (char *fn_name, size_t size)
 {
    fprintf (stderr, 
              "%s: Maximum allocation size exceeded "
              "(maxsize = %lu; size = %lu).\n",
              fn_name,
             (unsigned long)alloc_limit, 
              (unsigned long)size);
 }
