krb5_gss_process_context_token(minor_status, context_handle,
                               token_buffer)
    OM_uint32 *minor_status;
    gss_ctx_id_t context_handle;
    gss_buffer_t token_buffer;
{
    krb5_gss_ctx_id_rec *ctx;
    OM_uint32 majerr;
 
     ctx = (krb5_gss_ctx_id_t) context_handle;
 
    if (ctx->terminated || !ctx->established) {
         *minor_status = KG_CTX_INCOMPLETE;
         return(GSS_S_NO_CONTEXT);
     }
 
    /* We only support context deletion tokens for now, and RFC 4121 does not
     * define a context deletion token. */
    if (ctx->proto) {
        *minor_status = 0;
        return(GSS_S_DEFECTIVE_TOKEN);
    }

     /* "unseal" the token */
 
     if (GSS_ERROR(majerr = kg_unseal(minor_status, context_handle,
                                     token_buffer,
                                     GSS_C_NO_BUFFER, NULL, NULL,
                                      KG_TOK_DEL_CTX)))
         return(majerr);
 
    /* Mark the context as terminated, but do not delete it (as that would
     * leave the caller with a dangling context handle). */
    ctx->terminated = 1;
    return(GSS_S_COMPLETE);
 }
