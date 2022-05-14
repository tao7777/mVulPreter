pkinit_server_return_padata(krb5_context context,
                            krb5_pa_data * padata,
                            krb5_data *req_pkt,
                            krb5_kdc_req * request,
                            krb5_kdc_rep * reply,
                            krb5_keyblock * encrypting_key,
                            krb5_pa_data ** send_pa,
                            krb5_kdcpreauth_callbacks cb,
                            krb5_kdcpreauth_rock rock,
                            krb5_kdcpreauth_moddata moddata,
                            krb5_kdcpreauth_modreq modreq)
{
    krb5_error_code retval = 0;
    krb5_data scratch = {0, 0, NULL};
    krb5_pa_pk_as_req *reqp = NULL;
    krb5_pa_pk_as_req_draft9 *reqp9 = NULL;
    int i = 0;

    unsigned char *subjectPublicKey = NULL;
    unsigned char *dh_pubkey = NULL, *server_key = NULL;
    unsigned int subjectPublicKey_len = 0;
    unsigned int server_key_len = 0, dh_pubkey_len = 0;

    krb5_kdc_dh_key_info dhkey_info;
    krb5_data *encoded_dhkey_info = NULL;
    krb5_pa_pk_as_rep *rep = NULL;
    krb5_pa_pk_as_rep_draft9 *rep9 = NULL;
    krb5_data *out_data = NULL;
    krb5_octet_data secret;

    krb5_enctype enctype = -1;

    krb5_reply_key_pack *key_pack = NULL;
    krb5_reply_key_pack_draft9 *key_pack9 = NULL;
    krb5_data *encoded_key_pack = NULL;

    pkinit_kdc_context plgctx;
    pkinit_kdc_req_context reqctx;

    int fixed_keypack = 0;

    *send_pa = NULL;
    if (padata->pa_type == KRB5_PADATA_PKINIT_KX) {
        return return_pkinit_kx(context, request, reply,
                                encrypting_key, send_pa);
    }
    if (padata->length <= 0 || padata->contents == NULL)
        return 0;

    if (modreq == NULL) {
        pkiDebug("missing request context \n");
        return EINVAL;
    }

    plgctx = pkinit_find_realm_context(context, moddata, request->server);
    if (plgctx == NULL) {
        pkiDebug("Unable to locate correct realm context\n");
        return ENOENT;
    }

    pkiDebug("pkinit_return_padata: entered!\n");
    reqctx = (pkinit_kdc_req_context)modreq;

    if (encrypting_key->contents) {
        free(encrypting_key->contents);
        encrypting_key->length = 0;
        encrypting_key->contents = NULL;
    }

    for(i = 0; i < request->nktypes; i++) {
        enctype = request->ktype[i];
        if (!krb5_c_valid_enctype(enctype))
            continue;
        else {
            pkiDebug("KDC picked etype = %d\n", enctype);
            break;
        }
    }

    if (i == request->nktypes) {
        retval = KRB5KDC_ERR_ETYPE_NOSUPP;
        goto cleanup;
    }

    switch((int)reqctx->pa_type) {
    case KRB5_PADATA_PK_AS_REQ:
        init_krb5_pa_pk_as_rep(&rep);
        if (rep == NULL) {
            retval = ENOMEM;
            goto cleanup;
        }
        /* let's assume it's RSA. we'll reset it to DH if needed */
        rep->choice = choice_pa_pk_as_rep_encKeyPack;
        break;
    case KRB5_PADATA_PK_AS_REP_OLD:
    case KRB5_PADATA_PK_AS_REQ_OLD:
        init_krb5_pa_pk_as_rep_draft9(&rep9);
        if (rep9 == NULL) {
            retval = ENOMEM;
            goto cleanup;
        }
        rep9->choice = choice_pa_pk_as_rep_draft9_encKeyPack;
        break;
    default:
        retval = KRB5KDC_ERR_PREAUTH_FAILED;
        goto cleanup;
    }

    if (reqctx->rcv_auth_pack != NULL &&
        reqctx->rcv_auth_pack->clientPublicValue != NULL) {
        subjectPublicKey =
            reqctx->rcv_auth_pack->clientPublicValue->subjectPublicKey.data;
        subjectPublicKey_len =
            reqctx->rcv_auth_pack->clientPublicValue->subjectPublicKey.length;
        rep->choice = choice_pa_pk_as_rep_dhInfo;
    } else if (reqctx->rcv_auth_pack9 != NULL &&
               reqctx->rcv_auth_pack9->clientPublicValue != NULL) {
        subjectPublicKey =
            reqctx->rcv_auth_pack9->clientPublicValue->subjectPublicKey.data;
        subjectPublicKey_len =
            reqctx->rcv_auth_pack9->clientPublicValue->subjectPublicKey.length;
        rep9->choice = choice_pa_pk_as_rep_draft9_dhSignedData;
    }

    /* if this DH, then process finish computing DH key */
    if (rep != NULL && (rep->choice == choice_pa_pk_as_rep_dhInfo ||
                        rep->choice == choice_pa_pk_as_rep_draft9_dhSignedData)) {
        pkiDebug("received DH key delivery AS REQ\n");
        retval = server_process_dh(context, plgctx->cryptoctx,
                                   reqctx->cryptoctx, plgctx->idctx, subjectPublicKey,
                                   subjectPublicKey_len, &dh_pubkey, &dh_pubkey_len,
                                   &server_key, &server_key_len);
        if (retval) {
            pkiDebug("failed to process/create dh paramters\n");
            goto cleanup;
        }
    }
    if ((rep9 != NULL &&
         rep9->choice == choice_pa_pk_as_rep_draft9_dhSignedData) ||
        (rep != NULL && rep->choice == choice_pa_pk_as_rep_dhInfo)) {

        /*
         * This is DH, so don't generate the key until after we
         * encode the reply, because the encoded reply is needed
         * to generate the key in some cases.
         */

        dhkey_info.subjectPublicKey.length = dh_pubkey_len;
        dhkey_info.subjectPublicKey.data = dh_pubkey;
        dhkey_info.nonce = request->nonce;
        dhkey_info.dhKeyExpiration = 0;

        retval = k5int_encode_krb5_kdc_dh_key_info(&dhkey_info,
                                                   &encoded_dhkey_info);
        if (retval) {
            pkiDebug("encode_krb5_kdc_dh_key_info failed\n");
            goto cleanup;
        }
#ifdef DEBUG_ASN1
        print_buffer_bin((unsigned char *)encoded_dhkey_info->data,
                         encoded_dhkey_info->length,
                         "/tmp/kdc_dh_key_info");
#endif

        switch ((int)padata->pa_type) {
        case KRB5_PADATA_PK_AS_REQ:
            retval = cms_signeddata_create(context, plgctx->cryptoctx,
                                           reqctx->cryptoctx, plgctx->idctx, CMS_SIGN_SERVER, 1,
                                           (unsigned char *)encoded_dhkey_info->data,
                                           encoded_dhkey_info->length,
                                           &rep->u.dh_Info.dhSignedData.data,
                                           &rep->u.dh_Info.dhSignedData.length);
            if (retval) {
                pkiDebug("failed to create pkcs7 signed data\n");
                goto cleanup;
            }
            break;
        case KRB5_PADATA_PK_AS_REP_OLD:
        case KRB5_PADATA_PK_AS_REQ_OLD:
            retval = cms_signeddata_create(context, plgctx->cryptoctx,
                                           reqctx->cryptoctx, plgctx->idctx, CMS_SIGN_DRAFT9, 1,
                                           (unsigned char *)encoded_dhkey_info->data,
                                           encoded_dhkey_info->length,
                                           &rep9->u.dhSignedData.data,
                                           &rep9->u.dhSignedData.length);
            if (retval) {
                pkiDebug("failed to create pkcs7 signed data\n");
                goto cleanup;
            }
            break;
        }

    } else {
        pkiDebug("received RSA key delivery AS REQ\n");

        retval = krb5_c_make_random_key(context, enctype, encrypting_key);
        if (retval) {
            pkiDebug("unable to make a session key\n");
            goto cleanup;
        }

        /* check if PA_TYPE of 132 is present which means the client is
         * requesting that a checksum is send back instead of the nonce
         */
        for (i = 0; request->padata[i] != NULL; i++) {
            pkiDebug("%s: Checking pa_type 0x%08x\n",
                     __FUNCTION__, request->padata[i]->pa_type);
            if (request->padata[i]->pa_type == 132)
                fixed_keypack = 1;
        }
        pkiDebug("%s: return checksum instead of nonce = %d\n",
                 __FUNCTION__, fixed_keypack);

        /* if this is an RFC reply or draft9 client requested a checksum
         * in the reply instead of the nonce, create an RFC-style keypack
         */
        if ((int)padata->pa_type == KRB5_PADATA_PK_AS_REQ || fixed_keypack) {
            init_krb5_reply_key_pack(&key_pack);
            if (key_pack == NULL) {
                retval = ENOMEM;
                goto cleanup;
            }

            retval = krb5_c_make_checksum(context, 0,
                                          encrypting_key, KRB5_KEYUSAGE_TGS_REQ_AUTH_CKSUM,
                                          req_pkt, &key_pack->asChecksum);
            if (retval) {
                pkiDebug("unable to calculate AS REQ checksum\n");
                goto cleanup;
            }
#ifdef DEBUG_CKSUM
            pkiDebug("calculating checksum on buf size = %d\n", req_pkt->length);
            print_buffer(req_pkt->data, req_pkt->length);
            pkiDebug("checksum size = %d\n", key_pack->asChecksum.length);
            print_buffer(key_pack->asChecksum.contents,
                         key_pack->asChecksum.length);
            pkiDebug("encrypting key (%d)\n", encrypting_key->length);
            print_buffer(encrypting_key->contents, encrypting_key->length);
#endif

            krb5_copy_keyblock_contents(context, encrypting_key,
                                        &key_pack->replyKey);

            retval = k5int_encode_krb5_reply_key_pack(key_pack,
                                                      &encoded_key_pack);
            if (retval) {
                pkiDebug("failed to encode reply_key_pack\n");
                goto cleanup;
            }
        }

        switch ((int)padata->pa_type) {
        case KRB5_PADATA_PK_AS_REQ:
            rep->choice = choice_pa_pk_as_rep_encKeyPack;
            retval = cms_envelopeddata_create(context, plgctx->cryptoctx,
                                              reqctx->cryptoctx, plgctx->idctx, padata->pa_type, 1,
                                              (unsigned char *)encoded_key_pack->data,
                                              encoded_key_pack->length,
                                              &rep->u.encKeyPack.data, &rep->u.encKeyPack.length);
            break;
        case KRB5_PADATA_PK_AS_REP_OLD:
        case KRB5_PADATA_PK_AS_REQ_OLD:
            /* if the request is from the broken draft9 client that
             * expects back a nonce, create it now
             */
            if (!fixed_keypack) {
                init_krb5_reply_key_pack_draft9(&key_pack9);
                if (key_pack9 == NULL) {
                    retval = ENOMEM;
                    goto cleanup;
                }
                key_pack9->nonce = reqctx->rcv_auth_pack9->pkAuthenticator.nonce;
                krb5_copy_keyblock_contents(context, encrypting_key,
                                            &key_pack9->replyKey);

                retval = k5int_encode_krb5_reply_key_pack_draft9(key_pack9,
                                                                 &encoded_key_pack);
                if (retval) {
                    pkiDebug("failed to encode reply_key_pack\n");
                    goto cleanup;
                }
            }

            rep9->choice = choice_pa_pk_as_rep_draft9_encKeyPack;
            retval = cms_envelopeddata_create(context, plgctx->cryptoctx,
                                              reqctx->cryptoctx, plgctx->idctx, padata->pa_type, 1,
                                              (unsigned char *)encoded_key_pack->data,
                                              encoded_key_pack->length,
                                              &rep9->u.encKeyPack.data, &rep9->u.encKeyPack.length);
            break;
        }
        if (retval) {
            pkiDebug("failed to create pkcs7 enveloped data: %s\n",
                     error_message(retval));
            goto cleanup;
        }
#ifdef DEBUG_ASN1
        print_buffer_bin((unsigned char *)encoded_key_pack->data,
                         encoded_key_pack->length,
                         "/tmp/kdc_key_pack");
        switch ((int)padata->pa_type) {
        case KRB5_PADATA_PK_AS_REQ:
            print_buffer_bin(rep->u.encKeyPack.data,
                             rep->u.encKeyPack.length,
                             "/tmp/kdc_enc_key_pack");
            break;
        case KRB5_PADATA_PK_AS_REP_OLD:
        case KRB5_PADATA_PK_AS_REQ_OLD:
            print_buffer_bin(rep9->u.encKeyPack.data,
                             rep9->u.encKeyPack.length,
                             "/tmp/kdc_enc_key_pack");
            break;
        }
#endif
    }

    if ((rep != NULL && rep->choice == choice_pa_pk_as_rep_dhInfo) &&
        ((reqctx->rcv_auth_pack != NULL &&
          reqctx->rcv_auth_pack->supportedKDFs != NULL))) {

        /* If using the alg-agility KDF, put the algorithm in the reply
         * before encoding it.
         */
        if (reqctx->rcv_auth_pack != NULL &&
            reqctx->rcv_auth_pack->supportedKDFs != NULL) {
            retval = pkinit_pick_kdf_alg(context, reqctx->rcv_auth_pack->supportedKDFs,
                                         &(rep->u.dh_Info.kdfID));
            if (retval) {
                pkiDebug("pkinit_pick_kdf_alg failed: %s\n",
                         error_message(retval));
                goto cleanup;
            }
        }
    }

    switch ((int)padata->pa_type) {
    case KRB5_PADATA_PK_AS_REQ:
        retval = k5int_encode_krb5_pa_pk_as_rep(rep, &out_data);
        break;
    case KRB5_PADATA_PK_AS_REP_OLD:
    case KRB5_PADATA_PK_AS_REQ_OLD:
        retval = k5int_encode_krb5_pa_pk_as_rep_draft9(rep9, &out_data);
        break;
    }
    if (retval) {
        pkiDebug("failed to encode AS_REP\n");
        goto cleanup;
    }
#ifdef DEBUG_ASN1
    if (out_data != NULL)
        print_buffer_bin((unsigned char *)out_data->data, out_data->length,
                         "/tmp/kdc_as_rep");
#endif

    /* If this is DH, we haven't computed the key yet, so do it now. */
    if ((rep9 != NULL &&
          rep9->choice == choice_pa_pk_as_rep_draft9_dhSignedData) ||
         (rep != NULL && rep->choice == choice_pa_pk_as_rep_dhInfo)) {
 
        /* If we're not doing draft 9, and mutually supported KDFs were found,
         * use the algorithm agility KDF. */
        if (rep != NULL && rep->u.dh_Info.kdfID) {
            secret.data = (char *)server_key;
             secret.length = server_key_len;
 
             retval = pkinit_alg_agility_kdf(context, &secret,
                                            rep->u.dh_Info.kdfID,
                                            request->client, request->server,
                                            enctype,
                                            (krb5_octet_data *)req_pkt,
                                            (krb5_octet_data *)out_data,
                                            encrypting_key);
            if (retval) {
                pkiDebug("pkinit_alg_agility_kdf failed: %s\n",
                         error_message(retval));
                goto cleanup;
            }

            /* Otherwise, use the older octetstring2key() function */
        } else {
            retval = pkinit_octetstring2key(context, enctype, server_key,
                                            server_key_len, encrypting_key);
            if (retval) {
                pkiDebug("pkinit_octetstring2key failed: %s\n",
                         error_message(retval));
                goto cleanup;
            }
        }
    }

    *send_pa = malloc(sizeof(krb5_pa_data));
    if (*send_pa == NULL) {
        retval = ENOMEM;
        free(out_data->data);
        free(out_data);
        out_data = NULL;
        goto cleanup;
    }
    (*send_pa)->magic = KV5M_PA_DATA;
    switch ((int)padata->pa_type) {
    case KRB5_PADATA_PK_AS_REQ:
        (*send_pa)->pa_type = KRB5_PADATA_PK_AS_REP;
        break;
    case KRB5_PADATA_PK_AS_REQ_OLD:
    case KRB5_PADATA_PK_AS_REP_OLD:
        (*send_pa)->pa_type = KRB5_PADATA_PK_AS_REP_OLD;
        break;
    }
    (*send_pa)->length = out_data->length;
    (*send_pa)->contents = (krb5_octet *) out_data->data;

cleanup:
    pkinit_fini_kdc_req_context(context, reqctx);
    free(scratch.data);
    free(out_data);
    if (encoded_dhkey_info != NULL)
        krb5_free_data(context, encoded_dhkey_info);
    if (encoded_key_pack != NULL)
        krb5_free_data(context, encoded_key_pack);
    free(dh_pubkey);
    free(server_key);

    switch ((int)padata->pa_type) {
    case KRB5_PADATA_PK_AS_REQ:
        free_krb5_pa_pk_as_req(&reqp);
        free_krb5_pa_pk_as_rep(&rep);
        free_krb5_reply_key_pack(&key_pack);
        break;
    case KRB5_PADATA_PK_AS_REP_OLD:
    case KRB5_PADATA_PK_AS_REQ_OLD:
        free_krb5_pa_pk_as_req_draft9(&reqp9);
        free_krb5_pa_pk_as_rep_draft9(&rep9);
        if (!fixed_keypack)
            free_krb5_reply_key_pack_draft9(&key_pack9);
        else
            free_krb5_reply_key_pack(&key_pack);
        break;
    }

    if (retval)
        pkiDebug("pkinit_verify_padata failure");

    return retval;
}
