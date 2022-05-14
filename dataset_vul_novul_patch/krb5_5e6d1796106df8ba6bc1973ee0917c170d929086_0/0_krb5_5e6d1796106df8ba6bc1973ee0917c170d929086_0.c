s4u_identify_user(krb5_context context,
                  krb5_creds *in_creds,
                  krb5_data *subject_cert,
                  krb5_principal *canon_user)
{
    krb5_error_code code;
    krb5_preauthtype ptypes[1] = { KRB5_PADATA_S4U_X509_USER };
    krb5_creds creds;
    int use_master = 0;
    krb5_get_init_creds_opt *opts = NULL;
    krb5_principal_data client;
    krb5_s4u_userid userid;

    *canon_user = NULL;

    if (in_creds->client == NULL && subject_cert == NULL) {
        return EINVAL;
    }

    if (in_creds->client != NULL &&
        in_creds->client->type != KRB5_NT_ENTERPRISE_PRINCIPAL) {
        int anonymous;

        anonymous = krb5_principal_compare(context, in_creds->client,
                                           krb5_anonymous_principal());

        return krb5_copy_principal(context,
                                   anonymous ? in_creds->server
                                   : in_creds->client,
                                   canon_user);
    }

    memset(&creds, 0, sizeof(creds));

    memset(&userid, 0, sizeof(userid));
    if (subject_cert != NULL)
        userid.subject_cert = *subject_cert;

    code = krb5_get_init_creds_opt_alloc(context, &opts);
    if (code != 0)
        goto cleanup;
    krb5_get_init_creds_opt_set_tkt_life(opts, 15);
    krb5_get_init_creds_opt_set_renew_life(opts, 0);
    krb5_get_init_creds_opt_set_forwardable(opts, 0);
    krb5_get_init_creds_opt_set_proxiable(opts, 0);
    krb5_get_init_creds_opt_set_canonicalize(opts, 1);
    krb5_get_init_creds_opt_set_preauth_list(opts, ptypes, 1);

    if (in_creds->client != NULL) {
        client = *in_creds->client;
        client.realm = in_creds->server->realm;
    } else {
        client.magic = KV5M_PRINCIPAL;
        client.realm = in_creds->server->realm;
        /* should this be NULL, empty or a fixed string? XXX */
        client.data = NULL;
        client.length = 0;
        client.type = KRB5_NT_ENTERPRISE_PRINCIPAL;
    }

     code = k5_get_init_creds(context, &creds, &client, NULL, NULL, 0, NULL,
                              opts, krb5_get_as_key_noop, &userid, &use_master,
                              NULL);
    if (!code || code == KRB5_PREAUTH_FAILED || code == KRB5KDC_ERR_KEY_EXP) {
         *canon_user = userid.user;
         userid.user = NULL;
         code = 0;
    }

cleanup:
    krb5_free_cred_contents(context, &creds);
    if (opts != NULL)
        krb5_get_init_creds_opt_free(context, opts);
    if (userid.user != NULL)
        krb5_free_principal(context, userid.user);

    return code;
}
