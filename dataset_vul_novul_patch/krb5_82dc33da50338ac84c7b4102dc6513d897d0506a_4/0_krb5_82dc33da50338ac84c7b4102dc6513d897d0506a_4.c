kg_seal_iov(OM_uint32 *minor_status,
            gss_ctx_id_t context_handle,
            int conf_req_flag,
            gss_qop_t qop_req,
            int *conf_state,
            gss_iov_buffer_desc *iov,
            int iov_count,
            int toktype)
{
    krb5_gss_ctx_id_rec *ctx;
    krb5_error_code code;
    krb5_context context;

    if (qop_req != 0) {
        *minor_status = (OM_uint32)G_UNKNOWN_QOP;
        return GSS_S_FAILURE;
     }
 
     ctx = (krb5_gss_ctx_id_rec *)context_handle;
    if (ctx->terminated || !ctx->established) {
         *minor_status = KG_CTX_INCOMPLETE;
         return GSS_S_NO_CONTEXT;
     }

    if (conf_req_flag && kg_integ_only_iov(iov, iov_count)) {
        /* may be more sensible to return an error here */
        conf_req_flag = FALSE;
    }

    context = ctx->k5_context;
    switch (ctx->proto) {
    case 0:
        code = make_seal_token_v1_iov(context, ctx, conf_req_flag,
                                      conf_state, iov, iov_count, toktype);
        break;
    case 1:
        code = gss_krb5int_make_seal_token_v3_iov(context, ctx, conf_req_flag,
                                                  conf_state, iov, iov_count, toktype);
        break;
    default:
        code = G_UNKNOWN_QOP;
        break;
    }

    if (code != 0) {
        *minor_status = code;
        save_error_info(*minor_status, context);
        return GSS_S_FAILURE;
    }

    *minor_status = 0;

    return GSS_S_COMPLETE;
}
