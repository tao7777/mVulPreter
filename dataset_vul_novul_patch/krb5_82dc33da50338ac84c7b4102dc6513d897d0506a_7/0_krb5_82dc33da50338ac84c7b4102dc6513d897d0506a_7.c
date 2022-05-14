gss_krb5int_export_lucid_sec_context(
    OM_uint32           *minor_status,
    const gss_ctx_id_t  context_handle,
    const gss_OID       desired_object,
    gss_buffer_set_t    *data_set)
{
    krb5_error_code     kret = 0;
    OM_uint32           retval;
    krb5_gss_ctx_id_t   ctx = (krb5_gss_ctx_id_t)context_handle;
    void                *lctx = NULL;
    int                 version = 0;
    gss_buffer_desc     rep;

    /* Assume failure */
    retval = GSS_S_FAILURE;
     *minor_status = 0;
     *data_set = GSS_C_NO_BUFFER_SET;
 
    if (ctx->terminated || !ctx->established) {
        *minor_status = KG_CTX_INCOMPLETE;
        return GSS_S_NO_CONTEXT;
    }

     retval = generic_gss_oid_decompose(minor_status,
                                        GSS_KRB5_EXPORT_LUCID_SEC_CONTEXT_OID,
                                        GSS_KRB5_EXPORT_LUCID_SEC_CONTEXT_OID_LENGTH,
                                       desired_object,
                                       &version);
    if (GSS_ERROR(retval))
        return retval;

    /* Externalize a structure of the right version */
    switch (version) {
    case 1:
        kret = make_external_lucid_ctx_v1((krb5_pointer)ctx,
                                          version, &lctx);
        break;
    default:
        kret = (OM_uint32) KG_LUCID_VERSION;
        break;
    }

    if (kret)
        goto error_out;

    rep.value = &lctx;
    rep.length = sizeof(lctx);

    retval = generic_gss_add_buffer_set_member(minor_status, &rep, data_set);
    if (GSS_ERROR(retval))
        goto error_out;

error_out:
    if (*minor_status == 0)
        *minor_status = (OM_uint32) kret;
    return(retval);
}
