verify_client_san(krb5_context context,
                  pkinit_kdc_context plgctx,
                  pkinit_kdc_req_context reqctx,
                  krb5_kdcpreauth_callbacks cb,
                  krb5_kdcpreauth_rock rock,
                  krb5_const_principal client,
                  int *valid_san)
{
    krb5_error_code retval;
    krb5_principal *princs = NULL;
    krb5_principal *upns = NULL;
    int i;
#ifdef DEBUG_SAN_INFO
    char *client_string = NULL, *san_string;
#endif

    *valid_san = 0;
    retval = crypto_retrieve_cert_sans(context, plgctx->cryptoctx,
                                       reqctx->cryptoctx, plgctx->idctx,
                                        &princs,
                                        plgctx->opts->allow_upn ? &upns : NULL,
                                        NULL);
    if (retval == ENOENT) {
        TRACE_PKINIT_SERVER_NO_SAN(context);
        goto out;
    } else if (retval) {
         pkiDebug("%s: error from retrieve_certificate_sans()\n", __FUNCTION__);
         retval = KRB5KDC_ERR_CLIENT_NAME_MISMATCH;
         goto out;
     }
     /* XXX Verify this is consistent with client side XXX */
 #if 0
     retval = call_san_checking_plugins(context, plgctx, reqctx, princs,
                                       upns, NULL, &plugin_decision, &ignore);
    pkiDebug("%s: call_san_checking_plugins() returned retval %d\n",
             __FUNCTION__);
    if (retval) {
        retval = KRB5KDC_ERR_CLIENT_NAME_MISMATCH;
        goto cleanup;
    }
    pkiDebug("%s: call_san_checking_plugins() returned decision %d\n",
             __FUNCTION__, plugin_decision);
    if (plugin_decision != NO_DECISION) {
        retval = plugin_decision;
        goto out;
    }
#endif

#ifdef DEBUG_SAN_INFO
    krb5_unparse_name(context, client, &client_string);
#endif
    pkiDebug("%s: Checking pkinit sans\n", __FUNCTION__);
    for (i = 0; princs != NULL && princs[i] != NULL; i++) {
#ifdef DEBUG_SAN_INFO
        krb5_unparse_name(context, princs[i], &san_string);
        pkiDebug("%s: Comparing client '%s' to pkinit san value '%s'\n",
                 __FUNCTION__, client_string, san_string);
        krb5_free_unparsed_name(context, san_string);
#endif
        if (cb->match_client(context, rock, princs[i])) {
            TRACE_PKINIT_SERVER_MATCHING_SAN_FOUND(context);
            *valid_san = 1;
            retval = 0;
            goto out;
        }
    }
    pkiDebug("%s: no pkinit san match found\n", __FUNCTION__);
    /*
     * XXX if cert has names but none match, should we
     * be returning KRB5KDC_ERR_CLIENT_NAME_MISMATCH here?
     */

    if (upns == NULL) {
        pkiDebug("%s: no upn sans (or we wouldn't accept them anyway)\n",
                 __FUNCTION__);
        retval = KRB5KDC_ERR_CLIENT_NAME_MISMATCH;
        goto out;
    }

    pkiDebug("%s: Checking upn sans\n", __FUNCTION__);
    for (i = 0; upns[i] != NULL; i++) {
#ifdef DEBUG_SAN_INFO
        krb5_unparse_name(context, upns[i], &san_string);
        pkiDebug("%s: Comparing client '%s' to upn san value '%s'\n",
                 __FUNCTION__, client_string, san_string);
        krb5_free_unparsed_name(context, san_string);
#endif
        if (cb->match_client(context, rock, upns[i])) {
            TRACE_PKINIT_SERVER_MATCHING_UPN_FOUND(context);
            *valid_san = 1;
            retval = 0;
            goto out;
        }
    }
    pkiDebug("%s: no upn san match found\n", __FUNCTION__);

    /* We found no match */
    if (princs != NULL || upns != NULL) {
        *valid_san = 0;
        /* XXX ??? If there was one or more name in the cert, but
         * none matched the client name, then return mismatch? */
        retval = KRB5KDC_ERR_CLIENT_NAME_MISMATCH;
    }
    retval = 0;

out:
    if (princs != NULL) {
        for (i = 0; princs[i] != NULL; i++)
            krb5_free_principal(context, princs[i]);
        free(princs);
    }
    if (upns != NULL) {
        for (i = 0; upns[i] != NULL; i++)
            krb5_free_principal(context, upns[i]);
        free(upns);
    }
#ifdef DEBUG_SAN_INFO
    if (client_string != NULL)
        krb5_free_unparsed_name(context, client_string);
#endif
    pkiDebug("%s: returning retval %d, valid_san %d\n",
             __FUNCTION__, retval, *valid_san);
    return retval;
}
