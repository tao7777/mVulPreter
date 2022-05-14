static int get_client_master_key(SSL *s)
{
    int is_export, i, n, keya, ek;
    unsigned long len;
    unsigned char *p;
    const SSL_CIPHER *cp;
    const EVP_CIPHER *c;
    const EVP_MD *md;

    p = (unsigned char *)s->init_buf->data;
    if (s->state == SSL2_ST_GET_CLIENT_MASTER_KEY_A) {
        i = ssl2_read(s, (char *)&(p[s->init_num]), 10 - s->init_num);

        if (i < (10 - s->init_num))
            return (ssl2_part_read(s, SSL_F_GET_CLIENT_MASTER_KEY, i));
        s->init_num = 10;

        if (*(p++) != SSL2_MT_CLIENT_MASTER_KEY) {
            if (p[-1] != SSL2_MT_ERROR) {
                ssl2_return_error(s, SSL2_PE_UNDEFINED_ERROR);
                SSLerr(SSL_F_GET_CLIENT_MASTER_KEY,
                       SSL_R_READ_WRONG_PACKET_TYPE);
            } else
                SSLerr(SSL_F_GET_CLIENT_MASTER_KEY, SSL_R_PEER_ERROR);
            return (-1);
        }

        cp = ssl2_get_cipher_by_char(p);
        if (cp == NULL) {
            ssl2_return_error(s, SSL2_PE_NO_CIPHER);
            SSLerr(SSL_F_GET_CLIENT_MASTER_KEY, SSL_R_NO_CIPHER_MATCH);
            return (-1);
        }
        s->session->cipher = cp;

        p += 3;
        n2s(p, i);
        s->s2->tmp.clear = i;
        n2s(p, i);
        s->s2->tmp.enc = i;
        n2s(p, i);
        if (i > SSL_MAX_KEY_ARG_LENGTH) {
            ssl2_return_error(s, SSL2_PE_UNDEFINED_ERROR);
            SSLerr(SSL_F_GET_CLIENT_MASTER_KEY, SSL_R_KEY_ARG_TOO_LONG);
            return -1;
        }
        s->session->key_arg_length = i;
        s->state = SSL2_ST_GET_CLIENT_MASTER_KEY_B;
    }

    /* SSL2_ST_GET_CLIENT_MASTER_KEY_B */
    p = (unsigned char *)s->init_buf->data;
    if (s->init_buf->length < SSL2_MAX_RECORD_LENGTH_3_BYTE_HEADER) {
        ssl2_return_error(s, SSL2_PE_UNDEFINED_ERROR);
        SSLerr(SSL_F_GET_CLIENT_MASTER_KEY, ERR_R_INTERNAL_ERROR);
        return -1;
    }
    keya = s->session->key_arg_length;
    len =
        10 + (unsigned long)s->s2->tmp.clear + (unsigned long)s->s2->tmp.enc +
        (unsigned long)keya;
    if (len > SSL2_MAX_RECORD_LENGTH_3_BYTE_HEADER) {
        ssl2_return_error(s, SSL2_PE_UNDEFINED_ERROR);
        SSLerr(SSL_F_GET_CLIENT_MASTER_KEY, SSL_R_MESSAGE_TOO_LONG);
        return -1;
    }
    n = (int)len - s->init_num;
    i = ssl2_read(s, (char *)&(p[s->init_num]), n);
    if (i != n)
        return (ssl2_part_read(s, SSL_F_GET_CLIENT_MASTER_KEY, i));
    if (s->msg_callback) {
        /* CLIENT-MASTER-KEY */
        s->msg_callback(0, s->version, 0, p, (size_t)len, s,
                        s->msg_callback_arg);
    }
    p += 10;

    memcpy(s->session->key_arg, &(p[s->s2->tmp.clear + s->s2->tmp.enc]),
           (unsigned int)keya);

    if (s->cert->pkeys[SSL_PKEY_RSA_ENC].privatekey == NULL) {
        ssl2_return_error(s, SSL2_PE_UNDEFINED_ERROR);
         SSLerr(SSL_F_GET_CLIENT_MASTER_KEY, SSL_R_NO_PRIVATEKEY);
         return (-1);
     }
 
     is_export = SSL_C_IS_EXPORT(s->session->cipher);
 
                                (s->s2->ssl2_rollback) ? RSA_SSLV23_PADDING :
                                RSA_PKCS1_PADDING);

    is_export = SSL_C_IS_EXPORT(s->session->cipher);

    if (!ssl_cipher_get_evp(s->session, &c, &md, NULL, NULL, NULL)) {
        ssl2_return_error(s, SSL2_PE_NO_CIPHER);
        SSLerr(SSL_F_GET_CLIENT_MASTER_KEY,
               SSL_R_PROBLEMS_MAPPING_CIPHER_FUNCTIONS);
        return (0);
     } else
         ek = 5;
 
    /*
     * The format of the CLIENT-MASTER-KEY message is
     * 1 byte message type
     * 3 bytes cipher
     * 2-byte clear key length (stored in s->s2->tmp.clear)
     * 2-byte encrypted key length (stored in s->s2->tmp.enc)
     * 2-byte key args length (IV etc)
     * clear key
     * encrypted key
     * key args
     *
     * If the cipher is an export cipher, then the encrypted key bytes
     * are a fixed portion of the total key (5 or 8 bytes). The size of
     * this portion is in |ek|. If the cipher is not an export cipher,
     * then the entire key material is encrypted (i.e., clear key length
     * must be zero).
     */
    if ((!is_export && s->s2->tmp.clear != 0) ||
        (is_export && s->s2->tmp.clear + ek != EVP_CIPHER_key_length(c))) {
        ssl2_return_error(s, SSL2_PE_UNDEFINED_ERROR);
        SSLerr(SSL_F_GET_CLIENT_MASTER_KEY,SSL_R_BAD_LENGTH);
        return -1;
    }
    /*
     * The encrypted blob must decrypt to the encrypted portion of the key.
     * Decryption can't be expanding, so if we don't have enough encrypted
     * bytes to fit the key in the buffer, stop now.
     */
    if ((is_export && s->s2->tmp.enc < ek) ||
        (!is_export && s->s2->tmp.enc < EVP_CIPHER_key_length(c))) {
        ssl2_return_error(s,SSL2_PE_UNDEFINED_ERROR);
        SSLerr(SSL_F_GET_CLIENT_MASTER_KEY,SSL_R_LENGTH_TOO_SHORT);
        return -1;
    }

    i = ssl_rsa_private_decrypt(s->cert, s->s2->tmp.enc,
                                &(p[s->s2->tmp.clear]),
                                &(p[s->s2->tmp.clear]),
                                (s->s2->ssl2_rollback) ? RSA_SSLV23_PADDING :
                                RSA_PKCS1_PADDING);

     /* bad decrypt */
 # if 1
     /*
      * If a bad decrypt, continue with protocol but with a random master
      * secret (Bleichenbacher attack)
      */
    if ((i < 0) || ((!is_export && i != EVP_CIPHER_key_length(c))
                    || (is_export && i != ek))) {
         ERR_clear_error();
         if (is_export)
             i = ek;
         else
             i = EVP_CIPHER_key_length(c);
        if (RAND_pseudo_bytes(&p[s->s2->tmp.clear], i) <= 0)
             return 0;
     }
 # else
    unsigned long len;
    unsigned char *p;
    STACK_OF(SSL_CIPHER) *cs;   /* a stack of SSL_CIPHERS */
    STACK_OF(SSL_CIPHER) *cl;   /* the ones we want to use */
    STACK_OF(SSL_CIPHER) *prio, *allow;
    int z;

    /*
     * This is a bit of a hack to check for the correct packet type the first
     * time round.
     */
    if (s->state == SSL2_ST_GET_CLIENT_HELLO_A) {
        s->first_packet = 1;
        s->state = SSL2_ST_GET_CLIENT_HELLO_B;
    }
 # endif
 
     if (is_export)
        i = EVP_CIPHER_key_length(c);
 
     if (i > SSL_MAX_MASTER_KEY_LENGTH) {
         ssl2_return_error(s, SSL2_PE_UNDEFINED_ERROR);

        if (*(p++) != SSL2_MT_CLIENT_HELLO) {
            if (p[-1] != SSL2_MT_ERROR) {
                ssl2_return_error(s, SSL2_PE_UNDEFINED_ERROR);
                SSLerr(SSL_F_GET_CLIENT_HELLO, SSL_R_READ_WRONG_PACKET_TYPE);
            } else
                SSLerr(SSL_F_GET_CLIENT_HELLO, SSL_R_PEER_ERROR);
            return (-1);
        }
        n2s(p, i);
        if (i < s->version)
            s->version = i;
        n2s(p, i);
        s->s2->tmp.cipher_spec_length = i;
        n2s(p, i);
        s->s2->tmp.session_id_length = i;
        n2s(p, i);
        s->s2->challenge_length = i;
        if ((i < SSL2_MIN_CHALLENGE_LENGTH) ||
            (i > SSL2_MAX_CHALLENGE_LENGTH)) {
            ssl2_return_error(s, SSL2_PE_UNDEFINED_ERROR);
            SSLerr(SSL_F_GET_CLIENT_HELLO, SSL_R_INVALID_CHALLENGE_LENGTH);
            return (-1);
        }
        s->state = SSL2_ST_GET_CLIENT_HELLO_C;
    }

    /* SSL2_ST_GET_CLIENT_HELLO_C */
    p = (unsigned char *)s->init_buf->data;
    len =
        9 + (unsigned long)s->s2->tmp.cipher_spec_length +
        (unsigned long)s->s2->challenge_length +
        (unsigned long)s->s2->tmp.session_id_length;
    if (len > SSL2_MAX_RECORD_LENGTH_3_BYTE_HEADER) {
        ssl2_return_error(s, SSL2_PE_UNDEFINED_ERROR);
        SSLerr(SSL_F_GET_CLIENT_HELLO, SSL_R_MESSAGE_TOO_LONG);
        return -1;
    }
    n = (int)len - s->init_num;
    i = ssl2_read(s, (char *)&(p[s->init_num]), n);
    if (i != n)
        return (ssl2_part_read(s, SSL_F_GET_CLIENT_HELLO, i));
    if (s->msg_callback) {
        /* CLIENT-HELLO */
        s->msg_callback(0, s->version, 0, p, (size_t)len, s,
                        s->msg_callback_arg);
    }
    p += 9;

    /*
     * get session-id before cipher stuff so we can get out session structure
     * if it is cached
     */
    /* session-id */
    if ((s->s2->tmp.session_id_length != 0) &&
        (s->s2->tmp.session_id_length != SSL2_SSL_SESSION_ID_LENGTH)) {
        ssl2_return_error(s, SSL2_PE_UNDEFINED_ERROR);
        SSLerr(SSL_F_GET_CLIENT_HELLO, SSL_R_BAD_SSL_SESSION_ID_LENGTH);
        return (-1);
    }

    if (s->s2->tmp.session_id_length == 0) {
        if (!ssl_get_new_session(s, 1)) {
            ssl2_return_error(s, SSL2_PE_UNDEFINED_ERROR);
            return (-1);
        }
    } else {
        i = ssl_get_prev_session(s, &(p[s->s2->tmp.cipher_spec_length]),
                                 s->s2->tmp.session_id_length, NULL);
        if (i == 1) {           /* previous session */
            s->hit = 1;
        } else if (i == -1) {
            ssl2_return_error(s, SSL2_PE_UNDEFINED_ERROR);
            return (-1);
        } else {
            if (s->cert == NULL) {
                ssl2_return_error(s, SSL2_PE_NO_CERTIFICATE);
                SSLerr(SSL_F_GET_CLIENT_HELLO, SSL_R_NO_CERTIFICATE_SET);
                return (-1);
            }

            if (!ssl_get_new_session(s, 1)) {
                ssl2_return_error(s, SSL2_PE_UNDEFINED_ERROR);
                return (-1);
            }
        }
    }

    if (!s->hit) {
        cs = ssl_bytes_to_cipher_list(s, p, s->s2->tmp.cipher_spec_length,
                                      &s->session->ciphers);
        if (cs == NULL)
            goto mem_err;

        cl = SSL_get_ciphers(s);

        if (s->options & SSL_OP_CIPHER_SERVER_PREFERENCE) {
            prio = sk_SSL_CIPHER_dup(cl);
            if (prio == NULL)
                goto mem_err;
            allow = cs;
        } else {
            prio = cs;
            allow = cl;
        }
        for (z = 0; z < sk_SSL_CIPHER_num(prio); z++) {
            if (sk_SSL_CIPHER_find(allow, sk_SSL_CIPHER_value(prio, z)) < 0) {
                (void)sk_SSL_CIPHER_delete(prio, z);
                z--;
            }
        }
        if (s->options & SSL_OP_CIPHER_SERVER_PREFERENCE) {
            sk_SSL_CIPHER_free(s->session->ciphers);
            s->session->ciphers = prio;
        }
        /*
         * s->session->ciphers should now have a list of ciphers that are on
         * both the client and server. This list is ordered by the order the
         * client sent the ciphers or in the order of the server's preference
         * if SSL_OP_CIPHER_SERVER_PREFERENCE was set.
         */
    }
    p += s->s2->tmp.cipher_spec_length;
    /* done cipher selection */

    /* session id extracted already */
    p += s->s2->tmp.session_id_length;

    /* challenge */
    if (s->s2->challenge_length > sizeof s->s2->challenge) {
        ssl2_return_error(s, SSL2_PE_UNDEFINED_ERROR);
        SSLerr(SSL_F_GET_CLIENT_HELLO, ERR_R_INTERNAL_ERROR);
        return -1;
    }
    memcpy(s->s2->challenge, p, (unsigned int)s->s2->challenge_length);
    return (1);
 mem_err:
    SSLerr(SSL_F_GET_CLIENT_HELLO, ERR_R_MALLOC_FAILURE);
    return (0);
}
