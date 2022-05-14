kg_unseal(minor_status, context_handle, input_token_buffer,
          message_buffer, conf_state, qop_state, toktype)
    OM_uint32 *minor_status;
    gss_ctx_id_t context_handle;
    gss_buffer_t input_token_buffer;
    gss_buffer_t message_buffer;
    int *conf_state;
    gss_qop_t *qop_state;
    int toktype;
{
    krb5_gss_ctx_id_rec *ctx;
    unsigned char *ptr;
    unsigned int bodysize;
    int err;
    int toktype2;
    int vfyflags = 0;
    OM_uint32 ret;
 
     ctx = (krb5_gss_ctx_id_rec *) context_handle;
 
    if (ctx->terminated || !ctx->established) {
         *minor_status = KG_CTX_INCOMPLETE;
         return(GSS_S_NO_CONTEXT);
     }

    /* parse the token, leave the data in message_buffer, setting conf_state */

    /* verify the header */

    ptr = (unsigned char *) input_token_buffer->value;


    err = g_verify_token_header(ctx->mech_used,
                                &bodysize, &ptr, -1,
                                input_token_buffer->length,
                                vfyflags);
    if (err) {
        *minor_status = err;
        return GSS_S_DEFECTIVE_TOKEN;
    }

    if (bodysize < 2) {
        *minor_status = (OM_uint32)G_BAD_TOK_HEADER;
        return GSS_S_DEFECTIVE_TOKEN;
    }

    toktype2 = load_16_be(ptr);

    ptr += 2;
    bodysize -= 2;

    switch (toktype2) {
    case KG2_TOK_MIC_MSG:
    case KG2_TOK_WRAP_MSG:
    case KG2_TOK_DEL_CTX:
        ret = gss_krb5int_unseal_token_v3(&ctx->k5_context, minor_status, ctx,
                                          ptr, bodysize, message_buffer,
                                          conf_state, qop_state, toktype);
        break;
    case KG_TOK_MIC_MSG:
    case KG_TOK_WRAP_MSG:
    case KG_TOK_DEL_CTX:
        ret = kg_unseal_v1(ctx->k5_context, minor_status, ctx, ptr, bodysize,
                           message_buffer, conf_state, qop_state,
                           toktype);
        break;
    default:
        *minor_status = (OM_uint32)G_BAD_TOK_HEADER;
        ret = GSS_S_DEFECTIVE_TOKEN;
        break;
    }

    if (ret != 0)
        save_error_info (*minor_status, ctx->k5_context);

    return ret;
}
