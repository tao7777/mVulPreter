 void dtls1_free(SSL *s)
 {
     DTLS_RECORD_LAYER_free(&s->rlayer);
{
    pqueue *buffered_messages;
    pqueue *sent_messages;
    unsigned int mtu;
    unsigned int link_mtu;

    DTLS_RECORD_LAYER_clear(&s->rlayer);

    if (s->d1) {
        buffered_messages = s->d1->buffered_messages;
        sent_messages = s->d1->sent_messages;
        mtu = s->d1->mtu;
        link_mtu = s->d1->link_mtu;

        dtls1_clear_queues(s);

        memset(s->d1, 0, sizeof(*s->d1));

        if (s->server) {
            s->d1->cookie_len = sizeof(s->d1->cookie);
        }

        if (SSL_get_options(s) & SSL_OP_NO_QUERY_MTU) {
            s->d1->mtu = mtu;
            s->d1->link_mtu = link_mtu;
        }

        s->d1->buffered_messages = buffered_messages;
        s->d1->sent_messages = sent_messages;
    }

    ssl3_clear(s);

    if (s->method->version == DTLS_ANY_VERSION)
        s->version = DTLS_MAX_VERSION;
#ifndef OPENSSL_NO_DTLS1_METHOD
    else if (s->options & SSL_OP_CISCO_ANYCONNECT)
        s->client_version = s->version = DTLS1_BAD_VER;
#endif
    else
        s->version = s->method->version;
}

long dtls1_ctrl(SSL *s, int cmd, long larg, void *parg)
{
    int ret = 0;

    switch (cmd) {
    case DTLS_CTRL_GET_TIMEOUT:
        if (dtls1_get_timeout(s, (struct timeval *)parg) != NULL) {
            ret = 1;
        }
        break;
    case DTLS_CTRL_HANDLE_TIMEOUT:
        ret = dtls1_handle_timeout(s);
        break;
    case DTLS_CTRL_SET_LINK_MTU:
        if (larg < (long)dtls1_link_min_mtu())
            return 0;
        s->d1->link_mtu = larg;
        return 1;
    case DTLS_CTRL_GET_LINK_MIN_MTU:
        return (long)dtls1_link_min_mtu();
    case SSL_CTRL_SET_MTU:
        /*
         *  We may not have a BIO set yet so can't call dtls1_min_mtu()
         *  We'll have to make do with dtls1_link_min_mtu() and max overhead
         */
        if (larg < (long)dtls1_link_min_mtu() - DTLS1_MAX_MTU_OVERHEAD)
            return 0;
        s->d1->mtu = larg;
        return larg;
    default:
        ret = ssl3_ctrl(s, cmd, larg, parg);
        break;
    }
    return (ret);
}

void dtls1_start_timer(SSL *s)
{
#ifndef OPENSSL_NO_SCTP
    /* Disable timer for SCTP */
    if (BIO_dgram_is_sctp(SSL_get_wbio(s))) {
        memset(&s->d1->next_timeout, 0, sizeof(s->d1->next_timeout));
        return;
    }
#endif

    /* If timer is not set, initialize duration with 1 second */
    if (s->d1->next_timeout.tv_sec == 0 && s->d1->next_timeout.tv_usec == 0) {
        s->d1->timeout_duration = 1;
    }

    /* Set timeout to current time */
    get_current_time(&(s->d1->next_timeout));

    /* Add duration to current time */
    s->d1->next_timeout.tv_sec += s->d1->timeout_duration;
    BIO_ctrl(SSL_get_rbio(s), BIO_CTRL_DGRAM_SET_NEXT_TIMEOUT, 0,
             &(s->d1->next_timeout));
}

struct timeval *dtls1_get_timeout(SSL *s, struct timeval *timeleft)
{
    struct timeval timenow;

    /* If no timeout is set, just return NULL */
    if (s->d1->next_timeout.tv_sec == 0 && s->d1->next_timeout.tv_usec == 0) {
        return NULL;
    }

    /* Get current time */
    get_current_time(&timenow);

    /* If timer already expired, set remaining time to 0 */
    if (s->d1->next_timeout.tv_sec < timenow.tv_sec ||
        (s->d1->next_timeout.tv_sec == timenow.tv_sec &&
         s->d1->next_timeout.tv_usec <= timenow.tv_usec)) {
        memset(timeleft, 0, sizeof(*timeleft));
        return timeleft;
    }

    /* Calculate time left until timer expires */
    memcpy(timeleft, &(s->d1->next_timeout), sizeof(struct timeval));
    timeleft->tv_sec -= timenow.tv_sec;
    timeleft->tv_usec -= timenow.tv_usec;
    if (timeleft->tv_usec < 0) {
        timeleft->tv_sec--;
        timeleft->tv_usec += 1000000;
    }

    /*
     * If remaining time is less than 15 ms, set it to 0 to prevent issues
     * because of small divergences with socket timeouts.
     */
    if (timeleft->tv_sec == 0 && timeleft->tv_usec < 15000) {
        memset(timeleft, 0, sizeof(*timeleft));
    }

    return timeleft;
}

int dtls1_is_timer_expired(SSL *s)
{
    struct timeval timeleft;

    /* Get time left until timeout, return false if no timer running */
    if (dtls1_get_timeout(s, &timeleft) == NULL) {
        return 0;
    }

    /* Return false if timer is not expired yet */
    if (timeleft.tv_sec > 0 || timeleft.tv_usec > 0) {
        return 0;
    }

    /* Timer expired, so return true */
    return 1;
}

void dtls1_double_timeout(SSL *s)
{
    s->d1->timeout_duration *= 2;
    if (s->d1->timeout_duration > 60)
        s->d1->timeout_duration = 60;
    dtls1_start_timer(s);
}

void dtls1_stop_timer(SSL *s)
{
    /* Reset everything */
    memset(&s->d1->timeout, 0, sizeof(s->d1->timeout));
    memset(&s->d1->next_timeout, 0, sizeof(s->d1->next_timeout));
    s->d1->timeout_duration = 1;
    BIO_ctrl(SSL_get_rbio(s), BIO_CTRL_DGRAM_SET_NEXT_TIMEOUT, 0,
             &(s->d1->next_timeout));
    /* Clear retransmission buffer */
    dtls1_clear_record_buffer(s);
}

int dtls1_check_timeout_num(SSL *s)
{
    unsigned int mtu;

    s->d1->timeout.num_alerts++;

    /* Reduce MTU after 2 unsuccessful retransmissions */
     BIO_ctrl(SSL_get_rbio(s), BIO_CTRL_DGRAM_SET_NEXT_TIMEOUT, 0,
              &(s->d1->next_timeout));
     /* Clear retransmission buffer */
    dtls1_clear_sent_buffer(s);
 }
 
 int dtls1_check_timeout_num(SSL *s)

    if (s->d1->timeout.num_alerts > DTLS1_TMO_ALERT_COUNT) {
        /* fail the connection, enough alerts have been sent */
        SSLerr(SSL_F_DTLS1_CHECK_TIMEOUT_NUM, SSL_R_READ_TIMEOUT_EXPIRED);
        return -1;
    }

    return 0;
}
