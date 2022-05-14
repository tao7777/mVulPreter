pkinit_eku_authorize(krb5_context context, krb5_certauth_moddata moddata,
                     const uint8_t *cert, size_t cert_len,
                     krb5_const_principal princ, const void *opts,
                     const struct _krb5_db_entry_new *db_entry,
                     char ***authinds_out)
{
    krb5_error_code ret;
    int valid_eku;
    const struct certauth_req_opts *req_opts = opts;

    *authinds_out = NULL;

    /* Verify the client EKU. */
    ret = verify_client_eku(context, req_opts->plgctx, req_opts->reqctx,
                            &valid_eku);
    if (ret)
        return ret;

    if (!valid_eku) {
        TRACE_PKINIT_SERVER_EKU_REJECT(context);
         return KRB5KDC_ERR_INCONSISTENT_KEY_PURPOSE;
     }
 
    return 0;
 }
