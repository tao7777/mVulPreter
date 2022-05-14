 z2grestore(i_ctx_t *i_ctx_p)
 {
    int code = restore_page_device(i_ctx_p, igs, gs_gstate_saved(igs));
    if (code < 0) return code;

    if (code == 0)
         return gs_grestore(igs);
     return push_callout(i_ctx_p, "%grestorepagedevice");
 }
