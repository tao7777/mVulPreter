Strgrow(Str x)
 {
     char *old = x->ptr;
     int newlen;
    newlen = x->length * 6 / 5;
    if (newlen == x->length)
 	newlen += 2;
     x->ptr = GC_MALLOC_ATOMIC(newlen);
     x->area_size = newlen;
    bcopy((void *)old, (void *)x->ptr, x->length);
    GC_free(old);
}
