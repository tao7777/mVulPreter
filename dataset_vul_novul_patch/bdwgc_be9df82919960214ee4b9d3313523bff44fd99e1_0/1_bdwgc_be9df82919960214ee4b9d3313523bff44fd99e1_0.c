GC_INNER void * GC_generic_malloc_ignore_off_page(size_t lb, int k)
{
    void *result;
    size_t lg;
    size_t lb_rounded;
    word n_blocks;
    GC_bool init;
    DCL_LOCK_STATE;

    if (SMALL_OBJ(lb))
         return(GC_generic_malloc((word)lb, k));
     lg = ROUNDED_UP_GRANULES(lb);
     lb_rounded = GRANULES_TO_BYTES(lg);
     n_blocks = OBJ_SZ_TO_BLOCKS(lb_rounded);
     init = GC_obj_kinds[k].ok_init;
     if (EXPECT(GC_have_errors, FALSE))
      GC_print_all_errors();
    GC_INVOKE_FINALIZERS();
    LOCK();
    result = (ptr_t)GC_alloc_large(ADD_SLOP(lb), k, IGNORE_OFF_PAGE);
    if (0 != result) {
        if (GC_debugging_started) {
            BZERO(result, n_blocks * HBLKSIZE);
        } else {
#           ifdef THREADS
              /* Clear any memory that might be used for GC descriptors */
              /* before we release the lock.                          */
                ((word *)result)[0] = 0;
                ((word *)result)[1] = 0;
                ((word *)result)[GRANULES_TO_WORDS(lg)-1] = 0;
                ((word *)result)[GRANULES_TO_WORDS(lg)-2] = 0;
#           endif
        }
    }
    GC_bytes_allocd += lb_rounded;
    if (0 == result) {
        GC_oom_func oom_fn = GC_oom_fn;
        UNLOCK();
        return((*oom_fn)(lb));
    } else {
        UNLOCK();
        if (init && !GC_debugging_started) {
            BZERO(result, n_blocks * HBLKSIZE);
        }
        return(result);
    }
}
