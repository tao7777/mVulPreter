gs_main_init1(gs_main_instance * minst)
{
    if (minst->init_done < 1) {
        gs_dual_memory_t idmem;
        int code =
            ialloc_init(&idmem, minst->heap,
                        minst->memory_clump_size, gs_have_level2());

        if (code < 0)
            return code;
        code = gs_lib_init1((gs_memory_t *)idmem.space_system);
        if (code < 0)
            return code;
        alloc_save_init(&idmem);
        {
            gs_memory_t *mem = (gs_memory_t *)idmem.space_system;
            name_table *nt = names_init(minst->name_table_size,
                                        idmem.space_system);

            if (nt == 0)
                return_error(gs_error_VMerror);
            mem->gs_lib_ctx->gs_name_table = nt;
            code = gs_register_struct_root(mem, NULL,
                                           (void **)&mem->gs_lib_ctx->gs_name_table,
                                           "the_gs_name_table");
                                            "the_gs_name_table");
             if (code < 0)
                 return code;
            mem->gs_lib_ctx->client_check_file_permission = z_check_file_permissions;
         }
         code = obj_init(&minst->i_ctx_p, &idmem);  /* requires name_init */
         if (code < 0)
        if (code < 0)
            return code;
        code = i_iodev_init(minst->i_ctx_p);
        if (code < 0)
            return code;
        minst->init_done = 1;
    }
    return 0;
}
