long ssl3_ctx_ctrl(SSL_CTX *ctx, int cmd, long larg, void *parg)
{
    CERT *cert;

    cert = ctx->cert;

    switch (cmd) {
#ifndef OPENSSL_NO_RSA
    case SSL_CTRL_NEED_TMP_RSA:
        if ((cert->rsa_tmp == NULL) &&
            ((cert->pkeys[SSL_PKEY_RSA_ENC].privatekey == NULL) ||
             (EVP_PKEY_size(cert->pkeys[SSL_PKEY_RSA_ENC].privatekey) >
              (512 / 8)))
            )
            return (1);
        else
            return (0);
        /* break; */
    case SSL_CTRL_SET_TMP_RSA:
        {
            RSA *rsa;
            int i;

            rsa = (RSA *)parg;
            i = 1;
            if (rsa == NULL)
                i = 0;
            else {
                if ((rsa = RSAPrivateKey_dup(rsa)) == NULL)
                    i = 0;
            }
            if (!i) {
                SSLerr(SSL_F_SSL3_CTX_CTRL, ERR_R_RSA_LIB);
                return (0);
            } else {
                if (cert->rsa_tmp != NULL)
                    RSA_free(cert->rsa_tmp);
                cert->rsa_tmp = rsa;
                return (1);
            }
        }
        /* break; */
    case SSL_CTRL_SET_TMP_RSA_CB:
        {
            SSLerr(SSL_F_SSL3_CTX_CTRL, ERR_R_SHOULD_NOT_HAVE_BEEN_CALLED);
            return (0);
        }
        break;
#endif
                 SSLerr(SSL_F_SSL3_CTX_CTRL, ERR_R_DH_LIB);
                 return 0;
             }
            if (!(ctx->options & SSL_OP_SINGLE_DH_USE)) {
                if (!DH_generate_key(new)) {
                    SSLerr(SSL_F_SSL3_CTX_CTRL, ERR_R_DH_LIB);
                    DH_free(new);
                    return 0;
                }
            }
             if (cert->dh_tmp != NULL)
                 DH_free(cert->dh_tmp);
             cert->dh_tmp = new;
            if ((new = DHparams_dup(dh)) == NULL) {
                SSLerr(SSL_F_SSL3_CTX_CTRL, ERR_R_DH_LIB);
                return 0;
            }
            if (!(ctx->options & SSL_OP_SINGLE_DH_USE)) {
                if (!DH_generate_key(new)) {
                    SSLerr(SSL_F_SSL3_CTX_CTRL, ERR_R_DH_LIB);
                    DH_free(new);
                    return 0;
                }
            }
            if (cert->dh_tmp != NULL)
                DH_free(cert->dh_tmp);
            cert->dh_tmp = new;
            return 1;
        }
