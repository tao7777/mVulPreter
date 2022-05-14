Strgrow(Str x)
 {
     char *old = x->ptr;
     int newlen;
    newlen = x->area_size * 6 / 5;
    if (newlen == x->area_size)
 	newlen += 2;
     x->ptr = GC_MALLOC_ATOMIC(newlen);
     x->area_size = newlen;
    bcopy((void *)old, (void *)x->ptr, x->length);
    GC_free(old);
}
