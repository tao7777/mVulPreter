 void * calloc(size_t n, size_t lb)
 {
    if ((lb | n) > GC_SQRT_SIZE_MAX /* fast initial test */
        && lb && n > GC_SIZE_MAX / lb)
       return NULL;
 #   if defined(GC_LINUX_THREADS) /* && !defined(USE_PROC_FOR_LIBRARIES) */
         /* libpthread allocated some memory that is only pointed to by  */
        /* mmapped thread stacks.  Make sure it's not collectable.      */
        {
          static GC_bool lib_bounds_set = FALSE;
          ptr_t caller = (ptr_t)__builtin_return_address(0);
          /* This test does not need to ensure memory visibility, since */
          /* the bounds will be set when/if we create another thread.   */
          if (!EXPECT(lib_bounds_set, TRUE)) {
            GC_init_lib_bounds();
            lib_bounds_set = TRUE;
          }
          if (((word)caller >= (word)GC_libpthread_start
               && (word)caller < (word)GC_libpthread_end)
              || ((word)caller >= (word)GC_libld_start
                  && (word)caller < (word)GC_libld_end))
            return GC_malloc_uncollectable(n*lb);
          /* The two ranges are actually usually adjacent, so there may */
          /* be a way to speed this up.                                 */
        }
#   endif
    return((void *)REDIRECT_MALLOC(n*lb));
}
