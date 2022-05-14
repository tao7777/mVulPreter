int ssl3_send_server_key_exchange(SSL *s)
{
#ifndef OPENSSL_NO_RSA
    unsigned char *q;
    int j, num;
    RSA *rsa;
    unsigned char md_buf[MD5_DIGEST_LENGTH + SHA_DIGEST_LENGTH];
    unsigned int u;
#endif
#ifndef OPENSSL_NO_DH
    DH *dh = NULL, *dhp;
#endif
#ifndef OPENSSL_NO_ECDH
    EC_KEY *ecdh = NULL, *ecdhp;
    unsigned char *encodedPoint = NULL;
    int encodedlen = 0;
    int curve_id = 0;
    BN_CTX *bn_ctx = NULL;
#endif
    EVP_PKEY *pkey;
    const EVP_MD *md = NULL;
    unsigned char *p, *d;
    int al, i;
    unsigned long type;
    int n;
    CERT *cert;
    BIGNUM *r[4];
    int nr[4], kn;
    BUF_MEM *buf;
    EVP_MD_CTX md_ctx;

    EVP_MD_CTX_init(&md_ctx);
    if (s->state == SSL3_ST_SW_KEY_EXCH_A) {
        type = s->s3->tmp.new_cipher->algorithm_mkey;
        cert = s->cert;

        buf = s->init_buf;

        r[0] = r[1] = r[2] = r[3] = NULL;
        n = 0;
#ifndef OPENSSL_NO_RSA
        if (type & SSL_kRSA) {
            rsa = cert->rsa_tmp;
            if ((rsa == NULL) && (s->cert->rsa_tmp_cb != NULL)) {
                rsa = s->cert->rsa_tmp_cb(s,
                                          SSL_C_IS_EXPORT(s->s3->
                                                          tmp.new_cipher),
                                          SSL_C_EXPORT_PKEYLENGTH(s->s3->
                                                                  tmp.new_cipher));
                if (rsa == NULL) {
                    al = SSL_AD_HANDSHAKE_FAILURE;
                    SSLerr(SSL_F_SSL3_SEND_SERVER_KEY_EXCHANGE,
                           SSL_R_ERROR_GENERATING_TMP_RSA_KEY);
                    goto f_err;
                }
                RSA_up_ref(rsa);
                cert->rsa_tmp = rsa;
            }
            if (rsa == NULL) {
                al = SSL_AD_HANDSHAKE_FAILURE;
                SSLerr(SSL_F_SSL3_SEND_SERVER_KEY_EXCHANGE,
                       SSL_R_MISSING_TMP_RSA_KEY);
                goto f_err;
            }
            r[0] = rsa->n;
            r[1] = rsa->e;
            s->s3->tmp.use_rsa_tmp = 1;
        } else
#endif
#ifndef OPENSSL_NO_DH
        if (type & SSL_kEDH) {
            dhp = cert->dh_tmp;
            if ((dhp == NULL) && (s->cert->dh_tmp_cb != NULL))
                dhp = s->cert->dh_tmp_cb(s,
                                         SSL_C_IS_EXPORT(s->s3->
                                                         tmp.new_cipher),
                                         SSL_C_EXPORT_PKEYLENGTH(s->s3->
                                                                 tmp.new_cipher));
            if (dhp == NULL) {
                al = SSL_AD_HANDSHAKE_FAILURE;
                SSLerr(SSL_F_SSL3_SEND_SERVER_KEY_EXCHANGE,
                       SSL_R_MISSING_TMP_DH_KEY);
                goto f_err;
            }

            if (s->s3->tmp.dh != NULL) {
                SSLerr(SSL_F_SSL3_SEND_SERVER_KEY_EXCHANGE,
                       ERR_R_INTERNAL_ERROR);
                goto err;
            }

            if ((dh = DHparams_dup(dhp)) == NULL) {
                SSLerr(SSL_F_SSL3_SEND_SERVER_KEY_EXCHANGE, ERR_R_DH_LIB);
                goto err;
             }
 
             s->s3->tmp.dh = dh;
            if ((dhp->pub_key == NULL ||
                 dhp->priv_key == NULL ||
                 (s->options & SSL_OP_SINGLE_DH_USE))) {
                if (!DH_generate_key(dh)) {
                    SSLerr(SSL_F_SSL3_SEND_SERVER_KEY_EXCHANGE, ERR_R_DH_LIB);
                    goto err;
                }
            } else {
                dh->pub_key = BN_dup(dhp->pub_key);
                dh->priv_key = BN_dup(dhp->priv_key);
                if ((dh->pub_key == NULL) || (dh->priv_key == NULL)) {
                    SSLerr(SSL_F_SSL3_SEND_SERVER_KEY_EXCHANGE, ERR_R_DH_LIB);
                    goto err;
                }
             }
             r[0] = dh->p;
             r[1] = dh->g;
                }
            } else {
                dh->pub_key = BN_dup(dhp->pub_key);
                dh->priv_key = BN_dup(dhp->priv_key);
                if ((dh->pub_key == NULL) || (dh->priv_key == NULL)) {
                    SSLerr(SSL_F_SSL3_SEND_SERVER_KEY_EXCHANGE, ERR_R_DH_LIB);
                    goto err;
                }
            }
            r[0] = dh->p;
            r[1] = dh->g;
            r[2] = dh->pub_key;
        } else
