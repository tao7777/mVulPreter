validate_as_request(kdc_realm_t *kdc_active_realm,
                    register krb5_kdc_req *request, krb5_db_entry client,
                    krb5_db_entry server, krb5_timestamp kdc_time,
                    const char **status, krb5_pa_data ***e_data)
{
    int errcode;
    krb5_error_code ret;

    /*
     * If an option is set that is only allowed in TGS requests, complain.
     */
    if (request->kdc_options & AS_INVALID_OPTIONS) {
        *status = "INVALID AS OPTIONS";
        return KDC_ERR_BADOPTION;
    }

    /* The client must not be expired */
    if (client.expiration && client.expiration < kdc_time) {
        *status = "CLIENT EXPIRED";
        if (vague_errors)
            return(KRB_ERR_GENERIC);
        else
            return(KDC_ERR_NAME_EXP);
    }

    /* The client's password must not be expired, unless the server is
       a KRB5_KDC_PWCHANGE_SERVICE. */
    if (client.pw_expiration && client.pw_expiration < kdc_time &&
        !isflagset(server.attributes, KRB5_KDB_PWCHANGE_SERVICE)) {
        *status = "CLIENT KEY EXPIRED";
        if (vague_errors)
            return(KRB_ERR_GENERIC);
        else
            return(KDC_ERR_KEY_EXP);
    }

    /* The server must not be expired */
    if (server.expiration && server.expiration < kdc_time) {
        *status = "SERVICE EXPIRED";
        return(KDC_ERR_SERVICE_EXP);
    }

    /*
     * If the client requires password changing, then only allow the
     * pwchange service.
     */
    if (isflagset(client.attributes, KRB5_KDB_REQUIRES_PWCHANGE) &&
        !isflagset(server.attributes, KRB5_KDB_PWCHANGE_SERVICE)) {
        *status = "REQUIRED PWCHANGE";
        return(KDC_ERR_KEY_EXP);
    }

    /* Client and server must allow postdating tickets */
    if ((isflagset(request->kdc_options, KDC_OPT_ALLOW_POSTDATE) ||
         isflagset(request->kdc_options, KDC_OPT_POSTDATED)) &&
        (isflagset(client.attributes, KRB5_KDB_DISALLOW_POSTDATED) ||
         isflagset(server.attributes, KRB5_KDB_DISALLOW_POSTDATED))) {
        *status = "POSTDATE NOT ALLOWED";
        return(KDC_ERR_CANNOT_POSTDATE);
    }

    /*
     * A Windows KDC will return KDC_ERR_PREAUTH_REQUIRED instead of
     * KDC_ERR_POLICY in the following case:
     *
     *   - KDC_OPT_FORWARDABLE is set in KDCOptions but local
     *     policy has KRB5_KDB_DISALLOW_FORWARDABLE set for the
     *     client, and;
     *   - KRB5_KDB_REQUIRES_PRE_AUTH is set for the client but
     *     preauthentication data is absent in the request.
     *
     * Hence, this check most be done after the check for preauth
     * data, and is now performed by validate_forwardable() (the
     * contents of which were previously below).
     */

    /* Client and server must allow proxiable tickets */
    if (isflagset(request->kdc_options, KDC_OPT_PROXIABLE) &&
        (isflagset(client.attributes, KRB5_KDB_DISALLOW_PROXIABLE) ||
         isflagset(server.attributes, KRB5_KDB_DISALLOW_PROXIABLE))) {
        *status = "PROXIABLE NOT ALLOWED";
        return(KDC_ERR_POLICY);
    }

    /* Check to see if client is locked out */
    if (isflagset(client.attributes, KRB5_KDB_DISALLOW_ALL_TIX)) {
        *status = "CLIENT LOCKED OUT";
        return(KDC_ERR_CLIENT_REVOKED);
    }

    /* Check to see if server is locked out */
    if (isflagset(server.attributes, KRB5_KDB_DISALLOW_ALL_TIX)) {
        *status = "SERVICE LOCKED OUT";
        return(KDC_ERR_S_PRINCIPAL_UNKNOWN);
    }

    /* Check to see if server is allowed to be a service */
    if (isflagset(server.attributes, KRB5_KDB_DISALLOW_SVR)) {
        *status = "SERVICE NOT ALLOWED";
         return(KDC_ERR_MUST_USE_USER2USER);
     }
 
    if (check_anon(kdc_active_realm, client.princ, request->server) != 0) {
         *status = "ANONYMOUS NOT ALLOWED";
         return(KDC_ERR_POLICY);
     }

    /* Perform KDB module policy checks. */
    ret = krb5_db_check_policy_as(kdc_context, request, &client, &server,
                                  kdc_time, status, e_data);
    if (ret && ret != KRB5_PLUGIN_OP_NOTSUPP)
        return errcode_to_protocol(ret);

    /* Check against local policy. */
    errcode = against_local_policy_as(request, client, server,
                                      kdc_time, status, e_data);
    if (errcode)
        return errcode;

    return 0;
}
