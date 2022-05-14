iakerb_gss_init_sec_context(OM_uint32 *minor_status,
                            gss_cred_id_t claimant_cred_handle,
                            gss_ctx_id_t *context_handle,
                            gss_name_t target_name,
                            gss_OID mech_type,
                            OM_uint32 req_flags,
                            OM_uint32 time_req,
                            gss_channel_bindings_t input_chan_bindings,
                            gss_buffer_t input_token,
                            gss_OID *actual_mech_type,
                            gss_buffer_t output_token,
                            OM_uint32 *ret_flags,
                            OM_uint32 *time_rec)
{
    OM_uint32 major_status = GSS_S_FAILURE;
    krb5_error_code code;
    iakerb_ctx_id_t ctx;
    krb5_gss_cred_id_t kcred;
    krb5_gss_name_t kname;
    krb5_boolean cred_locked = FALSE;
     int initialContextToken = (*context_handle == GSS_C_NO_CONTEXT);
 
     if (initialContextToken) {
        code = iakerb_alloc_context(&ctx);
         if (code != 0) {
             *minor_status = code;
             goto cleanup;
        }
        if (claimant_cred_handle == GSS_C_NO_CREDENTIAL) {
            major_status = iakerb_gss_acquire_cred(minor_status, NULL,
                                                   GSS_C_INDEFINITE,
                                                   GSS_C_NULL_OID_SET,
                                                   GSS_C_INITIATE,
                                                   &ctx->defcred, NULL, NULL);
            if (GSS_ERROR(major_status))
                goto cleanup;
            claimant_cred_handle = ctx->defcred;
        }
    } else {
        ctx = (iakerb_ctx_id_t)*context_handle;
        if (claimant_cred_handle == GSS_C_NO_CREDENTIAL)
            claimant_cred_handle = ctx->defcred;
    }

    kname = (krb5_gss_name_t)target_name;

    major_status = kg_cred_resolve(minor_status, ctx->k5c,
                                   claimant_cred_handle, target_name);
    if (GSS_ERROR(major_status))
        goto cleanup;
    cred_locked = TRUE;
    kcred = (krb5_gss_cred_id_t)claimant_cred_handle;

    major_status = GSS_S_FAILURE;

    if (initialContextToken) {
        code = iakerb_get_initial_state(ctx, kcred, kname, time_req,
                                        &ctx->state);
        if (code != 0) {
            *minor_status = code;
            goto cleanup;
        }
        *context_handle = (gss_ctx_id_t)ctx;
    }

    if (ctx->state != IAKERB_AP_REQ) {
        /* We need to do IAKERB. */
        code = iakerb_initiator_step(ctx,
                                     kcred,
                                     kname,
                                     time_req,
                                     input_token,
                                     output_token);
        if (code == KRB5_BAD_MSIZE)
            major_status = GSS_S_DEFECTIVE_TOKEN;
        if (code != 0) {
            *minor_status = code;
            goto cleanup;
        }
    }

    if (ctx->state == IAKERB_AP_REQ) {
        krb5_gss_ctx_ext_rec exts;

        if (cred_locked) {
            k5_mutex_unlock(&kcred->lock);
            cred_locked = FALSE;
        }

        iakerb_make_exts(ctx, &exts);

        if (ctx->gssc == GSS_C_NO_CONTEXT)
            input_token = GSS_C_NO_BUFFER;

        /* IAKERB is finished, or we skipped to Kerberos directly. */
        major_status = krb5_gss_init_sec_context_ext(minor_status,
                                                     (gss_cred_id_t) kcred,
                                                     &ctx->gssc,
                                                     target_name,
                                                     (gss_OID)gss_mech_iakerb,
                                                     req_flags,
                                                     time_req,
                                                     input_chan_bindings,
                                                     input_token,
                                                     NULL,
                                                     output_token,
                                                      ret_flags,
                                                      time_rec,
                                                      &exts);
        if (major_status == GSS_S_COMPLETE) {
            *context_handle = ctx->gssc;
            ctx->gssc = GSS_C_NO_CONTEXT;
            iakerb_release_context(ctx);
        }
         if (actual_mech_type != NULL)
             *actual_mech_type = (gss_OID)gss_mech_krb5;
     } else {
        if (actual_mech_type != NULL)
            *actual_mech_type = (gss_OID)gss_mech_iakerb;
        if (ret_flags != NULL)
            *ret_flags = 0;
        if (time_rec != NULL)
            *time_rec = 0;
        major_status = GSS_S_CONTINUE_NEEDED;
    }

cleanup:
    if (cred_locked)
        k5_mutex_unlock(&kcred->lock);
    if (initialContextToken && GSS_ERROR(major_status)) {
        iakerb_release_context(ctx);
        *context_handle = GSS_C_NO_CONTEXT;
    }
 
     return major_status;
 }
