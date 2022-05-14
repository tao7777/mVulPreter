kdc_process_s4u2proxy_req(kdc_realm_t *kdc_active_realm,
                          krb5_kdc_req *request,
                          const krb5_enc_tkt_part *t2enc,
                          const krb5_db_entry *server,
                          krb5_const_principal server_princ,
                          krb5_const_principal proxy_princ,
                          const char **status)
{
    krb5_error_code errcode;

    /*
     * Constrained delegation is mutually exclusive with renew/forward/etc.
     * We can assert from this check that the header ticket was a TGT, as
      * that is validated previously in validate_tgs_request().
      */
     if (request->kdc_options & (NON_TGT_OPTION | KDC_OPT_ENC_TKT_IN_SKEY)) {
        *status = "INVALID_S4U2PROXY_OPTIONS";
         return KRB5KDC_ERR_BADOPTION;
     }
 
     /* Ensure that evidence ticket server matches TGT client */
     if (!krb5_principal_compare(kdc_context,
                                 server->princ, /* after canon */
                                 server_princ)) {
        *status = "EVIDENCE_TICKET_MISMATCH";
         return KRB5KDC_ERR_SERVER_NOMATCH;
     }
 
    if (!isflagset(t2enc->flags, TKT_FLG_FORWARDABLE)) {
        *status = "EVIDENCE_TKT_NOT_FORWARDABLE";
        return KRB5_TKT_NOT_FORWARDABLE;
    }

    /* Backend policy check */
    errcode = check_allowed_to_delegate_to(kdc_context,
                                           t2enc->client,
                                           server,
                                           proxy_princ);
    if (errcode) {
        *status = "NOT_ALLOWED_TO_DELEGATE";
        return errcode;
    }

    return 0;
}
