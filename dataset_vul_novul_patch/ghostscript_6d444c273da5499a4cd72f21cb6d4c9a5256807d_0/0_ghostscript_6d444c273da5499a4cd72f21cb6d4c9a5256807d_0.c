int gs_lib_ctx_init( gs_memory_t *mem )
{
    gs_lib_ctx_t *pio = 0;

    /* Check the non gc allocator is being passed in */
    if (mem == 0 || mem != mem->non_gc_memory)
        return_error(gs_error_Fatal);

#ifndef GS_THREADSAFE
    mem_err_print = mem;
#endif

    if (mem->gs_lib_ctx) /* one time initialization */
        return 0;

    pio = (gs_lib_ctx_t*)gs_alloc_bytes_immovable(mem,
                                                  sizeof(gs_lib_ctx_t),
                                                  "gs_lib_ctx_init");
    if( pio == 0 )
        return -1;

    /* Wholesale blanking is cheaper than retail, and scales better when new
     * fields are added. */
    memset(pio, 0, sizeof(*pio));
    /* Now set the non zero/false/NULL things */
    pio->memory               = mem;
    gs_lib_ctx_get_real_stdio(&pio->fstdin, &pio->fstdout, &pio->fstderr );
    pio->stdin_is_interactive = true;
    /* id's 1 through 4 are reserved for Device color spaces; see gscspace.h */
    pio->gs_next_id           = 5;  /* this implies that each thread has its own complete state */

    /* Need to set this before calling gs_lib_ctx_set_icc_directory. */
    mem->gs_lib_ctx = pio;
    /* Initialize our default ICCProfilesDir */
    pio->profiledir = NULL;
    pio->profiledir_len = 0;
    gs_lib_ctx_set_icc_directory(mem, DEFAULT_DIR_ICC, strlen(DEFAULT_DIR_ICC));

    if (gs_lib_ctx_set_default_device_list(mem, gs_dev_defaults,
                        strlen(gs_dev_defaults)) < 0) {
        
        gs_free_object(mem, pio, "gs_lib_ctx_init");
        mem->gs_lib_ctx = NULL;
    }

    /* Initialise the underlying CMS. */
    if (gscms_create(mem)) {
Failure:
        gs_free_object(mem, mem->gs_lib_ctx->default_device_list,
                "gs_lib_ctx_fin");

        gs_free_object(mem, pio, "gs_lib_ctx_init");
        mem->gs_lib_ctx = NULL;
        return -1;
    }

    /* Initialise any lock required for the jpx codec */
    if (sjpxd_create(mem)) {
         gscms_destroy(mem);
         goto Failure;
     }
    pio->client_check_file_permission = NULL;
     gp_get_realtime(pio->real_time_0);
 
     /* Set scanconverter to 1 (default) */
    pio->scanconverter = GS_SCANCONVERTER_DEFAULT;

    return 0;
}
