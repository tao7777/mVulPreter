int ssl3_write_bytes(SSL *s, int type, const void *buf_, int len)
{
    const unsigned char *buf = buf_;
    int tot;
    unsigned int n, split_send_fragment, maxpipes;
#if !defined(OPENSSL_NO_MULTIBLOCK) && EVP_CIPH_FLAG_TLS1_1_MULTIBLOCK
    unsigned int max_send_fragment, nw;
    unsigned int u_len = (unsigned int)len;
#endif
    SSL3_BUFFER *wb = &s->rlayer.wbuf[0];
    int i;

    if (len < 0) {
        SSLerr(SSL_F_SSL3_WRITE_BYTES, SSL_R_SSL_NEGATIVE_LENGTH);
        return -1;
    }

    s->rwstate = SSL_NOTHING;
    tot = s->rlayer.wnum;
    /*
     * ensure that if we end up with a smaller value of data to write out
     * than the the original len from a write which didn't complete for
     * non-blocking I/O and also somehow ended up avoiding the check for
     * this in ssl3_write_pending/SSL_R_BAD_WRITE_RETRY as it must never be
     * possible to end up with (len-tot) as a large number that will then
     * promptly send beyond the end of the users buffer ... so we trap and
     * report the error in a way the user will notice
     */
    if ((unsigned int)len < s->rlayer.wnum) {
        SSLerr(SSL_F_SSL3_WRITE_BYTES, SSL_R_BAD_LENGTH);
        return -1;
    }

    s->rlayer.wnum = 0;

    if (SSL_in_init(s) && !ossl_statem_get_in_handshake(s)) {
        i = s->handshake_func(s);
        if (i < 0)
            return (i);
        if (i == 0) {
            SSLerr(SSL_F_SSL3_WRITE_BYTES, SSL_R_SSL_HANDSHAKE_FAILURE);
            return -1;
        }
    }

    /*
     * first check if there is a SSL3_BUFFER still being written out.  This
     * will happen with non blocking IO
     */
    if (wb->left != 0) {
        i = ssl3_write_pending(s, type, &buf[tot], s->rlayer.wpend_tot);
        if (i <= 0) {
            /* XXX should we ssl3_release_write_buffer if i<0? */
            s->rlayer.wnum = tot;
            return i;
        }
        tot += i;               /* this might be last fragment */
    }
#if !defined(OPENSSL_NO_MULTIBLOCK) && EVP_CIPH_FLAG_TLS1_1_MULTIBLOCK
    /*
     * Depending on platform multi-block can deliver several *times*
     * better performance. Downside is that it has to allocate
     * jumbo buffer to accommodate up to 8 records, but the
     * compromise is considered worthy.
     */
     if (type == SSL3_RT_APPLICATION_DATA &&
         u_len >= 4 * (max_send_fragment = s->max_send_fragment) &&
         s->compress == NULL && s->msg_callback == NULL &&
        !SSL_WRITE_ETM(s) && SSL_USE_EXPLICIT_IV(s) &&
         EVP_CIPHER_flags(EVP_CIPHER_CTX_cipher(s->enc_write_ctx)) &
         EVP_CIPH_FLAG_TLS1_1_MULTIBLOCK) {
         unsigned char aad[13];
        EVP_CTRL_TLS1_1_MULTIBLOCK_PARAM mb_param;
        int packlen;

        /* minimize address aliasing conflicts */
        if ((max_send_fragment & 0xfff) == 0)
            max_send_fragment -= 512;

        if (tot == 0 || wb->buf == NULL) { /* allocate jumbo buffer */
            ssl3_release_write_buffer(s);

            packlen = EVP_CIPHER_CTX_ctrl(s->enc_write_ctx,
                                          EVP_CTRL_TLS1_1_MULTIBLOCK_MAX_BUFSIZE,
                                          max_send_fragment, NULL);

            if (u_len >= 8 * max_send_fragment)
                packlen *= 8;
            else
                packlen *= 4;

            if (!ssl3_setup_write_buffer(s, 1, packlen)) {
                SSLerr(SSL_F_SSL3_WRITE_BYTES, ERR_R_MALLOC_FAILURE);
                return -1;
            }
        } else if (tot == len) { /* done? */
            /* free jumbo buffer */
            ssl3_release_write_buffer(s);
            return tot;
        }

        n = (len - tot);
        for (;;) {
            if (n < 4 * max_send_fragment) {
                /* free jumbo buffer */
                ssl3_release_write_buffer(s);
                break;
            }

            if (s->s3->alert_dispatch) {
                i = s->method->ssl_dispatch_alert(s);
                if (i <= 0) {
                    s->rlayer.wnum = tot;
                    return i;
                }
            }

            if (n >= 8 * max_send_fragment)
                nw = max_send_fragment * (mb_param.interleave = 8);
            else
                nw = max_send_fragment * (mb_param.interleave = 4);

            memcpy(aad, s->rlayer.write_sequence, 8);
            aad[8] = type;
            aad[9] = (unsigned char)(s->version >> 8);
            aad[10] = (unsigned char)(s->version);
            aad[11] = 0;
            aad[12] = 0;
            mb_param.out = NULL;
            mb_param.inp = aad;
            mb_param.len = nw;

            packlen = EVP_CIPHER_CTX_ctrl(s->enc_write_ctx,
                                          EVP_CTRL_TLS1_1_MULTIBLOCK_AAD,
                                          sizeof(mb_param), &mb_param);

            if (packlen <= 0 || packlen > (int)wb->len) { /* never happens */
                /* free jumbo buffer */
                ssl3_release_write_buffer(s);
                break;
            }

            mb_param.out = wb->buf;
            mb_param.inp = &buf[tot];
            mb_param.len = nw;

            if (EVP_CIPHER_CTX_ctrl(s->enc_write_ctx,
                                    EVP_CTRL_TLS1_1_MULTIBLOCK_ENCRYPT,
                                    sizeof(mb_param), &mb_param) <= 0)
                return -1;

            s->rlayer.write_sequence[7] += mb_param.interleave;
            if (s->rlayer.write_sequence[7] < mb_param.interleave) {
                int j = 6;
                while (j >= 0 && (++s->rlayer.write_sequence[j--]) == 0) ;
            }

            wb->offset = 0;
            wb->left = packlen;

            s->rlayer.wpend_tot = nw;
            s->rlayer.wpend_buf = &buf[tot];
            s->rlayer.wpend_type = type;
            s->rlayer.wpend_ret = nw;

            i = ssl3_write_pending(s, type, &buf[tot], nw);
            if (i <= 0) {
                if (i < 0 && (!s->wbio || !BIO_should_retry(s->wbio))) {
                    /* free jumbo buffer */
                    ssl3_release_write_buffer(s);
                }
                s->rlayer.wnum = tot;
                return i;
            }
            if (i == (int)n) {
                /* free jumbo buffer */
                ssl3_release_write_buffer(s);
                return tot + i;
            }
            n -= i;
            tot += i;
        }
    } else
#endif
    if (tot == len) {           /* done? */
        if (s->mode & SSL_MODE_RELEASE_BUFFERS && !SSL_IS_DTLS(s))
            ssl3_release_write_buffer(s);

        return tot;
    }

    n = (len - tot);

    split_send_fragment = s->split_send_fragment;
    /*
     * If max_pipelines is 0 then this means "undefined" and we default to
     * 1 pipeline. Similarly if the cipher does not support pipelined
     * processing then we also only use 1 pipeline, or if we're not using
     * explicit IVs
     */
    maxpipes = s->max_pipelines;
    if (maxpipes > SSL_MAX_PIPELINES) {
        /*
         * We should have prevented this when we set max_pipelines so we
         * shouldn't get here
         */
        SSLerr(SSL_F_SSL3_WRITE_BYTES, ERR_R_INTERNAL_ERROR);
        return -1;
    }
    if (maxpipes == 0
        || s->enc_write_ctx == NULL
        || !(EVP_CIPHER_flags(EVP_CIPHER_CTX_cipher(s->enc_write_ctx))
             & EVP_CIPH_FLAG_PIPELINE)
        || !SSL_USE_EXPLICIT_IV(s))
        maxpipes = 1;
    if (s->max_send_fragment == 0 || split_send_fragment > s->max_send_fragment
        || split_send_fragment == 0) {
        /*
         * We should have prevented this when we set the split and max send
         * fragments so we shouldn't get here
         */
        SSLerr(SSL_F_SSL3_WRITE_BYTES, ERR_R_INTERNAL_ERROR);
        return -1;
    }

    for (;;) {
        unsigned int pipelens[SSL_MAX_PIPELINES], tmppipelen, remain;
        unsigned int numpipes, j;

        if (n == 0)
            numpipes = 1;
        else
            numpipes = ((n - 1) / split_send_fragment) + 1;
        if (numpipes > maxpipes)
            numpipes = maxpipes;

        if (n / numpipes >= s->max_send_fragment) {
            /*
             * We have enough data to completely fill all available
             * pipelines
             */
            for (j = 0; j < numpipes; j++) {
                pipelens[j] = s->max_send_fragment;
            }
        } else {
            /* We can partially fill all available pipelines */
            tmppipelen = n / numpipes;
            remain = n % numpipes;
            for (j = 0; j < numpipes; j++) {
                pipelens[j] = tmppipelen;
                if (j < remain)
                    pipelens[j]++;
            }
        }

        i = do_ssl3_write(s, type, &(buf[tot]), pipelens, numpipes, 0);
        if (i <= 0) {
            /* XXX should we ssl3_release_write_buffer if i<0? */
            s->rlayer.wnum = tot;
            return i;
        }

        if ((i == (int)n) ||
            (type == SSL3_RT_APPLICATION_DATA &&
             (s->mode & SSL_MODE_ENABLE_PARTIAL_WRITE))) {
            /*
             * next chunk of data should get another prepended empty fragment
             * in ciphersuites with known-IV weakness:
             */
            s->s3->empty_fragment_done = 0;

            if ((i == (int)n) && s->mode & SSL_MODE_RELEASE_BUFFERS &&
                !SSL_IS_DTLS(s))
                ssl3_release_write_buffer(s);

            return tot + i;
        }

        n -= i;
        tot += i;
    }
}
