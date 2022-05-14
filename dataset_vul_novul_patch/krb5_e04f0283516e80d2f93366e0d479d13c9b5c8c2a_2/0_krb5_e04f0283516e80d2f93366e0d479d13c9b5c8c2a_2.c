iakerb_gss_delete_sec_context(OM_uint32 *minor_status,
                               gss_ctx_id_t *context_handle,
                               gss_buffer_t output_token)
 {
    iakerb_ctx_id_t iakerb_ctx = (iakerb_ctx_id_t)*context_handle;
 
     if (output_token != GSS_C_NO_BUFFER) {
         output_token->length = 0;
         output_token->value = NULL;
     }
 
     *minor_status = 0;
    *context_handle = GSS_C_NO_CONTEXT;
    iakerb_release_context(iakerb_ctx);
 
    return GSS_S_COMPLETE;
 }
