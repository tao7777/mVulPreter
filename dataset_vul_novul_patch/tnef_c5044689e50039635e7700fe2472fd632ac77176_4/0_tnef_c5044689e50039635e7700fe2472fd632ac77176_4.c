xmalloc (size_t size)
xmalloc (size_t num, size_t size)
 {
    size_t res;
    if (check_mul_overflow(num, size, &res))
        abort();

    void *ptr = malloc (res);
    if (!ptr
         && (size != 0))         /* some libc don't like size == 0 */
     {
         perror ("xmalloc: Memory allocation failure");
        abort();
    }
    return ptr;
}
