int dtls1_read_bytes(SSL *s, int type, int *recvd_type, unsigned char *buf,
                     int len, int peek)
{
    int al, i, j, ret;
    unsigned int n;
    SSL3_RECORD *rr;
    void (*cb) (const SSL *ssl, int type2, int val) = NULL;

    if (!SSL3_BUFFER_is_initialised(&s->rlayer.rbuf)) {
        /* Not initialized yet */
        if (!ssl3_setup_buffers(s))
            return (-1);
    }

    if ((type && (type != SSL3_RT_APPLICATION_DATA) &&
         (type != SSL3_RT_HANDSHAKE)) ||
        (peek && (type != SSL3_RT_APPLICATION_DATA))) {
        SSLerr(SSL_F_DTLS1_READ_BYTES, ERR_R_INTERNAL_ERROR);
        return -1;
    }

    /*
     * check whether there's a handshake message (client hello?) waiting
     */
    if ((ret = have_handshake_fragment(s, type, buf, len)))
        return ret;

    /*
     * Now s->rlayer.d->handshake_fragment_len == 0 if
     * type == SSL3_RT_HANDSHAKE.
     */

#ifndef OPENSSL_NO_SCTP
    /*
     * Continue handshake if it had to be interrupted to read app data with
     * SCTP.
     */
    if ((!ossl_statem_get_in_handshake(s) && SSL_in_init(s)) ||
        (BIO_dgram_is_sctp(SSL_get_rbio(s))
         && ossl_statem_in_sctp_read_sock(s)
         && s->s3->in_read_app_data != 2))
#else
    if (!ossl_statem_get_in_handshake(s) && SSL_in_init(s))
#endif
    {
        /* type == SSL3_RT_APPLICATION_DATA */
        i = s->handshake_func(s);
        if (i < 0)
            return (i);
        if (i == 0) {
            SSLerr(SSL_F_DTLS1_READ_BYTES, SSL_R_SSL_HANDSHAKE_FAILURE);
            return (-1);
        }
    }

 start:
    s->rwstate = SSL_NOTHING;

    /*-
     * s->s3->rrec.type         - is the type of record
     * s->s3->rrec.data,    - data
     * s->s3->rrec.off,     - offset into 'data' for next read
     * s->s3->rrec.length,  - number of bytes.
     */
    rr = s->rlayer.rrec;

    /*
     * We are not handshaking and have no data yet, so process data buffered
     * during the last handshake in advance, if any.
     */
    if (SSL_is_init_finished(s) && SSL3_RECORD_get_length(rr) == 0) {
        pitem *item;
        item = pqueue_pop(s->rlayer.d->buffered_app_data.q);
        if (item) {
#ifndef OPENSSL_NO_SCTP
            /* Restore bio_dgram_sctp_rcvinfo struct */
            if (BIO_dgram_is_sctp(SSL_get_rbio(s))) {
                DTLS1_RECORD_DATA *rdata = (DTLS1_RECORD_DATA *)item->data;
                BIO_ctrl(SSL_get_rbio(s), BIO_CTRL_DGRAM_SCTP_SET_RCVINFO,
                         sizeof(rdata->recordinfo), &rdata->recordinfo);
            }
#endif

            dtls1_copy_record(s, item);

            OPENSSL_free(item->data);
            pitem_free(item);
        }
    }

    /* Check for timeout */
    if (dtls1_handle_timeout(s) > 0)
        goto start;

    /* get new packet if necessary */
    if ((SSL3_RECORD_get_length(rr) == 0)
        || (s->rlayer.rstate == SSL_ST_READ_BODY)) {
        ret = dtls1_get_record(s);
        if (ret <= 0) {
            ret = dtls1_read_failed(s, ret);
            /* anything other than a timeout is an error */
            if (ret <= 0)
                return (ret);
            else
                goto start;
         }
     }
 
    /*
     * Reset the count of consecutive warning alerts if we've got a non-empty
     * record that isn't an alert.
     */
    if (SSL3_RECORD_get_type(rr) != SSL3_RT_ALERT
            && SSL3_RECORD_get_length(rr) != 0)
        s->rlayer.alert_count = 0;

     /* we now have a packet which can be read and processed */
 
     if (s->s3->change_cipher_spec /* set when we receive ChangeCipherSpec,
                                SSL3_RECORD_get_seq_num(rr)) < 0) {
            SSLerr(SSL_F_DTLS1_READ_BYTES, ERR_R_INTERNAL_ERROR);
            return -1;
        }
        SSL3_RECORD_set_length(rr, 0);
        goto start;
    }
