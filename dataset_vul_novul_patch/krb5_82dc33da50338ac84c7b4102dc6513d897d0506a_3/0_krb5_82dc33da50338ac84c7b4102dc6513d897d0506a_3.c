kg_seal(minor_status, context_handle, conf_req_flag, qop_req,
        input_message_buffer, conf_state, output_message_buffer, toktype)
    OM_uint32 *minor_status;
    gss_ctx_id_t context_handle;
    int conf_req_flag;
    gss_qop_t qop_req;
    gss_buffer_t input_message_buffer;
    int *conf_state;
    gss_buffer_t output_message_buffer;
    int toktype;
{
    krb5_gss_ctx_id_rec *ctx;
    krb5_error_code code;
    krb5_context context;

    output_message_buffer->length = 0;
    output_message_buffer->value = NULL;

    /* Only default qop or matching established cryptosystem is allowed.

       There are NO EXTENSIONS to this set for AES and friends!  The
       new spec says "just use 0".  The old spec plus extensions would
       actually allow for certain non-zero values.  Fix this to handle
       them later.  */
    if (qop_req != 0) {
        *minor_status = (OM_uint32) G_UNKNOWN_QOP;
        return GSS_S_FAILURE;
    }
 
     ctx = (krb5_gss_ctx_id_rec *) context_handle;
 
    if (ctx->terminated || !ctx->established) {
         *minor_status = KG_CTX_INCOMPLETE;
         return(GSS_S_NO_CONTEXT);
     }

    context = ctx->k5_context;
    switch (ctx->proto)
    {
    case 0:
        code = make_seal_token_v1(context, ctx->enc, ctx->seq,
                                  &ctx->seq_send, ctx->initiate,
                                  input_message_buffer, output_message_buffer,
                                  ctx->signalg, ctx->cksum_size, ctx->sealalg,
                                  conf_req_flag, toktype, ctx->mech_used);
        break;
    case 1:
        code = gss_krb5int_make_seal_token_v3(context, ctx,
                                              input_message_buffer,
                                              output_message_buffer,
                                              conf_req_flag, toktype);
        break;
    default:
        code = G_UNKNOWN_QOP;   /* XXX */
        break;
    }

    if (code) {
        *minor_status = code;
        save_error_info(*minor_status, context);
        return(GSS_S_FAILURE);
    }

    if (conf_state)
        *conf_state = conf_req_flag;

    *minor_status = 0;
    return(GSS_S_COMPLETE);
}
