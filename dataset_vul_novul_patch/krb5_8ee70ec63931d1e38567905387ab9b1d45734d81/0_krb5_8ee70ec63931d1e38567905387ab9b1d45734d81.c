prep_reprocess_req(krb5_kdc_req *request, krb5_principal *krbtgt_princ)
{
    krb5_error_code retval = KRB5KRB_AP_ERR_BADMATCH;
    char **realms, **cpp, *temp_buf=NULL;
    krb5_data *comp1 = NULL, *comp2 = NULL;
    char *comp1_str = NULL;

    /* By now we know that server principal name is unknown.
     * If CANONICALIZE flag is set in the request
     * If req is not U2U authn. req
     * the requested server princ. has exactly two components
     * either
     *      the name type is NT-SRV-HST
     *      or name type is NT-UNKNOWN and
     *         the 1st component is listed in conf file under host_based_services
     * the 1st component is not in a list in conf under "no_host_referral"
     * the 2d component looks like fully-qualified domain name (FQDN)
     * If all of these conditions are satisfied - try mapping the FQDN and
     * re-process the request as if client had asked for cross-realm TGT.
     */
    if (isflagset(request->kdc_options, KDC_OPT_CANONICALIZE) &&
        !isflagset(request->kdc_options, KDC_OPT_ENC_TKT_IN_SKEY) &&
        krb5_princ_size(kdc_context, request->server) == 2) {

        comp1 = krb5_princ_component(kdc_context, request->server, 0);
        comp2 = krb5_princ_component(kdc_context, request->server, 1);

        comp1_str = calloc(1,comp1->length+1);
        if (!comp1_str) {
             retval = ENOMEM;
             goto cleanup;
         }
        if (comp1->data != NULL)
            memcpy(comp1_str, comp1->data, comp1->length);
 
         if ((krb5_princ_type(kdc_context, request->server) == KRB5_NT_SRV_HST ||
              krb5_princ_type(kdc_context, request->server) == KRB5_NT_SRV_INST ||
             (krb5_princ_type(kdc_context, request->server) == KRB5_NT_UNKNOWN &&
              kdc_active_realm->realm_host_based_services != NULL &&
              (krb5_match_config_pattern(kdc_active_realm->realm_host_based_services,
                                         comp1_str) == TRUE ||
               krb5_match_config_pattern(kdc_active_realm->realm_host_based_services,
                                         KRB5_CONF_ASTERISK) == TRUE))) &&
            (kdc_active_realm->realm_no_host_referral == NULL ||
             (krb5_match_config_pattern(kdc_active_realm->realm_no_host_referral,
                                        KRB5_CONF_ASTERISK) == FALSE &&
              krb5_match_config_pattern(kdc_active_realm->realm_no_host_referral,
                                        comp1_str) == FALSE))) {

            if (memchr(comp2->data, '.', comp2->length) == NULL)
                goto cleanup;
            temp_buf = calloc(1, comp2->length+1);
            if (!temp_buf) {
                 retval = ENOMEM;
                 goto cleanup;
             }
            if (comp2->data != NULL)
                memcpy(temp_buf, comp2->data, comp2->length);
             retval = krb5int_get_domain_realm_mapping(kdc_context, temp_buf, &realms);
             free(temp_buf);
             if (retval) {
                /* no match found */
                kdc_err(kdc_context, retval, "unable to find realm of host");
                goto cleanup;
            }
            if (realms == 0) {
                retval = KRB5KRB_AP_ERR_BADMATCH;
                goto cleanup;
            }
            /* Don't return a referral to the null realm or the service
             * realm. */
            if (realms[0] == 0 ||
                data_eq_string(request->server->realm, realms[0])) {
                free(realms[0]);
                free(realms);
                retval = KRB5KRB_AP_ERR_BADMATCH;
                goto cleanup;
            }
            /* Modify request.
             * Construct cross-realm tgt :  krbtgt/REMOTE_REALM@LOCAL_REALM
             * and use it as a principal in this req.
             */
            retval = krb5_build_principal(kdc_context, krbtgt_princ,
                                          (*request->server).realm.length,
                                          (*request->server).realm.data,
                                          "krbtgt", realms[0], (char *)0);
            for (cpp = realms; *cpp; cpp++)
                free(*cpp);
        }
    }
cleanup:
    free(comp1_str);

    return retval;
}
