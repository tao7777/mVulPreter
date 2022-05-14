int do_ssl3_write(SSL *s, int type, const unsigned char *buf,
                  unsigned int *pipelens, unsigned int numpipes,
                  int create_empty_fragment)
{
    unsigned char *outbuf[SSL_MAX_PIPELINES], *plen[SSL_MAX_PIPELINES];
    SSL3_RECORD wr[SSL_MAX_PIPELINES];
    int i, mac_size, clear = 0;
    int prefix_len = 0;
    int eivlen;
    size_t align = 0;
    SSL3_BUFFER *wb;
    SSL_SESSION *sess;
    unsigned int totlen = 0;
    unsigned int j;

    for (j = 0; j < numpipes; j++)
        totlen += pipelens[j];
    /*
     * first check if there is a SSL3_BUFFER still being written out.  This
     * will happen with non blocking IO
     */
    if (RECORD_LAYER_write_pending(&s->rlayer))
        return (ssl3_write_pending(s, type, buf, totlen));

    /* If we have an alert to send, lets send it */
    if (s->s3->alert_dispatch) {
        i = s->method->ssl_dispatch_alert(s);
        if (i <= 0)
            return (i);
        /* if it went, fall through and send more stuff */
    }

    if (s->rlayer.numwpipes < numpipes)
        if (!ssl3_setup_write_buffer(s, numpipes, 0))
            return -1;

    if (totlen == 0 && !create_empty_fragment)
        return 0;

    sess = s->session;

    if ((sess == NULL) ||
        (s->enc_write_ctx == NULL) || (EVP_MD_CTX_md(s->write_hash) == NULL)) {
        clear = s->enc_write_ctx ? 0 : 1; /* must be AEAD cipher */
        mac_size = 0;
    } else {
        mac_size = EVP_MD_CTX_size(s->write_hash);
        if (mac_size < 0)
            goto err;
    }

    /*
     * 'create_empty_fragment' is true only when this function calls itself
     */
    if (!clear && !create_empty_fragment && !s->s3->empty_fragment_done) {
        /*
         * countermeasure against known-IV weakness in CBC ciphersuites (see
         * http://www.openssl.org/~bodo/tls-cbc.txt)
         */

        if (s->s3->need_empty_fragments && type == SSL3_RT_APPLICATION_DATA) {
            /*
             * recursive function call with 'create_empty_fragment' set; this
             * prepares and buffers the data for an empty fragment (these
             * 'prefix_len' bytes are sent out later together with the actual
             * payload)
             */
            unsigned int tmppipelen = 0;

            prefix_len = do_ssl3_write(s, type, buf, &tmppipelen, 1, 1);
            if (prefix_len <= 0)
                goto err;

            if (prefix_len >
                (SSL3_RT_HEADER_LENGTH + SSL3_RT_SEND_MAX_ENCRYPTED_OVERHEAD)) {
                /* insufficient space */
                SSLerr(SSL_F_DO_SSL3_WRITE, ERR_R_INTERNAL_ERROR);
                goto err;
            }
        }

        s->s3->empty_fragment_done = 1;
    }

    if (create_empty_fragment) {
        wb = &s->rlayer.wbuf[0];
#if defined(SSL3_ALIGN_PAYLOAD) && SSL3_ALIGN_PAYLOAD!=0
        /*
         * extra fragment would be couple of cipher blocks, which would be
         * multiple of SSL3_ALIGN_PAYLOAD, so if we want to align the real
         * payload, then we can just pretend we simply have two headers.
         */
        align = (size_t)SSL3_BUFFER_get_buf(wb) + 2 * SSL3_RT_HEADER_LENGTH;
        align = SSL3_ALIGN_PAYLOAD - 1 - ((align - 1) % SSL3_ALIGN_PAYLOAD);
#endif
        outbuf[0] = SSL3_BUFFER_get_buf(wb) + align;
        SSL3_BUFFER_set_offset(wb, align);
    } else if (prefix_len) {
        wb = &s->rlayer.wbuf[0];
        outbuf[0] = SSL3_BUFFER_get_buf(wb) + SSL3_BUFFER_get_offset(wb)
            + prefix_len;
    } else {
        for (j = 0; j < numpipes; j++) {
            wb = &s->rlayer.wbuf[j];
#if defined(SSL3_ALIGN_PAYLOAD) && SSL3_ALIGN_PAYLOAD!=0
            align = (size_t)SSL3_BUFFER_get_buf(wb) + SSL3_RT_HEADER_LENGTH;
            align = SSL3_ALIGN_PAYLOAD - 1 - ((align - 1) % SSL3_ALIGN_PAYLOAD);
#endif
            outbuf[j] = SSL3_BUFFER_get_buf(wb) + align;
            SSL3_BUFFER_set_offset(wb, align);
        }
    }

    /* Explicit IV length, block ciphers appropriate version flag */
    if (s->enc_write_ctx && SSL_USE_EXPLICIT_IV(s)) {
        int mode = EVP_CIPHER_CTX_mode(s->enc_write_ctx);
        if (mode == EVP_CIPH_CBC_MODE) {
            eivlen = EVP_CIPHER_CTX_iv_length(s->enc_write_ctx);
            if (eivlen <= 1)
                eivlen = 0;
        }
        /* Need explicit part of IV for GCM mode */
        else if (mode == EVP_CIPH_GCM_MODE)
            eivlen = EVP_GCM_TLS_EXPLICIT_IV_LEN;
        else if (mode == EVP_CIPH_CCM_MODE)
            eivlen = EVP_CCM_TLS_EXPLICIT_IV_LEN;
        else
            eivlen = 0;
    } else
        eivlen = 0;

    totlen = 0;
    /* Clear our SSL3_RECORD structures */
    memset(wr, 0, sizeof wr);
    for (j = 0; j < numpipes; j++) {
        /* write the header */
        *(outbuf[j]++) = type & 0xff;
        SSL3_RECORD_set_type(&wr[j], type);

        *(outbuf[j]++) = (s->version >> 8);
        /*
         * Some servers hang if initial client hello is larger than 256 bytes
         * and record version number > TLS 1.0
         */
        if (SSL_get_state(s) == TLS_ST_CW_CLNT_HELLO
            && !s->renegotiate && TLS1_get_version(s) > TLS1_VERSION)
            *(outbuf[j]++) = 0x1;
        else
            *(outbuf[j]++) = s->version & 0xff;

        /* field where we are to write out packet length */
        plen[j] = outbuf[j];
        outbuf[j] += 2;

        /* lets setup the record stuff. */
        SSL3_RECORD_set_data(&wr[j], outbuf[j] + eivlen);
        SSL3_RECORD_set_length(&wr[j], (int)pipelens[j]);
        SSL3_RECORD_set_input(&wr[j], (unsigned char *)&buf[totlen]);
        totlen += pipelens[j];

        /*
         * we now 'read' from wr->input, wr->length bytes into wr->data
         */

        /* first we compress */
        if (s->compress != NULL) {
            if (!ssl3_do_compress(s, &wr[j])) {
                SSLerr(SSL_F_DO_SSL3_WRITE, SSL_R_COMPRESSION_FAILURE);
                goto err;
            }
        } else {
            memcpy(wr[j].data, wr[j].input, wr[j].length);
            SSL3_RECORD_reset_input(&wr[j]);
        }

        /*
         * we should still have the output to wr->data and the input from
         * wr->input.  Length should be wr->length. wr->data still points in the
          * wb->buf
          */
 
        if (!SSL_WRITE_ETM(s) && mac_size != 0) {
             if (s->method->ssl3_enc->mac(s, &wr[j],
                                          &(outbuf[j][wr[j].length + eivlen]),
                                          1) < 0)
                goto err;
            SSL3_RECORD_add_length(&wr[j], mac_size);
        }

        SSL3_RECORD_set_data(&wr[j], outbuf[j]);
        SSL3_RECORD_reset_input(&wr[j]);

        if (eivlen) {
            /*
             * if (RAND_pseudo_bytes(p, eivlen) <= 0) goto err;
             */
            SSL3_RECORD_add_length(&wr[j], eivlen);
        }
    }

    if (s->method->ssl3_enc->enc(s, wr, numpipes, 1) < 1)
         goto err;
 
     for (j = 0; j < numpipes; j++) {
        if (SSL_WRITE_ETM(s) && mac_size != 0) {
             if (s->method->ssl3_enc->mac(s, &wr[j],
                                          outbuf[j] + wr[j].length, 1) < 0)
                 goto err;
            SSL3_RECORD_add_length(&wr[j], mac_size);
        }

        /* record length after mac and block padding */
        s2n(SSL3_RECORD_get_length(&wr[j]), plen[j]);

        if (s->msg_callback)
            s->msg_callback(1, 0, SSL3_RT_HEADER, plen[j] - 5, 5, s,
                            s->msg_callback_arg);

        /*
         * we should now have wr->data pointing to the encrypted data, which is
         * wr->length long
         */
        SSL3_RECORD_set_type(&wr[j], type); /* not needed but helps for
                                             * debugging */
        SSL3_RECORD_add_length(&wr[j], SSL3_RT_HEADER_LENGTH);

        if (create_empty_fragment) {
            /*
             * we are in a recursive call; just return the length, don't write
             * out anything here
             */
            if (j > 0) {
                /* We should never be pipelining an empty fragment!! */
                SSLerr(SSL_F_DO_SSL3_WRITE, ERR_R_INTERNAL_ERROR);
                goto err;
            }
            return SSL3_RECORD_get_length(wr);
        }

        /* now let's set up wb */
        SSL3_BUFFER_set_left(&s->rlayer.wbuf[j],
                             prefix_len + SSL3_RECORD_get_length(&wr[j]));
    }

    /*
     * memorize arguments so that ssl3_write_pending can detect bad write
     * retries later
     */
    s->rlayer.wpend_tot = totlen;
    s->rlayer.wpend_buf = buf;
    s->rlayer.wpend_type = type;
    s->rlayer.wpend_ret = totlen;

    /* we now just need to write the buffer */
    return ssl3_write_pending(s, type, buf, totlen);
 err:
    return -1;
}
