WORK_STATE ossl_statem_server_pre_work(SSL *s, WORK_STATE wst)
{
    OSSL_STATEM *st = &s->statem;

    switch (st->hand_state) {
     case TLS_ST_SW_HELLO_REQ:
         s->shutdown = 0;
         if (SSL_IS_DTLS(s))
            dtls1_clear_sent_buffer(s);
         break;
 
     case DTLS_ST_SW_HELLO_VERIFY_REQUEST:
         s->shutdown = 0;
         if (SSL_IS_DTLS(s)) {
            dtls1_clear_sent_buffer(s);
             /* We don't buffer this message so don't use the timer */
             st->use_timer = 0;
         }
        break;

    case TLS_ST_SW_SRVR_HELLO:
        if (SSL_IS_DTLS(s)) {
            /*
             * Messages we write from now on should be bufferred and
             * retransmitted if necessary, so we need to use the timer now
             */
            st->use_timer = 1;
        }
        break;

    case TLS_ST_SW_SRVR_DONE:
#ifndef OPENSSL_NO_SCTP
        if (SSL_IS_DTLS(s) && BIO_dgram_is_sctp(SSL_get_wbio(s)))
            return dtls_wait_for_dry(s);
#endif
        return WORK_FINISHED_CONTINUE;

    case TLS_ST_SW_SESSION_TICKET:
        if (SSL_IS_DTLS(s)) {
            /*
             * We're into the last flight. We don't retransmit the last flight
             * unless we need to, so we don't use the timer
             */
            st->use_timer = 0;
        }
        break;

    case TLS_ST_SW_CHANGE:
        s->session->cipher = s->s3->tmp.new_cipher;
        if (!s->method->ssl3_enc->setup_key_block(s)) {
            ossl_statem_set_error(s);
            return WORK_ERROR;
        }
        if (SSL_IS_DTLS(s)) {
            /*
             * We're into the last flight. We don't retransmit the last flight
             * unless we need to, so we don't use the timer. This might have
             * already been set to 0 if we sent a NewSessionTicket message,
             * but we'll set it again here in case we didn't.
             */
            st->use_timer = 0;
        }
        return WORK_FINISHED_CONTINUE;

    case TLS_ST_OK:
        return tls_finish_handshake(s, wst);

    default:
        /* No pre work to be done */
        break;
    }

    return WORK_FINISHED_CONTINUE;
}
