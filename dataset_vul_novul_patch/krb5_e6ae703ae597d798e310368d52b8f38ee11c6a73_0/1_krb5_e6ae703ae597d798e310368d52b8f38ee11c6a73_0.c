kg_unseal_v1_iov(krb5_context context,
                 OM_uint32 *minor_status,
                 krb5_gss_ctx_id_rec *ctx,
                 gss_iov_buffer_desc *iov,
                 int iov_count,
                 size_t token_wrapper_len,
                 int *conf_state,
                 gss_qop_t *qop_state,
                 int toktype)
{
    OM_uint32 code;
    gss_iov_buffer_t header;
    gss_iov_buffer_t trailer;
    unsigned char *ptr;
    int sealalg;
    int signalg;
    krb5_checksum cksum;
    krb5_checksum md5cksum;
    size_t cksum_len = 0;
    size_t conflen = 0;
    int direction;
    krb5_ui_4 seqnum;
    OM_uint32 retval;
    size_t sumlen;
    krb5_keyusage sign_usage = KG_USAGE_SIGN;

    md5cksum.length = cksum.length = 0;
    md5cksum.contents = cksum.contents = NULL;

    header = kg_locate_header_iov(iov, iov_count, toktype);
    assert(header != NULL);

    trailer = kg_locate_iov(iov, iov_count, GSS_IOV_BUFFER_TYPE_TRAILER);
    if (trailer != NULL && trailer->buffer.length != 0) {
        *minor_status = (OM_uint32)KRB5_BAD_MSIZE;
         return GSS_S_DEFECTIVE_TOKEN;
     }
 
    if (header->buffer.length < token_wrapper_len + 14) {
         *minor_status = 0;
         return GSS_S_DEFECTIVE_TOKEN;
     }

    ptr = (unsigned char *)header->buffer.value + token_wrapper_len;

    signalg  = ptr[0];
    signalg |= ptr[1] << 8;

    sealalg  = ptr[2];
    sealalg |= ptr[3] << 8;

    if (ptr[4] != 0xFF || ptr[5] != 0xFF) {
        *minor_status = 0;
        return GSS_S_DEFECTIVE_TOKEN;
    }

    if (toktype != KG_TOK_WRAP_MSG && sealalg != 0xFFFF) {
        *minor_status = 0;
        return GSS_S_DEFECTIVE_TOKEN;
    }

    if (toktype == KG_TOK_WRAP_MSG &&
        !(sealalg == 0xFFFF || sealalg == ctx->sealalg)) {
        *minor_status = 0;
        return GSS_S_DEFECTIVE_TOKEN;
    }

    if ((ctx->sealalg == SEAL_ALG_NONE && signalg > 1) ||
        (ctx->sealalg == SEAL_ALG_1 && signalg != SGN_ALG_3) ||
        (ctx->sealalg == SEAL_ALG_DES3KD &&
         signalg != SGN_ALG_HMAC_SHA1_DES3_KD)||
        (ctx->sealalg == SEAL_ALG_MICROSOFT_RC4 &&
         signalg != SGN_ALG_HMAC_MD5)) {
        *minor_status = 0;
        return GSS_S_DEFECTIVE_TOKEN;
    }

    switch (signalg) {
    case SGN_ALG_DES_MAC_MD5:
    case SGN_ALG_MD2_5:
    case SGN_ALG_HMAC_MD5:
        cksum_len = 8;
        if (toktype != KG_TOK_WRAP_MSG)
            sign_usage = 15;
        break;
    case SGN_ALG_3:
        cksum_len = 16;
        break;
    case SGN_ALG_HMAC_SHA1_DES3_KD:
        cksum_len = 20;
        break;
    default:
        *minor_status = 0;
        return GSS_S_DEFECTIVE_TOKEN;
    }

    /* get the token parameters */
    code = kg_get_seq_num(context, ctx->seq, ptr + 14, ptr + 6, &direction,
                          &seqnum);
    if (code != 0) {
        *minor_status = code;
        return GSS_S_BAD_SIG;
    }

    /* decode the message, if SEAL */
    if (toktype == KG_TOK_WRAP_MSG) {
        if (sealalg != 0xFFFF) {
            if (ctx->sealalg == SEAL_ALG_MICROSOFT_RC4) {
                unsigned char bigend_seqnum[4];
                krb5_keyblock *enc_key;
                size_t i;

                store_32_be(seqnum, bigend_seqnum);

                code = krb5_k_key_keyblock(context, ctx->enc, &enc_key);
                if (code != 0) {
                    retval = GSS_S_FAILURE;
                    goto cleanup;
                }

                assert(enc_key->length == 16);

                for (i = 0; i < enc_key->length; i++)
                    ((char *)enc_key->contents)[i] ^= 0xF0;

                code = kg_arcfour_docrypt_iov(context, enc_key, 0,
                                              &bigend_seqnum[0], 4,
                                              iov, iov_count);
                krb5_free_keyblock(context, enc_key);
            } else {
                code = kg_decrypt_iov(context, 0,
                                      ((ctx->gss_flags & GSS_C_DCE_STYLE) != 0),
                                      0 /*EC*/, 0 /*RRC*/,
                                      ctx->enc, KG_USAGE_SEAL, NULL,
                                      iov, iov_count);
            }
            if (code != 0) {
                retval = GSS_S_FAILURE;
                goto cleanup;
            }
        }
        conflen = kg_confounder_size(context, ctx->enc->keyblock.enctype);
    }

    if (header->buffer.length != token_wrapper_len + 14 + cksum_len + conflen) {
        retval = GSS_S_DEFECTIVE_TOKEN;
        goto cleanup;
    }

    /* compute the checksum of the message */

    /* initialize the checksum */

    switch (signalg) {
    case SGN_ALG_DES_MAC_MD5:
    case SGN_ALG_MD2_5:
    case SGN_ALG_DES_MAC:
    case SGN_ALG_3:
        md5cksum.checksum_type = CKSUMTYPE_RSA_MD5;
        break;
    case SGN_ALG_HMAC_MD5:
        md5cksum.checksum_type = CKSUMTYPE_HMAC_MD5_ARCFOUR;
        break;
    case SGN_ALG_HMAC_SHA1_DES3_KD:
        md5cksum.checksum_type = CKSUMTYPE_HMAC_SHA1_DES3;
        break;
    default:
        abort();
    }

    code = krb5_c_checksum_length(context, md5cksum.checksum_type, &sumlen);
    if (code != 0) {
        retval = GSS_S_FAILURE;
        goto cleanup;
    }
    md5cksum.length = sumlen;

    /* compute the checksum of the message */
    code = kg_make_checksum_iov_v1(context, md5cksum.checksum_type,
                                   cksum_len, ctx->seq, ctx->enc,
                                   sign_usage, iov, iov_count, toktype,
                                   &md5cksum);
    if (code != 0) {
        retval = GSS_S_FAILURE;
        goto cleanup;
    }

    switch (signalg) {
    case SGN_ALG_DES_MAC_MD5:
    case SGN_ALG_3:
        code = kg_encrypt_inplace(context, ctx->seq, KG_USAGE_SEAL,
                                  (g_OID_equal(ctx->mech_used,
                                               gss_mech_krb5_old) ?
                                   ctx->seq->keyblock.contents : NULL),
                                  md5cksum.contents, 16);
        if (code != 0) {
            retval = GSS_S_FAILURE;
            goto cleanup;
        }

        cksum.length = cksum_len;
        cksum.contents = md5cksum.contents + 16 - cksum.length;

        code = k5_bcmp(cksum.contents, ptr + 14, cksum.length);
        break;
    case SGN_ALG_HMAC_SHA1_DES3_KD:
    case SGN_ALG_HMAC_MD5:
        code = k5_bcmp(md5cksum.contents, ptr + 14, cksum_len);
        break;
    default:
        code = 0;
        retval = GSS_S_DEFECTIVE_TOKEN;
        goto cleanup;
        break;
    }

    if (code != 0) {
        code = 0;
        retval = GSS_S_BAD_SIG;
        goto cleanup;
    }

    /*
     * For GSS_C_DCE_STYLE, the caller manages the padding, because the
     * pad length is in the RPC PDU. The value of the padding may be
     * uninitialized. For normal GSS, the last bytes of the decrypted
     * data contain the pad length. kg_fixup_padding_iov() will find
     * this and fixup the last data IOV appropriately.
     */
    if (toktype == KG_TOK_WRAP_MSG &&
        (ctx->gss_flags & GSS_C_DCE_STYLE) == 0) {
        retval = kg_fixup_padding_iov(&code, iov, iov_count);
        if (retval != GSS_S_COMPLETE)
            goto cleanup;
    }

    if (conf_state != NULL)
        *conf_state = (sealalg != 0xFFFF);

    if (qop_state != NULL)
        *qop_state = GSS_C_QOP_DEFAULT;

    if ((ctx->initiate && direction != 0xff) ||
        (!ctx->initiate && direction != 0)) {
        *minor_status = (OM_uint32)G_BAD_DIRECTION;
        retval = GSS_S_BAD_SIG;
    }

    code = 0;
    retval = g_order_check(&ctx->seqstate, (gssint_uint64)seqnum);

cleanup:
    krb5_free_checksum_contents(context, &md5cksum);

    *minor_status = code;

    return retval;
}
