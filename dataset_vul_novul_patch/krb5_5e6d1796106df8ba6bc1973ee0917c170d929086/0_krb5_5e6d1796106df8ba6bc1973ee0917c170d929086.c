kdc_process_s4u2self_req(kdc_realm_t *kdc_active_realm,
                         krb5_kdc_req *request,
                         krb5_const_principal client_princ,
                         krb5_const_principal header_srv_princ,
                         krb5_boolean issuing_referral,
                         const krb5_db_entry *server,
                         krb5_keyblock *tgs_subkey,
                         krb5_keyblock *tgs_session,
                         krb5_timestamp kdc_time,
                         krb5_pa_s4u_x509_user **s4u_x509_user,
                         krb5_db_entry **princ_ptr,
                         const char **status)
{
    krb5_error_code             code;
    krb5_boolean                is_local_tgt;
    krb5_pa_data                *pa_data;
    int                         flags;
    krb5_db_entry               *princ;

    *princ_ptr = NULL;

    pa_data = krb5int_find_pa_data(kdc_context,
                                   request->padata, KRB5_PADATA_S4U_X509_USER);
    if (pa_data != NULL) {
        code = kdc_process_s4u_x509_user(kdc_context,
                                         request,
                                         pa_data,
                                         tgs_subkey,
                                         tgs_session,
                                         s4u_x509_user,
                                         status);
        if (code != 0)
            return code;
    } else {
        pa_data = krb5int_find_pa_data(kdc_context,
                                       request->padata, KRB5_PADATA_FOR_USER);
        if (pa_data != NULL) {
            code = kdc_process_for_user(kdc_active_realm,
                                        pa_data,
                                        tgs_session,
                                        s4u_x509_user,
                                        status);
            if (code != 0)
                return code;
        } else
            return 0;
    }

    /*
     * We need to compare the client name in the TGT with the requested
     * server name. Supporting server name aliases without assuming a
     * global name service makes this difficult to do.
     *
     * The comparison below handles the following cases (note that the
     * term "principal name" below excludes the realm).
     *
     * (1) The requested service is a host-based service with two name
     *     components, in which case we assume the principal name to
     *     contain sufficient qualifying information. The realm is
     *     ignored for the purpose of comparison.
     *
     * (2) The requested service name is an enterprise principal name:
     *     the service principal name is compared with the unparsed
     *     form of the client name (including its realm).
     *
     * (3) The requested service is some other name type: an exact
     *     match is required.
     *
     * An alternative would be to look up the server once again with
     * FLAG_CANONICALIZE | FLAG_CLIENT_REFERRALS_ONLY set, do an exact
     * match between the returned name and client_princ. However, this
     * assumes that the client set FLAG_CANONICALIZE when requesting
     * the TGT and that we have a global name service.
     */
    flags = 0;
    switch (krb5_princ_type(kdc_context, request->server)) {
    case KRB5_NT_SRV_HST:                   /* (1) */
        if (krb5_princ_size(kdc_context, request->server) == 2)
            flags |= KRB5_PRINCIPAL_COMPARE_IGNORE_REALM;
        break;
    case KRB5_NT_ENTERPRISE_PRINCIPAL:      /* (2) */
        flags |= KRB5_PRINCIPAL_COMPARE_ENTERPRISE;
        break;
    default:                                /* (3) */
        break;
    }

    if (!krb5_principal_compare_flags(kdc_context,
                                      request->server,
                                      client_princ,
                                      flags)) {
        *status = "INVALID_S4U2SELF_REQUEST";
        return KRB5KDC_ERR_C_PRINCIPAL_UNKNOWN; /* match Windows error code */
    }

    /*
     * Protocol transition is mutually exclusive with renew/forward/etc
     * as well as user-to-user and constrained delegation. This check
     * is also made in validate_as_request().
     *
     * We can assert from this check that the header ticket was a TGT, as
     * that is validated previously in validate_tgs_request().
     */
    if (request->kdc_options & AS_INVALID_OPTIONS) {
        *status = "INVALID AS OPTIONS";
        return KRB5KDC_ERR_BADOPTION;
    }

    /*
     * Valid S4U2Self requests can occur in the following combinations:
     *
     * (1) local TGT, local user, local server
     * (2) cross TGT, local user, issuing referral
     * (3) cross TGT, non-local user, issuing referral
     * (4) cross TGT, non-local user, local server
     *
     * The first case is for a single-realm S4U2Self scenario; the second,
     * third, and fourth cases are for the initial, intermediate (if any), and
     * final cross-realm requests in a multi-realm scenario.
     */

    is_local_tgt = !is_cross_tgs_principal(header_srv_princ);
    if (is_local_tgt && issuing_referral) {
        /* The requesting server appears to no longer exist, and we found
         * a referral instead.  Treat this as a server lookup failure. */
        *status = "LOOKING_UP_SERVER";
        return KRB5KDC_ERR_S_PRINCIPAL_UNKNOWN;
    }

    /*
     * Do not attempt to lookup principals in foreign realms.
     */
    if (is_local_principal(kdc_active_realm,
                           (*s4u_x509_user)->user_id.user)) {
        krb5_db_entry no_server;
        krb5_pa_data **e_data = NULL;

        if (!is_local_tgt && !issuing_referral) {
            /* A local server should not need a cross-realm TGT to impersonate
             * a local principal. */
            *status = "NOT_CROSS_REALM_REQUEST";
            return KRB5KDC_ERR_C_PRINCIPAL_UNKNOWN; /* match Windows error */
        }

        code = krb5_db_get_principal(kdc_context,
                                     (*s4u_x509_user)->user_id.user,
                                     KRB5_KDB_FLAG_INCLUDE_PAC, &princ);
        if (code == KRB5_KDB_NOENTRY) {
            *status = "UNKNOWN_S4U2SELF_PRINCIPAL";
            return KRB5KDC_ERR_C_PRINCIPAL_UNKNOWN;
        } else if (code) {
            *status = "LOOKING_UP_S4U2SELF_PRINCIPAL";
            return code; /* caller can free for_user */
        }
 
         memset(&no_server, 0, sizeof(no_server));
 
        /* Ignore password expiration and needchange attributes (as Windows
         * does), since S4U2Self is not password authentication. */
        princ->pw_expiration = 0;
        clear(princ->attributes, KRB5_KDB_REQUIRES_PWCHANGE);

         code = validate_as_request(kdc_active_realm, request, *princ,
                                    no_server, kdc_time, status, &e_data);
         if (code) {
            krb5_db_free_principal(kdc_context, princ);
            krb5_free_pa_data(kdc_context, e_data);
            return code;
        }

        *princ_ptr = princ;
    } else if (is_local_tgt) {
        /*
         * The server is asking to impersonate a principal from another realm,
         * using a local TGT.  It should instead ask that principal's realm and
         * follow referrals back to us.
         */
        *status = "S4U2SELF_CLIENT_NOT_OURS";
        return KRB5KDC_ERR_POLICY; /* match Windows error */
    }

    return 0;
}
