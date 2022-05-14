checked_xmalloc (size_t size)
checked_xmalloc (size_t num, size_t size)
 {
    size_t res;
    if (check_mul_overflow(num, size, &res))
        abort();

    alloc_limit_assert ("checked_xmalloc", res);
    return xmalloc (num, size);
 }
