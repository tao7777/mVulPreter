GC_API void * GC_CALL GC_generic_malloc(size_t lb, int k)
{
    void * result;
    DCL_LOCK_STATE;

    if (EXPECT(GC_have_errors, FALSE))
      GC_print_all_errors();
    GC_INVOKE_FINALIZERS();
    if (SMALL_OBJ(lb)) {
        LOCK();
        result = GC_generic_malloc_inner((word)lb, k);
        UNLOCK();
    } else {
        size_t lg;
        size_t lb_rounded;
        word n_blocks;
         GC_bool init;
         lg = ROUNDED_UP_GRANULES(lb);
         lb_rounded = GRANULES_TO_BYTES(lg);
        if (lb_rounded < lb)
            return((*GC_get_oom_fn())(lb));
         n_blocks = OBJ_SZ_TO_BLOCKS(lb_rounded);
         init = GC_obj_kinds[k].ok_init;
         LOCK();
        result = (ptr_t)GC_alloc_large(lb_rounded, k, 0);
        if (0 != result) {
          if (GC_debugging_started) {
            BZERO(result, n_blocks * HBLKSIZE);
          } else {
#           ifdef THREADS
              /* Clear any memory that might be used for GC descriptors */
              /* before we release the lock.                            */
                ((word *)result)[0] = 0;
                ((word *)result)[1] = 0;
                ((word *)result)[GRANULES_TO_WORDS(lg)-1] = 0;
                ((word *)result)[GRANULES_TO_WORDS(lg)-2] = 0;
#           endif
          }
        }
        GC_bytes_allocd += lb_rounded;
        UNLOCK();
        if (init && !GC_debugging_started && 0 != result) {
            BZERO(result, n_blocks * HBLKSIZE);
        }
    }
    if (0 == result) {
        return((*GC_get_oom_fn())(lb));
    } else {
        return(result);
    }
}
