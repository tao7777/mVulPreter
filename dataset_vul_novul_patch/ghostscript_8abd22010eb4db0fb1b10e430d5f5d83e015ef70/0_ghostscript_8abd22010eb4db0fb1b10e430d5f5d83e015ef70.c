lib_file_open(gs_file_path_ptr  lib_path, const gs_memory_t *mem, i_ctx_t *i_ctx_p,
                       const char *fname, uint flen, char *buffer, int blen, uint *pclen, ref *pfile)
{   /* i_ctx_p is NULL running arg (@) files.
     * lib_path and mem are never NULL
     */
    bool starting_arg_file = (i_ctx_p == NULL) ? true : i_ctx_p->starting_arg_file;
    bool search_with_no_combine = false;
    bool search_with_combine = false;
    char fmode[2] = { 'r', 0};
    gx_io_device *iodev = iodev_default(mem);
     gs_main_instance *minst = get_minst_from_memory(mem);
     int code;
 
    if (i_ctx_p && starting_arg_file)
        i_ctx_p->starting_arg_file = false;

     /* when starting arg files (@ files) iodev_default is not yet set */
     if (iodev == 0)
         iodev = (gx_io_device *)gx_io_device_table[0];
       search_with_combine = false;
    } else {
       search_with_no_combine = starting_arg_file;
       search_with_combine = true;
    }
