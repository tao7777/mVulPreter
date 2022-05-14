int tls1_change_cipher_state(SSL *s, int which)
{
    unsigned char *p, *mac_secret;
    unsigned char tmp1[EVP_MAX_KEY_LENGTH];
    unsigned char tmp2[EVP_MAX_KEY_LENGTH];
    unsigned char iv1[EVP_MAX_IV_LENGTH * 2];
    unsigned char iv2[EVP_MAX_IV_LENGTH * 2];
    unsigned char *ms, *key, *iv;
    EVP_CIPHER_CTX *dd;
    const EVP_CIPHER *c;
#ifndef OPENSSL_NO_COMP
    const SSL_COMP *comp;
#endif
    const EVP_MD *m;
    int mac_type;
    int *mac_secret_size;
    EVP_MD_CTX *mac_ctx;
    EVP_PKEY *mac_key;
    int n, i, j, k, cl;
    int reuse_dd = 0;

    c = s->s3->tmp.new_sym_enc;
    m = s->s3->tmp.new_hash;
    mac_type = s->s3->tmp.new_mac_pkey_type;
#ifndef OPENSSL_NO_COMP
    comp = s->s3->tmp.new_compression;
 #endif
 
     if (which & SSL3_CC_READ) {
         if (s->s3->tmp.new_cipher->algorithm2 & TLS1_STREAM_MAC)
             s->mac_flags |= SSL_MAC_FLAG_READ_MAC_STREAM;
         else
            s->mac_flags &= ~SSL_MAC_FLAG_READ_MAC_STREAM;

        if (s->enc_read_ctx != NULL)
            reuse_dd = 1;
        else if ((s->enc_read_ctx = EVP_CIPHER_CTX_new()) == NULL)
            goto err;
        else
            /*
             * make sure it's initialised in case we exit later with an error
             */
            EVP_CIPHER_CTX_reset(s->enc_read_ctx);
        dd = s->enc_read_ctx;
        mac_ctx = ssl_replace_hash(&s->read_hash, NULL);
        if (mac_ctx == NULL)
            goto err;
#ifndef OPENSSL_NO_COMP
        COMP_CTX_free(s->expand);
        s->expand = NULL;
        if (comp != NULL) {
            s->expand = COMP_CTX_new(comp->method);
            if (s->expand == NULL) {
                SSLerr(SSL_F_TLS1_CHANGE_CIPHER_STATE,
                       SSL_R_COMPRESSION_LIBRARY_ERROR);
                goto err2;
            }
        }
#endif
        /*
         * this is done by dtls1_reset_seq_numbers for DTLS
         */
        if (!SSL_IS_DTLS(s))
            RECORD_LAYER_reset_read_sequence(&s->rlayer);
         mac_secret = &(s->s3->read_mac_secret[0]);
         mac_secret_size = &(s->s3->read_mac_secret_size);
     } else {
         if (s->s3->tmp.new_cipher->algorithm2 & TLS1_STREAM_MAC)
             s->mac_flags |= SSL_MAC_FLAG_WRITE_MAC_STREAM;
         else
            s->mac_flags &= ~SSL_MAC_FLAG_WRITE_MAC_STREAM;
        if (s->enc_write_ctx != NULL && !SSL_IS_DTLS(s))
            reuse_dd = 1;
        else if ((s->enc_write_ctx = EVP_CIPHER_CTX_new()) == NULL)
            goto err;
        dd = s->enc_write_ctx;
        if (SSL_IS_DTLS(s)) {
            mac_ctx = EVP_MD_CTX_new();
            if (mac_ctx == NULL)
                goto err;
            s->write_hash = mac_ctx;
        } else {
            mac_ctx = ssl_replace_hash(&s->write_hash, NULL);
            if (mac_ctx == NULL)
                goto err;
        }
#ifndef OPENSSL_NO_COMP
        COMP_CTX_free(s->compress);
        s->compress = NULL;
        if (comp != NULL) {
            s->compress = COMP_CTX_new(comp->method);
            if (s->compress == NULL) {
                SSLerr(SSL_F_TLS1_CHANGE_CIPHER_STATE,
                       SSL_R_COMPRESSION_LIBRARY_ERROR);
                goto err2;
            }
        }
#endif
        /*
         * this is done by dtls1_reset_seq_numbers for DTLS
         */
        if (!SSL_IS_DTLS(s))
            RECORD_LAYER_reset_write_sequence(&s->rlayer);
        mac_secret = &(s->s3->write_mac_secret[0]);
        mac_secret_size = &(s->s3->write_mac_secret_size);
    }

    if (reuse_dd)
        EVP_CIPHER_CTX_reset(dd);

    p = s->s3->tmp.key_block;
    i = *mac_secret_size = s->s3->tmp.new_mac_secret_size;

    cl = EVP_CIPHER_key_length(c);
    j = cl;
    /* Was j=(exp)?5:EVP_CIPHER_key_length(c); */
    /* If GCM/CCM mode only part of IV comes from PRF */
    if (EVP_CIPHER_mode(c) == EVP_CIPH_GCM_MODE)
        k = EVP_GCM_TLS_FIXED_IV_LEN;
    else if (EVP_CIPHER_mode(c) == EVP_CIPH_CCM_MODE)
        k = EVP_CCM_TLS_FIXED_IV_LEN;
    else
        k = EVP_CIPHER_iv_length(c);
    if ((which == SSL3_CHANGE_CIPHER_CLIENT_WRITE) ||
        (which == SSL3_CHANGE_CIPHER_SERVER_READ)) {
        ms = &(p[0]);
        n = i + i;
        key = &(p[n]);
        n += j + j;
        iv = &(p[n]);
        n += k + k;
    } else {
        n = i;
        ms = &(p[n]);
        n += i + j;
        key = &(p[n]);
        n += j + k;
        iv = &(p[n]);
        n += k;
    }

    if (n > s->s3->tmp.key_block_length) {
        SSLerr(SSL_F_TLS1_CHANGE_CIPHER_STATE, ERR_R_INTERNAL_ERROR);
        goto err2;
    }

    memcpy(mac_secret, ms, i);

    if (!(EVP_CIPHER_flags(c) & EVP_CIPH_FLAG_AEAD_CIPHER)) {
        mac_key = EVP_PKEY_new_mac_key(mac_type, NULL,
                                       mac_secret, *mac_secret_size);
        if (mac_key == NULL
            || EVP_DigestSignInit(mac_ctx, NULL, m, NULL, mac_key) <= 0) {
            EVP_PKEY_free(mac_key);
            SSLerr(SSL_F_TLS1_CHANGE_CIPHER_STATE, ERR_R_INTERNAL_ERROR);
            goto err2;
        }
        EVP_PKEY_free(mac_key);
    }
#ifdef SSL_DEBUG
    printf("which = %04X\nmac key=", which);
    {
        int z;
        for (z = 0; z < i; z++)
            printf("%02X%c", ms[z], ((z + 1) % 16) ? ' ' : '\n');
    }
#endif

    if (EVP_CIPHER_mode(c) == EVP_CIPH_GCM_MODE) {
        if (!EVP_CipherInit_ex(dd, c, NULL, key, NULL, (which & SSL3_CC_WRITE))
            || !EVP_CIPHER_CTX_ctrl(dd, EVP_CTRL_GCM_SET_IV_FIXED, k, iv)) {
            SSLerr(SSL_F_TLS1_CHANGE_CIPHER_STATE, ERR_R_INTERNAL_ERROR);
            goto err2;
        }
    } else if (EVP_CIPHER_mode(c) == EVP_CIPH_CCM_MODE) {
        int taglen;
        if (s->s3->tmp.
            new_cipher->algorithm_enc & (SSL_AES128CCM8 | SSL_AES256CCM8))
            taglen = 8;
        else
            taglen = 16;
        if (!EVP_CipherInit_ex(dd, c, NULL, NULL, NULL, (which & SSL3_CC_WRITE))
            || !EVP_CIPHER_CTX_ctrl(dd, EVP_CTRL_AEAD_SET_IVLEN, 12, NULL)
            || !EVP_CIPHER_CTX_ctrl(dd, EVP_CTRL_AEAD_SET_TAG, taglen, NULL)
            || !EVP_CIPHER_CTX_ctrl(dd, EVP_CTRL_CCM_SET_IV_FIXED, k, iv)
            || !EVP_CipherInit_ex(dd, NULL, NULL, key, NULL, -1)) {
            SSLerr(SSL_F_TLS1_CHANGE_CIPHER_STATE, ERR_R_INTERNAL_ERROR);
            goto err2;
        }
    } else {
        if (!EVP_CipherInit_ex(dd, c, NULL, key, iv, (which & SSL3_CC_WRITE))) {
            SSLerr(SSL_F_TLS1_CHANGE_CIPHER_STATE, ERR_R_INTERNAL_ERROR);
            goto err2;
        }
    }
    /* Needed for "composite" AEADs, such as RC4-HMAC-MD5 */
    if ((EVP_CIPHER_flags(c) & EVP_CIPH_FLAG_AEAD_CIPHER) && *mac_secret_size
        && !EVP_CIPHER_CTX_ctrl(dd, EVP_CTRL_AEAD_SET_MAC_KEY,
                                *mac_secret_size, mac_secret)) {
        SSLerr(SSL_F_TLS1_CHANGE_CIPHER_STATE, ERR_R_INTERNAL_ERROR);
        goto err2;
    }
#ifdef OPENSSL_SSL_TRACE_CRYPTO
    if (s->msg_callback) {
        int wh = which & SSL3_CC_WRITE ? TLS1_RT_CRYPTO_WRITE : 0;
        if (*mac_secret_size)
            s->msg_callback(2, s->version, wh | TLS1_RT_CRYPTO_MAC,
                            mac_secret, *mac_secret_size,
                            s, s->msg_callback_arg);
        if (c->key_len)
            s->msg_callback(2, s->version, wh | TLS1_RT_CRYPTO_KEY,
                            key, c->key_len, s, s->msg_callback_arg);
        if (k) {
            if (EVP_CIPHER_mode(c) == EVP_CIPH_GCM_MODE)
                wh |= TLS1_RT_CRYPTO_FIXED_IV;
            else
                wh |= TLS1_RT_CRYPTO_IV;
            s->msg_callback(2, s->version, wh, iv, k, s, s->msg_callback_arg);
        }
    }
#endif

#ifdef SSL_DEBUG
    printf("which = %04X\nkey=", which);
    {
        int z;
        for (z = 0; z < EVP_CIPHER_key_length(c); z++)
            printf("%02X%c", key[z], ((z + 1) % 16) ? ' ' : '\n');
    }
    printf("\niv=");
    {
        int z;
        for (z = 0; z < k; z++)
            printf("%02X%c", iv[z], ((z + 1) % 16) ? ' ' : '\n');
    }
    printf("\n");
#endif

    OPENSSL_cleanse(tmp1, sizeof(tmp1));
    OPENSSL_cleanse(tmp2, sizeof(tmp1));
    OPENSSL_cleanse(iv1, sizeof(iv1));
    OPENSSL_cleanse(iv2, sizeof(iv2));
    return (1);
 err:
    SSLerr(SSL_F_TLS1_CHANGE_CIPHER_STATE, ERR_R_MALLOC_FAILURE);
 err2:
    OPENSSL_cleanse(tmp1, sizeof(tmp1));
    OPENSSL_cleanse(tmp2, sizeof(tmp1));
    OPENSSL_cleanse(iv1, sizeof(iv1));
    OPENSSL_cleanse(iv2, sizeof(iv2));
    return (0);
}
