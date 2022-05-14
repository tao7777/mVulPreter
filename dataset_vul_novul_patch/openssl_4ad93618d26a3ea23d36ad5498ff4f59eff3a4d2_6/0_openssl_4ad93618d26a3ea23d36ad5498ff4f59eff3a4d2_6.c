unsigned char *ssl_add_serverhello_tlsext(SSL *s, unsigned char *buf,
                                          unsigned char *limit, int *al)
{
    int extdatalen = 0;
    unsigned char *orig = buf;
    unsigned char *ret = buf;
#ifndef OPENSSL_NO_NEXTPROTONEG
    int next_proto_neg_seen;
#endif
#ifndef OPENSSL_NO_EC
    unsigned long alg_k = s->s3->tmp.new_cipher->algorithm_mkey;
    unsigned long alg_a = s->s3->tmp.new_cipher->algorithm_auth;
    int using_ecc = (alg_k & SSL_kECDHE) || (alg_a & SSL_aECDSA);
    using_ecc = using_ecc && (s->session->tlsext_ecpointformatlist != NULL);
#endif

    ret += 2;
    if (ret >= limit)
        return NULL;            /* this really never occurs, but ... */

    if (s->s3->send_connection_binding) {
        int el;

        if (!ssl_add_serverhello_renegotiate_ext(s, 0, &el, 0)) {
            SSLerr(SSL_F_SSL_ADD_SERVERHELLO_TLSEXT, ERR_R_INTERNAL_ERROR);
            return NULL;
        }

        /*-
         * check for enough space.
         * 4 bytes for the reneg type and extension length
         * + reneg data length
         */
        if (CHECKLEN(ret, 4 + el, limit))
            return NULL;

        s2n(TLSEXT_TYPE_renegotiate, ret);
        s2n(el, ret);

        if (!ssl_add_serverhello_renegotiate_ext(s, ret, &el, el)) {
            SSLerr(SSL_F_SSL_ADD_SERVERHELLO_TLSEXT, ERR_R_INTERNAL_ERROR);
            return NULL;
        }

        ret += el;
    }

    /* Only add RI for SSLv3 */
    if (s->version == SSL3_VERSION)
        goto done;

    if (!s->hit && s->servername_done == 1
        && s->session->tlsext_hostname != NULL) {
        /*-
         * check for enough space.
         * 4 bytes for the server name type and extension length
         */
        if (CHECKLEN(ret, 4, limit))
            return NULL;

        s2n(TLSEXT_TYPE_server_name, ret);
        s2n(0, ret);
    }
#ifndef OPENSSL_NO_EC
    if (using_ecc) {
        const unsigned char *plist;
        size_t plistlen;
        /*
         * Add TLS extension ECPointFormats to the ServerHello message
         */

        tls1_get_formatlist(s, &plist, &plistlen);

        if (plistlen > 255) {
            SSLerr(SSL_F_SSL_ADD_SERVERHELLO_TLSEXT, ERR_R_INTERNAL_ERROR);
            return NULL;
        }

        /*-
         * check for enough space.
         * 4 bytes for the ec points format type and extension length
         * 1 byte for the points format list length
         * + length of points format list
         */
        if (CHECKLEN(ret, 5 + plistlen, limit))
            return NULL;

        s2n(TLSEXT_TYPE_ec_point_formats, ret);
        s2n(plistlen + 1, ret);
        *(ret++) = (unsigned char)plistlen;
        memcpy(ret, plist, plistlen);
        ret += plistlen;

    }
    /*
     * Currently the server should not respond with a SupportedCurves
     * extension
     */
#endif                          /* OPENSSL_NO_EC */

    if (s->tlsext_ticket_expected && tls_use_ticket(s)) {
        /*-
         * check for enough space.
         * 4 bytes for the Ticket type and extension length
         */
        if (CHECKLEN(ret, 4, limit))
            return NULL;
        s2n(TLSEXT_TYPE_session_ticket, ret);
        s2n(0, ret);
    } else {
        /*
         * if we don't add the above TLSEXT, we can't add a session ticket
         * later
         */
        s->tlsext_ticket_expected = 0;
    }

    if (s->tlsext_status_expected) {
        /*-
         * check for enough space.
         * 4 bytes for the Status request type and extension length
         */
        if (CHECKLEN(ret, 4, limit))
            return NULL;
        s2n(TLSEXT_TYPE_status_request, ret);
        s2n(0, ret);
    }
#ifndef OPENSSL_NO_SRTP
    if (SSL_IS_DTLS(s) && s->srtp_profile) {
        int el;

        /* Returns 0 on success!! */
        if (ssl_add_serverhello_use_srtp_ext(s, 0, &el, 0)) {
            SSLerr(SSL_F_SSL_ADD_SERVERHELLO_TLSEXT, ERR_R_INTERNAL_ERROR);
            return NULL;
        }
        /*-
         * check for enough space.
         * 4 bytes for the SRTP profiles type and extension length
         * + length of the SRTP profiles list
         */
        if (CHECKLEN(ret, 4 + el, limit))
            return NULL;

        s2n(TLSEXT_TYPE_use_srtp, ret);
        s2n(el, ret);

        if (ssl_add_serverhello_use_srtp_ext(s, ret, &el, el)) {
            SSLerr(SSL_F_SSL_ADD_SERVERHELLO_TLSEXT, ERR_R_INTERNAL_ERROR);
            return NULL;
        }
        ret += el;
    }
#endif

    if (((s->s3->tmp.new_cipher->id & 0xFFFF) == 0x80
         || (s->s3->tmp.new_cipher->id & 0xFFFF) == 0x81)
        && (SSL_get_options(s) & SSL_OP_CRYPTOPRO_TLSEXT_BUG)) {
        const unsigned char cryptopro_ext[36] = {
            0xfd, 0xe8,         /* 65000 */
            0x00, 0x20,         /* 32 bytes length */
            0x30, 0x1e, 0x30, 0x08, 0x06, 0x06, 0x2a, 0x85,
            0x03, 0x02, 0x02, 0x09, 0x30, 0x08, 0x06, 0x06,
            0x2a, 0x85, 0x03, 0x02, 0x02, 0x16, 0x30, 0x08,
            0x06, 0x06, 0x2a, 0x85, 0x03, 0x02, 0x02, 0x17
        };

        /* check for enough space. */
        if (CHECKLEN(ret, sizeof(cryptopro_ext), limit))
            return NULL;
        memcpy(ret, cryptopro_ext, sizeof(cryptopro_ext));
        ret += sizeof(cryptopro_ext);

    }
#ifndef OPENSSL_NO_HEARTBEATS
    /* Add Heartbeat extension if we've received one */
    if (SSL_IS_DTLS(s) && (s->tlsext_heartbeat & SSL_DTLSEXT_HB_ENABLED)) {
        /*-
         * check for enough space.
         * 4 bytes for the Heartbeat type and extension length
         * 1 byte for the mode
         */
        if (CHECKLEN(ret, 5, limit))
            return NULL;
        s2n(TLSEXT_TYPE_heartbeat, ret);
        s2n(1, ret);
        /*-
         * Set mode:
         * 1: peer may send requests
         * 2: peer not allowed to send requests
         */
        if (s->tlsext_heartbeat & SSL_DTLSEXT_HB_DONT_RECV_REQUESTS)
            *(ret++) = SSL_DTLSEXT_HB_DONT_SEND_REQUESTS;
        else
            *(ret++) = SSL_DTLSEXT_HB_ENABLED;

    }
#endif

#ifndef OPENSSL_NO_NEXTPROTONEG
    next_proto_neg_seen = s->s3->next_proto_neg_seen;
    s->s3->next_proto_neg_seen = 0;
    if (next_proto_neg_seen && s->ctx->next_protos_advertised_cb) {
        const unsigned char *npa;
        unsigned int npalen;
        int r;

        r = s->ctx->next_protos_advertised_cb(s, &npa, &npalen,
                                              s->
                                              ctx->next_protos_advertised_cb_arg);
        if (r == SSL_TLSEXT_ERR_OK) {
            /*-
             * check for enough space.
             * 4 bytes for the NPN type and extension length
             * + length of protocols list
             */
            if (CHECKLEN(ret, 4 + npalen, limit))
                return NULL;
            s2n(TLSEXT_TYPE_next_proto_neg, ret);
            s2n(npalen, ret);
            memcpy(ret, npa, npalen);
            ret += npalen;
            s->s3->next_proto_neg_seen = 1;
        }
    }
 #endif
     if (!custom_ext_add(s, 1, &ret, limit, al))
         return NULL;
    if (s->tlsext_use_etm) {
         /*
          * Don't use encrypt_then_mac if AEAD or RC4 might want to disable
          * for other cases too.
         */
        if (SSL_IS_DTLS(s) || s->s3->tmp.new_cipher->algorithm_mac == SSL_AEAD
             || s->s3->tmp.new_cipher->algorithm_enc == SSL_RC4
             || s->s3->tmp.new_cipher->algorithm_enc == SSL_eGOST2814789CNT
             || s->s3->tmp.new_cipher->algorithm_enc == SSL_eGOST2814789CNT12)
            s->tlsext_use_etm = 0;
         else {
             /*-
              * check for enough space.
             * 4 bytes for the ETM type and extension length
             */
            if (CHECKLEN(ret, 4, limit))
                return NULL;
            s2n(TLSEXT_TYPE_encrypt_then_mac, ret);
            s2n(0, ret);
        }
    }
    if (s->s3->flags & TLS1_FLAGS_RECEIVED_EXTMS) {
        /*-
         * check for enough space.
         * 4 bytes for the EMS type and extension length
         */
        if (CHECKLEN(ret, 4, limit))
            return NULL;
        s2n(TLSEXT_TYPE_extended_master_secret, ret);
        s2n(0, ret);
    }

    if (s->s3->alpn_selected != NULL) {
        const unsigned char *selected = s->s3->alpn_selected;
        size_t len = s->s3->alpn_selected_len;

        /*-
         * check for enough space.
         * 4 bytes for the ALPN type and extension length
         * 2 bytes for ALPN data length
         * 1 byte for selected protocol length
         * + length of the selected protocol
         */
        if (CHECKLEN(ret, 7 + len, limit))
            return NULL;
        s2n(TLSEXT_TYPE_application_layer_protocol_negotiation, ret);
        s2n(3 + len, ret);
        s2n(1 + len, ret);
        *ret++ = len;
        memcpy(ret, selected, len);
        ret += len;
    }

 done:

    if ((extdatalen = ret - orig - 2) == 0)
        return orig;

    s2n(extdatalen, orig);
    return ret;
}
