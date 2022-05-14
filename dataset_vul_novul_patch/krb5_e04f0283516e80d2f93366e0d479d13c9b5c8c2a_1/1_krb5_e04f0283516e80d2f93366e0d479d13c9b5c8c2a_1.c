iakerb_gss_accept_sec_context(OM_uint32 *minor_status,
                              gss_ctx_id_t *context_handle,
                              gss_cred_id_t verifier_cred_handle,
                              gss_buffer_t input_token,
                              gss_channel_bindings_t input_chan_bindings,
                              gss_name_t *src_name,
                              gss_OID *mech_type,
                              gss_buffer_t output_token,
                              OM_uint32 *ret_flags,
                              OM_uint32 *time_rec,
                              gss_cred_id_t *delegated_cred_handle)
{
    OM_uint32 major_status = GSS_S_FAILURE;
    OM_uint32 code;
    iakerb_ctx_id_t ctx;
     int initialContextToken = (*context_handle == GSS_C_NO_CONTEXT);
 
     if (initialContextToken) {
        code = iakerb_alloc_context(&ctx);
         if (code != 0)
             goto cleanup;
 
    } else
        ctx = (iakerb_ctx_id_t)*context_handle;

    if (iakerb_is_iakerb_token(input_token)) {
        if (ctx->gssc != GSS_C_NO_CONTEXT) {
            /* We shouldn't get an IAKERB token now. */
            code = G_WRONG_TOKID;
            major_status = GSS_S_DEFECTIVE_TOKEN;
            goto cleanup;
        }
        code = iakerb_acceptor_step(ctx, initialContextToken,
                                    input_token, output_token);
        if (code == (OM_uint32)KRB5_BAD_MSIZE)
            major_status = GSS_S_DEFECTIVE_TOKEN;
        if (code != 0)
            goto cleanup;
        if (initialContextToken) {
            *context_handle = (gss_ctx_id_t)ctx;
            ctx = NULL;
        }
        if (src_name != NULL)
            *src_name = GSS_C_NO_NAME;
        if (mech_type != NULL)
            *mech_type = (gss_OID)gss_mech_iakerb;
        if (ret_flags != NULL)
            *ret_flags = 0;
        if (time_rec != NULL)
            *time_rec = 0;
        if (delegated_cred_handle != NULL)
            *delegated_cred_handle = GSS_C_NO_CREDENTIAL;
        major_status = GSS_S_CONTINUE_NEEDED;
    } else {
        krb5_gss_ctx_ext_rec exts;

        iakerb_make_exts(ctx, &exts);

        major_status = krb5_gss_accept_sec_context_ext(&code,
                                                       &ctx->gssc,
                                                       verifier_cred_handle,
                                                       input_token,
                                                       input_chan_bindings,
                                                       src_name,
                                                       NULL,
                                                       output_token,
                                                       ret_flags,
                                                        time_rec,
                                                        delegated_cred_handle,
                                                        &exts);
        if (major_status == GSS_S_COMPLETE) {
            *context_handle = ctx->gssc;
            ctx->gssc = NULL;
            iakerb_release_context(ctx);
        }
         if (mech_type != NULL)
             *mech_type = (gss_OID)gss_mech_krb5;
     }

cleanup:
    if (initialContextToken && GSS_ERROR(major_status)) {
        iakerb_release_context(ctx);
        *context_handle = GSS_C_NO_CONTEXT;
    }

    *minor_status = code;
    return major_status;
}
