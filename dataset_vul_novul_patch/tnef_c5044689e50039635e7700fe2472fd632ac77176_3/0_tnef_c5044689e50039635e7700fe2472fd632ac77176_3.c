 xcalloc (size_t num, size_t size)
 {
    size_t res;
    if (check_mul_overflow(num, size, &res))
        abort();

    void *ptr;
    ptr = malloc(res);
     if (ptr)
     {
        memset (ptr, '\0', (res));
     }
     return ptr;
 }
