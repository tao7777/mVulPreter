int ssl3_get_record(SSL *s)
{
    int ssl_major, ssl_minor, al;
    int enc_err, n, i, ret = -1;
    SSL3_RECORD *rr;
    SSL3_BUFFER *rbuf;
    SSL_SESSION *sess;
    unsigned char *p;
    unsigned char md[EVP_MAX_MD_SIZE];
    short version;
    unsigned mac_size;
    unsigned int num_recs = 0;
    unsigned int max_recs;
    unsigned int j;

    rr = RECORD_LAYER_get_rrec(&s->rlayer);
    rbuf = RECORD_LAYER_get_rbuf(&s->rlayer);
    max_recs = s->max_pipelines;
    if (max_recs == 0)
        max_recs = 1;
    sess = s->session;

    do {
        /* check if we have the header */
        if ((RECORD_LAYER_get_rstate(&s->rlayer) != SSL_ST_READ_BODY) ||
            (RECORD_LAYER_get_packet_length(&s->rlayer)
             < SSL3_RT_HEADER_LENGTH)) {
            n = ssl3_read_n(s, SSL3_RT_HEADER_LENGTH,
                            SSL3_BUFFER_get_len(rbuf), 0,
                            num_recs == 0 ? 1 : 0);
            if (n <= 0)
                return (n);     /* error or non-blocking */
            RECORD_LAYER_set_rstate(&s->rlayer, SSL_ST_READ_BODY);

            p = RECORD_LAYER_get_packet(&s->rlayer);

            /*
             * The first record received by the server may be a V2ClientHello.
             */
            if (s->server && RECORD_LAYER_is_first_record(&s->rlayer)
                && (p[0] & 0x80) && (p[2] == SSL2_MT_CLIENT_HELLO)) {
                /*
                 *  SSLv2 style record
                 *
                 * |num_recs| here will actually always be 0 because
                 * |num_recs > 0| only ever occurs when we are processing
                 * multiple app data records - which we know isn't the case here
                 * because it is an SSLv2ClientHello. We keep it using
                 * |num_recs| for the sake of consistency
                 */
                rr[num_recs].type = SSL3_RT_HANDSHAKE;
                rr[num_recs].rec_version = SSL2_VERSION;

                rr[num_recs].length = ((p[0] & 0x7f) << 8) | p[1];

                if (rr[num_recs].length > SSL3_BUFFER_get_len(rbuf)
                    - SSL2_RT_HEADER_LENGTH) {
                    al = SSL_AD_RECORD_OVERFLOW;
                    SSLerr(SSL_F_SSL3_GET_RECORD, SSL_R_PACKET_LENGTH_TOO_LONG);
                    goto f_err;
                }

                if (rr[num_recs].length < MIN_SSL2_RECORD_LEN) {
                    al = SSL_AD_HANDSHAKE_FAILURE;
                    SSLerr(SSL_F_SSL3_GET_RECORD, SSL_R_LENGTH_TOO_SHORT);
                    goto f_err;
                }
            } else {
                /* SSLv3+ style record */
                if (s->msg_callback)
                    s->msg_callback(0, 0, SSL3_RT_HEADER, p, 5, s,
                                    s->msg_callback_arg);

                /* Pull apart the header into the SSL3_RECORD */
                rr[num_recs].type = *(p++);
                ssl_major = *(p++);
                ssl_minor = *(p++);
                version = (ssl_major << 8) | ssl_minor;
                rr[num_recs].rec_version = version;
                n2s(p, rr[num_recs].length);

                /* Lets check version */
                if (!s->first_packet && version != s->version) {
                    SSLerr(SSL_F_SSL3_GET_RECORD, SSL_R_WRONG_VERSION_NUMBER);
                    if ((s->version & 0xFF00) == (version & 0xFF00)
                        && !s->enc_write_ctx && !s->write_hash) {
                        if (rr->type == SSL3_RT_ALERT) {
                            /*
                             * The record is using an incorrect version number,
                             * but what we've got appears to be an alert. We
                             * haven't read the body yet to check whether its a
                             * fatal or not - but chances are it is. We probably
                             * shouldn't send a fatal alert back. We'll just
                             * end.
                             */
                            goto err;
                        }
                        /*
                         * Send back error using their minor version number :-)
                         */
                        s->version = (unsigned short)version;
                    }
                    al = SSL_AD_PROTOCOL_VERSION;
                    goto f_err;
                }

                if ((version >> 8) != SSL3_VERSION_MAJOR) {
                    if (RECORD_LAYER_is_first_record(&s->rlayer)) {
                        /* Go back to start of packet, look at the five bytes
                         * that we have. */
                        p = RECORD_LAYER_get_packet(&s->rlayer);
                        if (strncmp((char *)p, "GET ", 4) == 0 ||
                            strncmp((char *)p, "POST ", 5) == 0 ||
                            strncmp((char *)p, "HEAD ", 5) == 0 ||
                            strncmp((char *)p, "PUT ", 4) == 0) {
                            SSLerr(SSL_F_SSL3_GET_RECORD, SSL_R_HTTP_REQUEST);
                            goto err;
                        } else if (strncmp((char *)p, "CONNE", 5) == 0) {
                            SSLerr(SSL_F_SSL3_GET_RECORD,
                                   SSL_R_HTTPS_PROXY_REQUEST);
                            goto err;
                        }

                        /* Doesn't look like TLS - don't send an alert */
                        SSLerr(SSL_F_SSL3_GET_RECORD,
                               SSL_R_WRONG_VERSION_NUMBER);
                        goto err;
                    } else {
                        SSLerr(SSL_F_SSL3_GET_RECORD,
                               SSL_R_WRONG_VERSION_NUMBER);
                        al = SSL_AD_PROTOCOL_VERSION;
                        goto f_err;
                    }
                }

                if (rr[num_recs].length >
                    SSL3_BUFFER_get_len(rbuf) - SSL3_RT_HEADER_LENGTH) {
                    al = SSL_AD_RECORD_OVERFLOW;
                    SSLerr(SSL_F_SSL3_GET_RECORD, SSL_R_PACKET_LENGTH_TOO_LONG);
                    goto f_err;
                }
            }

            /* now s->rlayer.rstate == SSL_ST_READ_BODY */
        }

        /*
         * s->rlayer.rstate == SSL_ST_READ_BODY, get and decode the data.
         * Calculate how much more data we need to read for the rest of the
         * record
         */
        if (rr[num_recs].rec_version == SSL2_VERSION) {
            i = rr[num_recs].length + SSL2_RT_HEADER_LENGTH
                - SSL3_RT_HEADER_LENGTH;
        } else {
            i = rr[num_recs].length;
        }
        if (i > 0) {
            /* now s->packet_length == SSL3_RT_HEADER_LENGTH */

            n = ssl3_read_n(s, i, i, 1, 0);
            if (n <= 0)
                return (n);     /* error or non-blocking io */
        }

        /* set state for later operations */
        RECORD_LAYER_set_rstate(&s->rlayer, SSL_ST_READ_HEADER);

        /*
         * At this point, s->packet_length == SSL3_RT_HEADER_LENGTH + rr->length,
         * or s->packet_length == SSL2_RT_HEADER_LENGTH + rr->length
         * and we have that many bytes in s->packet
         */
        if (rr[num_recs].rec_version == SSL2_VERSION) {
            rr[num_recs].input =
                &(RECORD_LAYER_get_packet(&s->rlayer)[SSL2_RT_HEADER_LENGTH]);
        } else {
            rr[num_recs].input =
                &(RECORD_LAYER_get_packet(&s->rlayer)[SSL3_RT_HEADER_LENGTH]);
        }

        /*
         * ok, we can now read from 's->packet' data into 'rr' rr->input points
         * at rr->length bytes, which need to be copied into rr->data by either
         * the decryption or by the decompression When the data is 'copied' into
         * the rr->data buffer, rr->input will be pointed at the new buffer
         */

        /*
         * We now have - encrypted [ MAC [ compressed [ plain ] ] ] rr->length
         * bytes of encrypted compressed stuff.
         */

        /* check is not needed I believe */
        if (rr[num_recs].length > SSL3_RT_MAX_ENCRYPTED_LENGTH) {
            al = SSL_AD_RECORD_OVERFLOW;
            SSLerr(SSL_F_SSL3_GET_RECORD, SSL_R_ENCRYPTED_LENGTH_TOO_LONG);
            goto f_err;
        }

        /* decrypt in place in 'rr->input' */
        rr[num_recs].data = rr[num_recs].input;
        rr[num_recs].orig_len = rr[num_recs].length;

        /* Mark this record as not read by upper layers yet */
        rr[num_recs].read = 0;

        num_recs++;

        /* we have pulled in a full packet so zero things */
        RECORD_LAYER_reset_packet_length(&s->rlayer);
        RECORD_LAYER_clear_first_record(&s->rlayer);
    } while (num_recs < max_recs
             && rr[num_recs - 1].type == SSL3_RT_APPLICATION_DATA
             && SSL_USE_EXPLICIT_IV(s)
             && s->enc_read_ctx != NULL
             && (EVP_CIPHER_flags(EVP_CIPHER_CTX_cipher(s->enc_read_ctx))
                 & EVP_CIPH_FLAG_PIPELINE)
             && ssl3_record_app_data_waiting(s));

    /*
      * If in encrypt-then-mac mode calculate mac from encrypted record. All
      * the details below are public so no timing details can leak.
      */
    if (SSL_READ_ETM(s) && s->read_hash) {
         unsigned char *mac;
         mac_size = EVP_MD_CTX_size(s->read_hash);
         OPENSSL_assert(mac_size <= EVP_MAX_MD_SIZE);
        for (j = 0; j < num_recs; j++) {
            if (rr[j].length < mac_size) {
                al = SSL_AD_DECODE_ERROR;
                SSLerr(SSL_F_SSL3_GET_RECORD, SSL_R_LENGTH_TOO_SHORT);
                goto f_err;
            }
            rr[j].length -= mac_size;
            mac = rr[j].data + rr[j].length;
            i = s->method->ssl3_enc->mac(s, &rr[j], md, 0 /* not send */ );
            if (i < 0 || CRYPTO_memcmp(md, mac, (size_t)mac_size) != 0) {
                al = SSL_AD_BAD_RECORD_MAC;
                SSLerr(SSL_F_SSL3_GET_RECORD,
                       SSL_R_DECRYPTION_FAILED_OR_BAD_RECORD_MAC);
                goto f_err;
            }
        }
    }

    enc_err = s->method->ssl3_enc->enc(s, rr, num_recs, 0);
    /*-
     * enc_err is:
     *    0: (in non-constant time) if the record is publically invalid.
     *    1: if the padding is valid
     *    -1: if the padding is invalid
     */
    if (enc_err == 0) {
        al = SSL_AD_DECRYPTION_FAILED;
        SSLerr(SSL_F_SSL3_GET_RECORD, SSL_R_BLOCK_CIPHER_PAD_IS_WRONG);
        goto f_err;
    }
#ifdef SSL_DEBUG
    printf("dec %d\n", rr->length);
    {
        unsigned int z;
        for (z = 0; z < rr->length; z++)
            printf("%02X%c", rr->data[z], ((z + 1) % 16) ? ' ' : '\n');
    }
    printf("\n");
#endif

     /* r->length is now the compressed data plus mac */
     if ((sess != NULL) &&
         (s->enc_read_ctx != NULL) &&
        (!SSL_READ_ETM(s) && EVP_MD_CTX_md(s->read_hash) != NULL)) {
         /* s->read_hash != NULL => mac_size != -1 */
         unsigned char *mac = NULL;
         unsigned char mac_tmp[EVP_MAX_MD_SIZE];

        mac_size = EVP_MD_CTX_size(s->read_hash);
        OPENSSL_assert(mac_size <= EVP_MAX_MD_SIZE);

        for (j = 0; j < num_recs; j++) {
            /*
             * orig_len is the length of the record before any padding was
             * removed. This is public information, as is the MAC in use,
             * therefore we can safely process the record in a different amount
             * of time if it's too short to possibly contain a MAC.
             */
            if (rr[j].orig_len < mac_size ||
                /* CBC records must have a padding length byte too. */
                (EVP_CIPHER_CTX_mode(s->enc_read_ctx) == EVP_CIPH_CBC_MODE &&
                 rr[j].orig_len < mac_size + 1)) {
                al = SSL_AD_DECODE_ERROR;
                SSLerr(SSL_F_SSL3_GET_RECORD, SSL_R_LENGTH_TOO_SHORT);
                goto f_err;
            }

            if (EVP_CIPHER_CTX_mode(s->enc_read_ctx) == EVP_CIPH_CBC_MODE) {
                /*
                 * We update the length so that the TLS header bytes can be
                 * constructed correctly but we need to extract the MAC in
                 * constant time from within the record, without leaking the
                 * contents of the padding bytes.
                 */
                mac = mac_tmp;
                ssl3_cbc_copy_mac(mac_tmp, &rr[j], mac_size);
                rr[j].length -= mac_size;
            } else {
                /*
                 * In this case there's no padding, so |rec->orig_len| equals
                 * |rec->length| and we checked that there's enough bytes for
                 * |mac_size| above.
                 */
                rr[j].length -= mac_size;
                mac = &rr[j].data[rr[j].length];
            }

            i = s->method->ssl3_enc->mac(s, &rr[j], md, 0 /* not send */ );
            if (i < 0 || mac == NULL
                || CRYPTO_memcmp(md, mac, (size_t)mac_size) != 0)
                enc_err = -1;
            if (rr->length > SSL3_RT_MAX_COMPRESSED_LENGTH + mac_size)
                enc_err = -1;
        }
    }

    if (enc_err < 0) {
        /*
         * A separate 'decryption_failed' alert was introduced with TLS 1.0,
         * SSL 3.0 only has 'bad_record_mac'.  But unless a decryption
         * failure is directly visible from the ciphertext anyway, we should
         * not reveal which kind of error occurred -- this might become
         * visible to an attacker (e.g. via a logfile)
         */
        al = SSL_AD_BAD_RECORD_MAC;
        SSLerr(SSL_F_SSL3_GET_RECORD,
               SSL_R_DECRYPTION_FAILED_OR_BAD_RECORD_MAC);
        goto f_err;
    }

    for (j = 0; j < num_recs; j++) {
        /* rr[j].length is now just compressed */
        if (s->expand != NULL) {
            if (rr[j].length > SSL3_RT_MAX_COMPRESSED_LENGTH) {
                al = SSL_AD_RECORD_OVERFLOW;
                SSLerr(SSL_F_SSL3_GET_RECORD, SSL_R_COMPRESSED_LENGTH_TOO_LONG);
                goto f_err;
            }
            if (!ssl3_do_uncompress(s, &rr[j])) {
                al = SSL_AD_DECOMPRESSION_FAILURE;
                SSLerr(SSL_F_SSL3_GET_RECORD, SSL_R_BAD_DECOMPRESSION);
                goto f_err;
            }
        }

        if (rr[j].length > SSL3_RT_MAX_PLAIN_LENGTH) {
            al = SSL_AD_RECORD_OVERFLOW;
            SSLerr(SSL_F_SSL3_GET_RECORD, SSL_R_DATA_LENGTH_TOO_LONG);
            goto f_err;
        }

        rr[j].off = 0;
        /*-
         * So at this point the following is true
         * rr[j].type   is the type of record
         * rr[j].length == number of bytes in record
         * rr[j].off    == offset to first valid byte
         * rr[j].data   == where to take bytes from, increment after use :-).
         */

        /* just read a 0 length packet */
        if (rr[j].length == 0) {
            RECORD_LAYER_inc_empty_record_count(&s->rlayer);
            if (RECORD_LAYER_get_empty_record_count(&s->rlayer)
                > MAX_EMPTY_RECORDS) {
                al = SSL_AD_UNEXPECTED_MESSAGE;
                SSLerr(SSL_F_SSL3_GET_RECORD, SSL_R_RECORD_TOO_SMALL);
                goto f_err;
            }
        } else {
            RECORD_LAYER_reset_empty_record_count(&s->rlayer);
        }
    }

    RECORD_LAYER_set_numrpipes(&s->rlayer, num_recs);
    return 1;

 f_err:
    ssl3_send_alert(s, SSL3_AL_FATAL, al);
 err:
    return ret;
}
