 z2grestore(i_ctx_t *i_ctx_p)
 {
    if (!restore_page_device(igs, gs_gstate_saved(igs)))
         return gs_grestore(igs);
     return push_callout(i_ctx_p, "%grestorepagedevice");
 }
