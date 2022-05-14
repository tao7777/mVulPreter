int tls1_enc(SSL *s, SSL3_RECORD *recs, unsigned int n_recs, int send)
{
    EVP_CIPHER_CTX *ds;
    size_t reclen[SSL_MAX_PIPELINES];
    unsigned char buf[SSL_MAX_PIPELINES][EVP_AEAD_TLS1_AAD_LEN];
    int bs, i, j, k, pad = 0, ret, mac_size = 0;
    const EVP_CIPHER *enc;
    unsigned int ctr;

    if (send) {
        if (EVP_MD_CTX_md(s->write_hash)) {
            int n = EVP_MD_CTX_size(s->write_hash);
            OPENSSL_assert(n >= 0);
        }
        ds = s->enc_write_ctx;
        if (s->enc_write_ctx == NULL)
            enc = NULL;
        else {
            int ivlen;
            enc = EVP_CIPHER_CTX_cipher(s->enc_write_ctx);
            /* For TLSv1.1 and later explicit IV */
            if (SSL_USE_EXPLICIT_IV(s)
                && EVP_CIPHER_mode(enc) == EVP_CIPH_CBC_MODE)
                ivlen = EVP_CIPHER_iv_length(enc);
            else
                ivlen = 0;
            if (ivlen > 1) {
                for (ctr = 0; ctr < n_recs; ctr++) {
                    if (recs[ctr].data != recs[ctr].input) {
                        /*
                         * we can't write into the input stream: Can this ever
                         * happen?? (steve)
                         */
                        SSLerr(SSL_F_TLS1_ENC, ERR_R_INTERNAL_ERROR);
                        return -1;
                    } else if (RAND_bytes(recs[ctr].input, ivlen) <= 0) {
                        SSLerr(SSL_F_TLS1_ENC, ERR_R_INTERNAL_ERROR);
                        return -1;
                    }
                }
            }
        }
    } else {
        if (EVP_MD_CTX_md(s->read_hash)) {
            int n = EVP_MD_CTX_size(s->read_hash);
            OPENSSL_assert(n >= 0);
        }
        ds = s->enc_read_ctx;
        if (s->enc_read_ctx == NULL)
            enc = NULL;
        else
            enc = EVP_CIPHER_CTX_cipher(s->enc_read_ctx);
    }

    if ((s->session == NULL) || (ds == NULL) || (enc == NULL)) {
        for (ctr = 0; ctr < n_recs; ctr++) {
            memmove(recs[ctr].data, recs[ctr].input, recs[ctr].length);
            recs[ctr].input = recs[ctr].data;
        }
        ret = 1;
    } else {
        bs = EVP_CIPHER_block_size(EVP_CIPHER_CTX_cipher(ds));

        if (n_recs > 1) {
            if (!(EVP_CIPHER_flags(EVP_CIPHER_CTX_cipher(ds))
                  & EVP_CIPH_FLAG_PIPELINE)) {
                /*
                 * We shouldn't have been called with pipeline data if the
                 * cipher doesn't support pipelining
                 */
                SSLerr(SSL_F_TLS1_ENC, SSL_R_PIPELINE_FAILURE);
                return -1;
            }
        }
        for (ctr = 0; ctr < n_recs; ctr++) {
            reclen[ctr] = recs[ctr].length;

            if (EVP_CIPHER_flags(EVP_CIPHER_CTX_cipher(ds))
                & EVP_CIPH_FLAG_AEAD_CIPHER) {
                unsigned char *seq;

                seq = send ? RECORD_LAYER_get_write_sequence(&s->rlayer)
                    : RECORD_LAYER_get_read_sequence(&s->rlayer);

                if (SSL_IS_DTLS(s)) {
                    /* DTLS does not support pipelining */
                    unsigned char dtlsseq[9], *p = dtlsseq;

                    s2n(send ? DTLS_RECORD_LAYER_get_w_epoch(&s->rlayer) :
                        DTLS_RECORD_LAYER_get_r_epoch(&s->rlayer), p);
                    memcpy(p, &seq[2], 6);
                    memcpy(buf[ctr], dtlsseq, 8);
                } else {
                    memcpy(buf[ctr], seq, 8);
                    for (i = 7; i >= 0; i--) { /* increment */
                        ++seq[i];
                        if (seq[i] != 0)
                            break;
                    }
                }

                buf[ctr][8] = recs[ctr].type;
                buf[ctr][9] = (unsigned char)(s->version >> 8);
                buf[ctr][10] = (unsigned char)(s->version);
                buf[ctr][11] = recs[ctr].length >> 8;
                buf[ctr][12] = recs[ctr].length & 0xff;
                pad = EVP_CIPHER_CTX_ctrl(ds, EVP_CTRL_AEAD_TLS1_AAD,
                                          EVP_AEAD_TLS1_AAD_LEN, buf[ctr]);
                if (pad <= 0)
                    return -1;

                if (send) {
                    reclen[ctr] += pad;
                    recs[ctr].length += pad;
                }

            } else if ((bs != 1) && send) {
                i = bs - ((int)reclen[ctr] % bs);

                /* Add weird padding of upto 256 bytes */

                /* we need to add 'i' padding bytes of value j */
                j = i - 1;
                for (k = (int)reclen[ctr]; k < (int)(reclen[ctr] + i); k++)
                    recs[ctr].input[k] = j;
                reclen[ctr] += i;
                recs[ctr].length += i;
            }

            if (!send) {
                if (reclen[ctr] == 0 || reclen[ctr] % bs != 0)
                    return 0;
            }
        }
        if (n_recs > 1) {
            unsigned char *data[SSL_MAX_PIPELINES];

            /* Set the output buffers */
            for (ctr = 0; ctr < n_recs; ctr++) {
                data[ctr] = recs[ctr].data;
            }
            if (EVP_CIPHER_CTX_ctrl(ds, EVP_CTRL_SET_PIPELINE_OUTPUT_BUFS,
                                    n_recs, data) <= 0) {
                SSLerr(SSL_F_TLS1_ENC, SSL_R_PIPELINE_FAILURE);
            }
            /* Set the input buffers */
            for (ctr = 0; ctr < n_recs; ctr++) {
                data[ctr] = recs[ctr].input;
            }
            if (EVP_CIPHER_CTX_ctrl(ds, EVP_CTRL_SET_PIPELINE_INPUT_BUFS,
                                    n_recs, data) <= 0
                || EVP_CIPHER_CTX_ctrl(ds, EVP_CTRL_SET_PIPELINE_INPUT_LENS,
                                       n_recs, reclen) <= 0) {
                SSLerr(SSL_F_TLS1_ENC, SSL_R_PIPELINE_FAILURE);
                return -1;
            }
        }

        i = EVP_Cipher(ds, recs[0].data, recs[0].input, reclen[0]);
        if ((EVP_CIPHER_flags(EVP_CIPHER_CTX_cipher(ds))
             & EVP_CIPH_FLAG_CUSTOM_CIPHER)
            ? (i < 0)
            : (i == 0))
            return -1;          /* AEAD can fail to verify MAC */
        if (send == 0) {
            if (EVP_CIPHER_mode(enc) == EVP_CIPH_GCM_MODE) {
                for (ctr = 0; ctr < n_recs; ctr++) {
                    recs[ctr].data += EVP_GCM_TLS_EXPLICIT_IV_LEN;
                    recs[ctr].input += EVP_GCM_TLS_EXPLICIT_IV_LEN;
                    recs[ctr].length -= EVP_GCM_TLS_EXPLICIT_IV_LEN;
                }
            } else if (EVP_CIPHER_mode(enc) == EVP_CIPH_CCM_MODE) {
                for (ctr = 0; ctr < n_recs; ctr++) {
                    recs[ctr].data += EVP_CCM_TLS_EXPLICIT_IV_LEN;
                    recs[ctr].input += EVP_CCM_TLS_EXPLICIT_IV_LEN;
                    recs[ctr].length -= EVP_CCM_TLS_EXPLICIT_IV_LEN;
                }
            }
         }
 
         ret = 1;
        if (!SSL_USE_ETM(s) && EVP_MD_CTX_md(s->read_hash) != NULL)
             mac_size = EVP_MD_CTX_size(s->read_hash);
         if ((bs != 1) && !send) {
             int tmpret;
            for (ctr = 0; ctr < n_recs; ctr++) {
                tmpret = tls1_cbc_remove_padding(s, &recs[ctr], bs, mac_size);
                /*
                 * If tmpret == 0 then this means publicly invalid so we can
                 * short circuit things here. Otherwise we must respect constant
                 * time behaviour.
                 */
                if (tmpret == 0)
                    return 0;
                ret = constant_time_select_int(constant_time_eq_int(tmpret, 1),
                                               ret, -1);
            }
        }
        if (pad && !send) {
            for (ctr = 0; ctr < n_recs; ctr++) {
                recs[ctr].length -= pad;
            }
        }
    }
    return ret;
}
