find_alternate_tgs(kdc_realm_t *kdc_active_realm, krb5_principal princ,
                   krb5_db_entry **server_ptr, const char **status)
{
    krb5_error_code retval;
    krb5_principal *plist = NULL, *pl2;
    krb5_data tmp;
    krb5_db_entry *server = NULL;

    *server_ptr = NULL;
    assert(is_cross_tgs_principal(princ));
    if ((retval = krb5_walk_realm_tree(kdc_context,
                                       krb5_princ_realm(kdc_context, princ),
                                       krb5_princ_component(kdc_context, princ, 1),
                                       &plist, KRB5_REALM_BRANCH_CHAR))) {
        goto cleanup;
    }
    /* move to the end */
    for (pl2 = plist; *pl2; pl2++);

    /* the first entry in this array is for krbtgt/local@local, so we
       ignore it */
    while (--pl2 > plist) {
        tmp = *krb5_princ_realm(kdc_context, *pl2);
        krb5_princ_set_realm(kdc_context, *pl2,
                             krb5_princ_realm(kdc_context, tgs_server));
        retval = db_get_svc_princ(kdc_context, *pl2, 0, &server, status);
        krb5_princ_set_realm(kdc_context, *pl2, &tmp);
        if (retval == KRB5_KDB_NOENTRY)
            continue;
        else if (retval)
            goto cleanup;

        log_tgs_alt_tgt(kdc_context, server->princ);
        *server_ptr = server;
        server = NULL;
         goto cleanup;
     }
 cleanup:
    if (retval == 0 && server_ptr == NULL)
        retval = KRB5_KDB_NOENTRY;
     if (retval != 0)
         *status = "UNKNOWN_SERVER";
 
    krb5_free_realm_tree(kdc_context, plist);
    krb5_db_free_principal(kdc_context, server);
    return retval;
}
