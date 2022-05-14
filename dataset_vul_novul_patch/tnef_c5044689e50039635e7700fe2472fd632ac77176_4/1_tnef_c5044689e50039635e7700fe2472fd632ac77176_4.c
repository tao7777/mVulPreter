xmalloc (size_t size)
 {
    void *ptr = malloc (size);
    if (!ptr 
         && (size != 0))         /* some libc don't like size == 0 */
     {
         perror ("xmalloc: Memory allocation failure");
        abort();
    }
    return ptr;
}
