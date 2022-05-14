checked_xmalloc (size_t size)
 {
    alloc_limit_assert ("checked_xmalloc", size);
    return xmalloc (size);
 }
