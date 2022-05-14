iakerb_gss_export_sec_context(OM_uint32 *minor_status,
                              gss_ctx_id_t *context_handle,
                               gss_buffer_t interprocess_token)
 {
     OM_uint32 maj;
    iakerb_ctx_id_t ctx = (iakerb_ctx_id_t)*context_handle;
 
     /* We don't currently support exporting partially established contexts. */
     if (!ctx->established)
        return GSS_S_UNAVAILABLE;

    maj = krb5_gss_export_sec_context(minor_status, &ctx->gssc,
                                      interprocess_token);
    if (ctx->gssc == GSS_C_NO_CONTEXT) {
        iakerb_release_context(ctx);
        *context_handle = GSS_C_NO_CONTEXT;
    }
     return maj;
 }
