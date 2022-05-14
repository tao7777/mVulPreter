pkinit_server_verify_padata(krb5_context context,
                            krb5_data *req_pkt,
                            krb5_kdc_req * request,
                            krb5_enc_tkt_part * enc_tkt_reply,
                            krb5_pa_data * data,
                            krb5_kdcpreauth_callbacks cb,
                            krb5_kdcpreauth_rock rock,
                            krb5_kdcpreauth_moddata moddata,
                            krb5_kdcpreauth_verify_respond_fn respond,
                            void *arg)
{
    krb5_error_code retval = 0;
    krb5_data authp_data = {0, 0, NULL}, krb5_authz = {0, 0, NULL};
    krb5_pa_pk_as_req *reqp = NULL;
    krb5_pa_pk_as_req_draft9 *reqp9 = NULL;
    krb5_auth_pack *auth_pack = NULL;
    krb5_auth_pack_draft9 *auth_pack9 = NULL;
    pkinit_kdc_context plgctx = NULL;
    pkinit_kdc_req_context reqctx = NULL;
    krb5_checksum cksum = {0, 0, 0, NULL};
    krb5_data *der_req = NULL;
    int valid_eku = 0, valid_san = 0;
    krb5_data k5data;
    int is_signed = 1;
    krb5_pa_data **e_data = NULL;
    krb5_kdcpreauth_modreq modreq = NULL;
 
     pkiDebug("pkinit_verify_padata: entered!\n");
     if (data == NULL || data->length <= 0 || data->contents == NULL) {
        (*respond)(arg, EINVAL, NULL, NULL, NULL);
         return;
     }
 

    if (moddata == NULL) {
        (*respond)(arg, EINVAL, NULL, NULL, NULL);
        return;
    }
 
     plgctx = pkinit_find_realm_context(context, moddata, request->server);
     if (plgctx == NULL) {
        (*respond)(arg, EINVAL, NULL, NULL, NULL);
         return;
     }
 
#ifdef DEBUG_ASN1
    print_buffer_bin(data->contents, data->length, "/tmp/kdc_as_req");
#endif
    /* create a per-request context */
    retval = pkinit_init_kdc_req_context(context, &reqctx);
    if (retval)
        goto cleanup;
    reqctx->pa_type = data->pa_type;

    PADATA_TO_KRB5DATA(data, &k5data);

    switch ((int)data->pa_type) {
    case KRB5_PADATA_PK_AS_REQ:
        pkiDebug("processing KRB5_PADATA_PK_AS_REQ\n");
        retval = k5int_decode_krb5_pa_pk_as_req(&k5data, &reqp);
        if (retval) {
            pkiDebug("decode_krb5_pa_pk_as_req failed\n");
            goto cleanup;
        }
#ifdef DEBUG_ASN1
        print_buffer_bin(reqp->signedAuthPack.data,
                         reqp->signedAuthPack.length,
                         "/tmp/kdc_signed_data");
#endif
        retval = cms_signeddata_verify(context, plgctx->cryptoctx,
                                       reqctx->cryptoctx, plgctx->idctx, CMS_SIGN_CLIENT,
                                       plgctx->opts->require_crl_checking,
                                       (unsigned char *)
                                       reqp->signedAuthPack.data, reqp->signedAuthPack.length,
                                       (unsigned char **)&authp_data.data,
                                       &authp_data.length,
                                       (unsigned char **)&krb5_authz.data,
                                       &krb5_authz.length, &is_signed);
        break;
    case KRB5_PADATA_PK_AS_REP_OLD:
    case KRB5_PADATA_PK_AS_REQ_OLD:
        pkiDebug("processing KRB5_PADATA_PK_AS_REQ_OLD\n");
        retval = k5int_decode_krb5_pa_pk_as_req_draft9(&k5data, &reqp9);
        if (retval) {
            pkiDebug("decode_krb5_pa_pk_as_req_draft9 failed\n");
            goto cleanup;
        }
#ifdef DEBUG_ASN1
        print_buffer_bin(reqp9->signedAuthPack.data,
                         reqp9->signedAuthPack.length,
                         "/tmp/kdc_signed_data_draft9");
#endif

        retval = cms_signeddata_verify(context, plgctx->cryptoctx,
                                       reqctx->cryptoctx, plgctx->idctx, CMS_SIGN_DRAFT9,
                                       plgctx->opts->require_crl_checking,
                                       (unsigned char *)
                                       reqp9->signedAuthPack.data, reqp9->signedAuthPack.length,
                                       (unsigned char **)&authp_data.data,
                                       &authp_data.length,
                                       (unsigned char **)&krb5_authz.data,
                                       &krb5_authz.length, NULL);
        break;
    default:
        pkiDebug("unrecognized pa_type = %d\n", data->pa_type);
        retval = EINVAL;
        goto cleanup;
    }
    if (retval) {
        pkiDebug("pkcs7_signeddata_verify failed\n");
        goto cleanup;
    }
    if (is_signed) {

        retval = verify_client_san(context, plgctx, reqctx, request->client,
                                   &valid_san);
        if (retval)
            goto cleanup;
        if (!valid_san) {
            pkiDebug("%s: did not find an acceptable SAN in user "
                     "certificate\n", __FUNCTION__);
            retval = KRB5KDC_ERR_CLIENT_NAME_MISMATCH;
            goto cleanup;
        }
        retval = verify_client_eku(context, plgctx, reqctx, &valid_eku);
        if (retval)
            goto cleanup;

        if (!valid_eku) {
            pkiDebug("%s: did not find an acceptable EKU in user "
                     "certificate\n", __FUNCTION__);
            retval = KRB5KDC_ERR_INCONSISTENT_KEY_PURPOSE;
            goto cleanup;
        }
    } else { /* !is_signed */
        if (!krb5_principal_compare(context, request->client,
                                    krb5_anonymous_principal())) {
            retval = KRB5KDC_ERR_PREAUTH_FAILED;
            krb5_set_error_message(context, retval,
                                   _("Pkinit request not signed, but client "
                                     "not anonymous."));
            goto cleanup;
        }
    }
#ifdef DEBUG_ASN1
    print_buffer_bin(authp_data.data, authp_data.length, "/tmp/kdc_auth_pack");
#endif

    OCTETDATA_TO_KRB5DATA(&authp_data, &k5data);
    switch ((int)data->pa_type) {
    case KRB5_PADATA_PK_AS_REQ:
        retval = k5int_decode_krb5_auth_pack(&k5data, &auth_pack);
        if (retval) {
            pkiDebug("failed to decode krb5_auth_pack\n");
            goto cleanup;
        }

        retval = krb5_check_clockskew(context,
                                      auth_pack->pkAuthenticator.ctime);
        if (retval)
            goto cleanup;

        /* check dh parameters */
        if (auth_pack->clientPublicValue != NULL) {
            retval = server_check_dh(context, plgctx->cryptoctx,
                                     reqctx->cryptoctx, plgctx->idctx,
                                     &auth_pack->clientPublicValue->algorithm.parameters,
                                     plgctx->opts->dh_min_bits);

            if (retval) {
                pkiDebug("bad dh parameters\n");
                goto cleanup;
            }
        } else if (!is_signed) {
            /*Anonymous pkinit requires DH*/
            retval = KRB5KDC_ERR_PREAUTH_FAILED;
            krb5_set_error_message(context, retval,
                                   _("Anonymous pkinit without DH public "
                                     "value not supported."));
            goto cleanup;
        }
        der_req = cb->request_body(context, rock);
        retval = krb5_c_make_checksum(context, CKSUMTYPE_NIST_SHA, NULL,
                                      0, der_req, &cksum);
        if (retval) {
            pkiDebug("unable to calculate AS REQ checksum\n");
            goto cleanup;
        }
        if (cksum.length != auth_pack->pkAuthenticator.paChecksum.length ||
            k5_bcmp(cksum.contents,
                    auth_pack->pkAuthenticator.paChecksum.contents,
                    cksum.length) != 0) {
            pkiDebug("failed to match the checksum\n");
#ifdef DEBUG_CKSUM
            pkiDebug("calculating checksum on buf size (%d)\n",
                     req_pkt->length);
            print_buffer(req_pkt->data, req_pkt->length);
            pkiDebug("received checksum type=%d size=%d ",
                     auth_pack->pkAuthenticator.paChecksum.checksum_type,
                     auth_pack->pkAuthenticator.paChecksum.length);
            print_buffer(auth_pack->pkAuthenticator.paChecksum.contents,
                         auth_pack->pkAuthenticator.paChecksum.length);
            pkiDebug("expected checksum type=%d size=%d ",
                     cksum.checksum_type, cksum.length);
            print_buffer(cksum.contents, cksum.length);
#endif

            retval = KRB5KDC_ERR_PA_CHECKSUM_MUST_BE_INCLUDED;
            goto cleanup;
        }

        /* check if kdcPkId present and match KDC's subjectIdentifier */
        if (reqp->kdcPkId.data != NULL) {
            int valid_kdcPkId = 0;
            retval = pkinit_check_kdc_pkid(context, plgctx->cryptoctx,
                                           reqctx->cryptoctx, plgctx->idctx,
                                           (unsigned char *)reqp->kdcPkId.data,
                                           reqp->kdcPkId.length, &valid_kdcPkId);
            if (retval)
                goto cleanup;
            if (!valid_kdcPkId)
                pkiDebug("kdcPkId in AS_REQ does not match KDC's cert"
                         "RFC says to ignore and proceed\n");

        }
        /* remember the decoded auth_pack for verify_padata routine */
        reqctx->rcv_auth_pack = auth_pack;
        auth_pack = NULL;
        break;
    case KRB5_PADATA_PK_AS_REP_OLD:
    case KRB5_PADATA_PK_AS_REQ_OLD:
        retval = k5int_decode_krb5_auth_pack_draft9(&k5data, &auth_pack9);
        if (retval) {
            pkiDebug("failed to decode krb5_auth_pack_draft9\n");
            goto cleanup;
        }
        if (auth_pack9->clientPublicValue != NULL) {
            retval = server_check_dh(context, plgctx->cryptoctx,
                                     reqctx->cryptoctx, plgctx->idctx,
                                     &auth_pack9->clientPublicValue->algorithm.parameters,
                                     plgctx->opts->dh_min_bits);

            if (retval) {
                pkiDebug("bad dh parameters\n");
                goto cleanup;
            }
        }
        /* remember the decoded auth_pack for verify_padata routine */
        reqctx->rcv_auth_pack9 = auth_pack9;
        auth_pack9 = NULL;
        break;
    }

    /* remember to set the PREAUTH flag in the reply */
    enc_tkt_reply->flags |= TKT_FLG_PRE_AUTH;
    modreq = (krb5_kdcpreauth_modreq)reqctx;
    reqctx = NULL;

cleanup:
    if (retval && data->pa_type == KRB5_PADATA_PK_AS_REQ) {
        pkiDebug("pkinit_verify_padata failed: creating e-data\n");
        if (pkinit_create_edata(context, plgctx->cryptoctx, reqctx->cryptoctx,
                                plgctx->idctx, plgctx->opts, retval, &e_data))
            pkiDebug("pkinit_create_edata failed\n");
    }

    switch ((int)data->pa_type) {
    case KRB5_PADATA_PK_AS_REQ:
        free_krb5_pa_pk_as_req(&reqp);
        free(cksum.contents);
        break;
    case KRB5_PADATA_PK_AS_REP_OLD:
    case KRB5_PADATA_PK_AS_REQ_OLD:
        free_krb5_pa_pk_as_req_draft9(&reqp9);
    }
    free(authp_data.data);
    free(krb5_authz.data);
    if (reqctx != NULL)
        pkinit_fini_kdc_req_context(context, reqctx);
    free_krb5_auth_pack(&auth_pack);
    free_krb5_auth_pack_draft9(context, &auth_pack9);

    (*respond)(arg, retval, modreq, e_data, NULL);
}
