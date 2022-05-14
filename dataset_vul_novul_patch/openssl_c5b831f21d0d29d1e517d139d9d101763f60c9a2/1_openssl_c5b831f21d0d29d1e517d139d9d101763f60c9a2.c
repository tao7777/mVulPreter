long ssl3_ctrl(SSL *s, int cmd, long larg, void *parg)
{
    int ret = 0;

#if !defined(OPENSSL_NO_DSA) || !defined(OPENSSL_NO_RSA)
    if (
# ifndef OPENSSL_NO_RSA
           cmd == SSL_CTRL_SET_TMP_RSA || cmd == SSL_CTRL_SET_TMP_RSA_CB ||
# endif
# ifndef OPENSSL_NO_DSA
           cmd == SSL_CTRL_SET_TMP_DH || cmd == SSL_CTRL_SET_TMP_DH_CB ||
# endif
           0) {
        if (!ssl_cert_inst(&s->cert)) {
            SSLerr(SSL_F_SSL3_CTRL, ERR_R_MALLOC_FAILURE);
            return (0);
        }
    }
#endif

    switch (cmd) {
    case SSL_CTRL_GET_SESSION_REUSED:
        ret = s->hit;
        break;
    case SSL_CTRL_GET_CLIENT_CERT_REQUEST:
        break;
    case SSL_CTRL_GET_NUM_RENEGOTIATIONS:
        ret = s->s3->num_renegotiations;
        break;
    case SSL_CTRL_CLEAR_NUM_RENEGOTIATIONS:
        ret = s->s3->num_renegotiations;
        s->s3->num_renegotiations = 0;
        break;
    case SSL_CTRL_GET_TOTAL_RENEGOTIATIONS:
        ret = s->s3->total_renegotiations;
        break;
    case SSL_CTRL_GET_FLAGS:
        ret = (int)(s->s3->flags);
        break;
#ifndef OPENSSL_NO_RSA
    case SSL_CTRL_NEED_TMP_RSA:
        if ((s->cert != NULL) && (s->cert->rsa_tmp == NULL) &&
            ((s->cert->pkeys[SSL_PKEY_RSA_ENC].privatekey == NULL) ||
             (EVP_PKEY_size(s->cert->pkeys[SSL_PKEY_RSA_ENC].privatekey) >
              (512 / 8))))
            ret = 1;
        break;
    case SSL_CTRL_SET_TMP_RSA:
        {
            RSA *rsa = (RSA *)parg;
            if (rsa == NULL) {
                SSLerr(SSL_F_SSL3_CTRL, ERR_R_PASSED_NULL_PARAMETER);
                return (ret);
            }
            if ((rsa = RSAPrivateKey_dup(rsa)) == NULL) {
                SSLerr(SSL_F_SSL3_CTRL, ERR_R_RSA_LIB);
                return (ret);
            }
            if (s->cert->rsa_tmp != NULL)
                RSA_free(s->cert->rsa_tmp);
            s->cert->rsa_tmp = rsa;
            ret = 1;
        }
        break;
    case SSL_CTRL_SET_TMP_RSA_CB:
        {
            SSLerr(SSL_F_SSL3_CTRL, ERR_R_SHOULD_NOT_HAVE_BEEN_CALLED);
            return (ret);
        }
        break;
#endif
#ifndef OPENSSL_NO_DH
    case SSL_CTRL_SET_TMP_DH:
        {
            DH *dh = (DH *)parg;
            if (dh == NULL) {
                SSLerr(SSL_F_SSL3_CTRL, ERR_R_PASSED_NULL_PARAMETER);
                return (ret);
            }
            if ((dh = DHparams_dup(dh)) == NULL) {
                 SSLerr(SSL_F_SSL3_CTRL, ERR_R_DH_LIB);
                 return (ret);
             }
            if (!(s->options & SSL_OP_SINGLE_DH_USE)) {
                if (!DH_generate_key(dh)) {
                    DH_free(dh);
                    SSLerr(SSL_F_SSL3_CTRL, ERR_R_DH_LIB);
                    return (ret);
                }
            }
             if (s->cert->dh_tmp != NULL)
                 DH_free(s->cert->dh_tmp);
             s->cert->dh_tmp = dh;
                    SSLerr(SSL_F_SSL3_CTRL, ERR_R_DH_LIB);
                    return (ret);
                }
            }
            if (s->cert->dh_tmp != NULL)
                DH_free(s->cert->dh_tmp);
            s->cert->dh_tmp = dh;
            ret = 1;
        }
