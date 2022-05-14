crypto_retrieve_X509_sans(krb5_context context,
                          pkinit_plg_crypto_context plgctx,
                          pkinit_req_crypto_context reqctx,
                          X509 *cert,
                          krb5_principal **princs_ret,
                          krb5_principal **upn_ret,
                          unsigned char ***dns_ret)
{
    krb5_error_code retval = EINVAL;
    char buf[DN_BUF_LEN];
    int p = 0, u = 0, d = 0, ret = 0, l;
    krb5_principal *princs = NULL;
    krb5_principal *upns = NULL;
    unsigned char **dnss = NULL;
    unsigned int i, num_found = 0, num_sans = 0;
    X509_EXTENSION *ext = NULL;
    GENERAL_NAMES *ialt = NULL;
    GENERAL_NAME *gen = NULL;

    if (princs_ret != NULL)
        *princs_ret = NULL;
    if (upn_ret != NULL)
        *upn_ret = NULL;
    if (dns_ret != NULL)
        *dns_ret = NULL;

    if (princs_ret == NULL && upn_ret == NULL && dns_ret == NULL) {
        pkiDebug("%s: nowhere to return any values!\n", __FUNCTION__);
        return retval;
    }

    if (cert == NULL) {
        pkiDebug("%s: no certificate!\n", __FUNCTION__);
        return retval;
    }

    X509_NAME_oneline(X509_get_subject_name(cert),
                      buf, sizeof(buf));
    pkiDebug("%s: looking for SANs in cert = %s\n", __FUNCTION__, buf);

    l = X509_get_ext_by_NID(cert, NID_subject_alt_name, -1);
    if (l < 0)
        return 0;
 
     if (!(ext = X509_get_ext(cert, l)) || !(ialt = X509V3_EXT_d2i(ext))) {
         pkiDebug("%s: found no subject alt name extensions\n", __FUNCTION__);
         goto cleanup;
     }
     num_sans = sk_GENERAL_NAME_num(ialt);

    pkiDebug("%s: found %d subject alt name extension(s)\n", __FUNCTION__,
             num_sans);

    /* OK, we're likely returning something. Allocate return values */
    if (princs_ret != NULL) {
        princs = calloc(num_sans + 1, sizeof(krb5_principal));
        if (princs == NULL) {
            retval = ENOMEM;
            goto cleanup;
        }
    }
    if (upn_ret != NULL) {
        upns = calloc(num_sans + 1, sizeof(krb5_principal));
        if (upns == NULL) {
            retval = ENOMEM;
            goto cleanup;
        }
    }
    if (dns_ret != NULL) {
        dnss = calloc(num_sans + 1, sizeof(*dnss));
        if (dnss == NULL) {
            retval = ENOMEM;
            goto cleanup;
        }
    }

    for (i = 0; i < num_sans; i++) {
        krb5_data name = { 0, 0, NULL };

        gen = sk_GENERAL_NAME_value(ialt, i);
        switch (gen->type) {
        case GEN_OTHERNAME:
            name.length = gen->d.otherName->value->value.sequence->length;
            name.data = (char *)gen->d.otherName->value->value.sequence->data;
            if (princs != NULL &&
                OBJ_cmp(plgctx->id_pkinit_san,
                        gen->d.otherName->type_id) == 0) {
#ifdef DEBUG_ASN1
                print_buffer_bin((unsigned char *)name.data, name.length,
                                 "/tmp/pkinit_san");
#endif
                ret = k5int_decode_krb5_principal_name(&name, &princs[p]);
                if (ret) {
                    pkiDebug("%s: failed decoding pkinit san value\n",
                             __FUNCTION__);
                } else {
                    p++;
                    num_found++;
                }
            } else if (upns != NULL &&
                       OBJ_cmp(plgctx->id_ms_san_upn,
                               gen->d.otherName->type_id) == 0) {
                /* Prevent abuse of embedded null characters. */
                if (memchr(name.data, '\0', name.length))
                    break;
                ret = krb5_parse_name_flags(context, name.data,
                                            KRB5_PRINCIPAL_PARSE_ENTERPRISE,
                                            &upns[u]);
                if (ret) {
                    pkiDebug("%s: failed parsing ms-upn san value\n",
                             __FUNCTION__);
                } else {
                    u++;
                    num_found++;
                }
            } else {
                pkiDebug("%s: unrecognized othername oid in SAN\n",
                         __FUNCTION__);
                continue;
            }

            break;
        case GEN_DNS:
            if (dnss != NULL) {
                /* Prevent abuse of embedded null characters. */
                if (memchr(gen->d.dNSName->data, '\0', gen->d.dNSName->length))
                    break;
                pkiDebug("%s: found dns name = %s\n", __FUNCTION__,
                         gen->d.dNSName->data);
                dnss[d] = (unsigned char *)
                    strdup((char *)gen->d.dNSName->data);
                if (dnss[d] == NULL) {
                    pkiDebug("%s: failed to duplicate dns name\n",
                             __FUNCTION__);
                } else {
                    d++;
                    num_found++;
                }
            }
            break;
        default:
            pkiDebug("%s: SAN type = %d expecting %d\n", __FUNCTION__,
                     gen->type, GEN_OTHERNAME);
        }
    }
     sk_GENERAL_NAME_pop_free(ialt, GENERAL_NAME_free);
 
     retval = 0;
    if (princs != NULL && *princs != NULL) {
         *princs_ret = princs;
        princs = NULL;
    }
    if (upns != NULL && *upns != NULL) {
         *upn_ret = upns;
        upns = NULL;
    }
    if (dnss != NULL && *dnss != NULL) {
         *dns_ret = dnss;
        dnss = NULL;
    }
 
 cleanup:
    for (i = 0; princs != NULL && princs[i] != NULL; i++)
        krb5_free_principal(context, princs[i]);
    free(princs);
    for (i = 0; upns != NULL && upns[i] != NULL; i++)
        krb5_free_principal(context, upns[i]);
    free(upns);
    for (i = 0; dnss != NULL && dnss[i] != NULL; i++)
        free(dnss[i]);
    free(dnss);
     return retval;
 }
