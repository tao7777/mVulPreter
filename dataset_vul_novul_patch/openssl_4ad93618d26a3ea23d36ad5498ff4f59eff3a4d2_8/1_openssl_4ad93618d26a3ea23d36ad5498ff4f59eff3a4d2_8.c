static int ssl_scan_serverhello_tlsext(SSL *s, PACKET *pkt, int *al)
{
    unsigned int length, type, size;
    int tlsext_servername = 0;
    int renegotiate_seen = 0;

#ifndef OPENSSL_NO_NEXTPROTONEG
    s->s3->next_proto_neg_seen = 0;
#endif
    s->tlsext_ticket_expected = 0;

    OPENSSL_free(s->s3->alpn_selected);
    s->s3->alpn_selected = NULL;
#ifndef OPENSSL_NO_HEARTBEATS
    s->tlsext_heartbeat &= ~(SSL_DTLSEXT_HB_ENABLED |
                              SSL_DTLSEXT_HB_DONT_SEND_REQUESTS);
 #endif
 
    s->s3->flags &= ~TLS1_FLAGS_ENCRYPT_THEN_MAC;
 
     s->s3->flags &= ~TLS1_FLAGS_RECEIVED_EXTMS;
 
    if (!PACKET_get_net_2(pkt, &length))
        goto ri_check;

    if (PACKET_remaining(pkt) != length) {
        *al = SSL_AD_DECODE_ERROR;
        return 0;
    }

    if (!tls1_check_duplicate_extensions(pkt)) {
        *al = SSL_AD_DECODE_ERROR;
        return 0;
    }

    while (PACKET_get_net_2(pkt, &type) && PACKET_get_net_2(pkt, &size)) {
        const unsigned char *data;
        PACKET spkt;

        if (!PACKET_get_sub_packet(pkt, &spkt, size)
            || !PACKET_peek_bytes(&spkt, &data, size))
            goto ri_check;

        if (s->tlsext_debug_cb)
            s->tlsext_debug_cb(s, 1, type, data, size, s->tlsext_debug_arg);

        if (type == TLSEXT_TYPE_renegotiate) {
            if (!ssl_parse_serverhello_renegotiate_ext(s, &spkt, al))
                return 0;
            renegotiate_seen = 1;
        } else if (s->version == SSL3_VERSION) {
        } else if (type == TLSEXT_TYPE_server_name) {
            if (s->tlsext_hostname == NULL || size > 0) {
                *al = TLS1_AD_UNRECOGNIZED_NAME;
                return 0;
            }
            tlsext_servername = 1;
        }
#ifndef OPENSSL_NO_EC
        else if (type == TLSEXT_TYPE_ec_point_formats) {
            unsigned int ecpointformatlist_length;
            if (!PACKET_get_1(&spkt, &ecpointformatlist_length)
                || ecpointformatlist_length != size - 1) {
                *al = TLS1_AD_DECODE_ERROR;
                return 0;
            }
            if (!s->hit) {
                s->session->tlsext_ecpointformatlist_length = 0;
                OPENSSL_free(s->session->tlsext_ecpointformatlist);
                if ((s->session->tlsext_ecpointformatlist =
                     OPENSSL_malloc(ecpointformatlist_length)) == NULL) {
                    *al = TLS1_AD_INTERNAL_ERROR;
                    return 0;
                }
                s->session->tlsext_ecpointformatlist_length =
                    ecpointformatlist_length;
                if (!PACKET_copy_bytes(&spkt,
                                       s->session->tlsext_ecpointformatlist,
                                       ecpointformatlist_length)) {
                    *al = TLS1_AD_DECODE_ERROR;
                    return 0;
                }

            }
        }
#endif                          /* OPENSSL_NO_EC */

        else if (type == TLSEXT_TYPE_session_ticket) {
            if (s->tls_session_ticket_ext_cb &&
                !s->tls_session_ticket_ext_cb(s, data, size,
                                              s->tls_session_ticket_ext_cb_arg))
            {
                *al = TLS1_AD_INTERNAL_ERROR;
                return 0;
            }
            if (!tls_use_ticket(s) || (size > 0)) {
                *al = TLS1_AD_UNSUPPORTED_EXTENSION;
                return 0;
            }
            s->tlsext_ticket_expected = 1;
        } else if (type == TLSEXT_TYPE_status_request) {
            /*
             * MUST be empty and only sent if we've requested a status
             * request message.
             */
            if ((s->tlsext_status_type == -1) || (size > 0)) {
                *al = TLS1_AD_UNSUPPORTED_EXTENSION;
                return 0;
            }
            /* Set flag to expect CertificateStatus message */
            s->tlsext_status_expected = 1;
        }
#ifndef OPENSSL_NO_CT
        /*
         * Only take it if we asked for it - i.e if there is no CT validation
         * callback set, then a custom extension MAY be processing it, so we
         * need to let control continue to flow to that.
         */
        else if (type == TLSEXT_TYPE_signed_certificate_timestamp &&
                 s->ct_validation_callback != NULL) {
            /* Simply copy it off for later processing */
            if (s->tlsext_scts != NULL) {
                OPENSSL_free(s->tlsext_scts);
                s->tlsext_scts = NULL;
            }
            s->tlsext_scts_len = size;
            if (size > 0) {
                s->tlsext_scts = OPENSSL_malloc(size);
                if (s->tlsext_scts == NULL) {
                    *al = TLS1_AD_INTERNAL_ERROR;
                    return 0;
                }
                memcpy(s->tlsext_scts, data, size);
            }
        }
#endif
#ifndef OPENSSL_NO_NEXTPROTONEG
        else if (type == TLSEXT_TYPE_next_proto_neg &&
                 s->s3->tmp.finish_md_len == 0) {
            unsigned char *selected;
            unsigned char selected_len;
            /* We must have requested it. */
            if (s->ctx->next_proto_select_cb == NULL) {
                *al = TLS1_AD_UNSUPPORTED_EXTENSION;
                return 0;
            }
            /* The data must be valid */
            if (!ssl_next_proto_validate(&spkt)) {
                *al = TLS1_AD_DECODE_ERROR;
                return 0;
            }
            if (s->ctx->next_proto_select_cb(s, &selected, &selected_len, data,
                                             size,
                                             s->
                                             ctx->next_proto_select_cb_arg) !=
                SSL_TLSEXT_ERR_OK) {
                *al = TLS1_AD_INTERNAL_ERROR;
                return 0;
            }
            /*
             * Could be non-NULL if server has sent multiple NPN extensions in
             * a single Serverhello
             */
            OPENSSL_free(s->next_proto_negotiated);
            s->next_proto_negotiated = OPENSSL_malloc(selected_len);
            if (s->next_proto_negotiated == NULL) {
                *al = TLS1_AD_INTERNAL_ERROR;
                return 0;
            }
            memcpy(s->next_proto_negotiated, selected, selected_len);
            s->next_proto_negotiated_len = selected_len;
            s->s3->next_proto_neg_seen = 1;
        }
#endif

        else if (type == TLSEXT_TYPE_application_layer_protocol_negotiation) {
            unsigned len;
            /* We must have requested it. */
            if (!s->s3->alpn_sent) {
                *al = TLS1_AD_UNSUPPORTED_EXTENSION;
                return 0;
            }
            /*-
             * The extension data consists of:
             *   uint16 list_length
             *   uint8 proto_length;
             *   uint8 proto[proto_length];
             */
            if (!PACKET_get_net_2(&spkt, &len)
                || PACKET_remaining(&spkt) != len || !PACKET_get_1(&spkt, &len)
                || PACKET_remaining(&spkt) != len) {
                *al = TLS1_AD_DECODE_ERROR;
                return 0;
            }
            OPENSSL_free(s->s3->alpn_selected);
            s->s3->alpn_selected = OPENSSL_malloc(len);
            if (s->s3->alpn_selected == NULL) {
                *al = TLS1_AD_INTERNAL_ERROR;
                return 0;
            }
            if (!PACKET_copy_bytes(&spkt, s->s3->alpn_selected, len)) {
                *al = TLS1_AD_DECODE_ERROR;
                return 0;
            }
            s->s3->alpn_selected_len = len;
        }
#ifndef OPENSSL_NO_HEARTBEATS
        else if (SSL_IS_DTLS(s) && type == TLSEXT_TYPE_heartbeat) {
            unsigned int hbtype;
            if (!PACKET_get_1(&spkt, &hbtype)) {
                *al = SSL_AD_DECODE_ERROR;
                return 0;
            }
            switch (hbtype) {
            case 0x01:         /* Server allows us to send HB requests */
                s->tlsext_heartbeat |= SSL_DTLSEXT_HB_ENABLED;
                break;
            case 0x02:         /* Server doesn't accept HB requests */
                s->tlsext_heartbeat |= SSL_DTLSEXT_HB_ENABLED;
                s->tlsext_heartbeat |= SSL_DTLSEXT_HB_DONT_SEND_REQUESTS;
                break;
            default:
                *al = SSL_AD_ILLEGAL_PARAMETER;
                return 0;
            }
        }
#endif
#ifndef OPENSSL_NO_SRTP
        else if (SSL_IS_DTLS(s) && type == TLSEXT_TYPE_use_srtp) {
            if (ssl_parse_serverhello_use_srtp_ext(s, &spkt, al))
                return 0;
        }
#endif
        else if (type == TLSEXT_TYPE_encrypt_then_mac) {
             /* Ignore if inappropriate ciphersuite */
             if (s->s3->tmp.new_cipher->algorithm_mac != SSL_AEAD
                 && s->s3->tmp.new_cipher->algorithm_enc != SSL_RC4)
                s->s3->flags |= TLS1_FLAGS_ENCRYPT_THEN_MAC;
         } else if (type == TLSEXT_TYPE_extended_master_secret) {
             s->s3->flags |= TLS1_FLAGS_RECEIVED_EXTMS;
             if (!s->hit)
                s->session->flags |= SSL_SESS_FLAG_EXTMS;
        }
        /*
         * If this extension type was not otherwise handled, but matches a
         * custom_cli_ext_record, then send it to the c callback
         */
        else if (custom_ext_parse(s, 0, type, data, size, al) <= 0)
            return 0;
    }

    if (PACKET_remaining(pkt) != 0) {
        *al = SSL_AD_DECODE_ERROR;
        return 0;
    }

    if (!s->hit && tlsext_servername == 1) {
        if (s->tlsext_hostname) {
            if (s->session->tlsext_hostname == NULL) {
                s->session->tlsext_hostname =
                    OPENSSL_strdup(s->tlsext_hostname);
                if (!s->session->tlsext_hostname) {
                    *al = SSL_AD_UNRECOGNIZED_NAME;
                    return 0;
                }
            } else {
                *al = SSL_AD_DECODE_ERROR;
                return 0;
            }
        }
    }

 ri_check:

    /*
     * Determine if we need to see RI. Strictly speaking if we want to avoid
     * an attack we should *always* see RI even on initial server hello
     * because the client doesn't see any renegotiation during an attack.
     * However this would mean we could not connect to any server which
     * doesn't support RI so for the immediate future tolerate RI absence
     */
    if (!renegotiate_seen && !(s->options & SSL_OP_LEGACY_SERVER_CONNECT)
        && !(s->options & SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION)) {
        *al = SSL_AD_HANDSHAKE_FAILURE;
        SSLerr(SSL_F_SSL_SCAN_SERVERHELLO_TLSEXT,
               SSL_R_UNSAFE_LEGACY_RENEGOTIATION_DISABLED);
        return 0;
    }

    if (s->hit) {
        /*
         * Check extended master secret extension is consistent with
         * original session.
         */
        if (!(s->s3->flags & TLS1_FLAGS_RECEIVED_EXTMS) !=
            !(s->session->flags & SSL_SESS_FLAG_EXTMS)) {
            *al = SSL_AD_HANDSHAKE_FAILURE;
            SSLerr(SSL_F_SSL_SCAN_SERVERHELLO_TLSEXT, SSL_R_INCONSISTENT_EXTMS);
            return 0;
        }
    }

    return 1;
}
