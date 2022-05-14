kg_unseal_iov(OM_uint32 *minor_status,
              gss_ctx_id_t context_handle,
              int *conf_state,
              gss_qop_t *qop_state,
              gss_iov_buffer_desc *iov,
              int iov_count,
              int toktype)
{
    krb5_gss_ctx_id_rec *ctx;
     OM_uint32 code;
 
     ctx = (krb5_gss_ctx_id_rec *)context_handle;
    if (ctx->terminated || !ctx->established) {
         *minor_status = KG_CTX_INCOMPLETE;
         return GSS_S_NO_CONTEXT;
     }

    if (kg_locate_iov(iov, iov_count, GSS_IOV_BUFFER_TYPE_STREAM) != NULL) {
        code = kg_unseal_stream_iov(minor_status, ctx, conf_state, qop_state,
                                    iov, iov_count, toktype);
    } else {
        code = kg_unseal_iov_token(minor_status, ctx, conf_state, qop_state,
                                   iov, iov_count, toktype);
    }

    return code;
}
