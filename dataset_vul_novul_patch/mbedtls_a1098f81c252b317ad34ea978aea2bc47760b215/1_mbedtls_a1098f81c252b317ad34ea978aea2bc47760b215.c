static int ssl_parse_server_key_exchange( mbedtls_ssl_context *ssl )
{
    int ret;
    const mbedtls_ssl_ciphersuite_t *ciphersuite_info =
        ssl->transform_negotiate->ciphersuite_info;
    unsigned char *p = NULL, *end = NULL;

    MBEDTLS_SSL_DEBUG_MSG( 2, ( "=> parse server key exchange" ) );

#if defined(MBEDTLS_KEY_EXCHANGE_RSA_ENABLED)
    if( ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_RSA )
    {
        MBEDTLS_SSL_DEBUG_MSG( 2, ( "<= skip parse server key exchange" ) );
        ssl->state++;
        return( 0 );
    }
    ((void) p);
    ((void) end);
#endif

#if defined(MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED) || \
    defined(MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED)
    if( ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_ECDH_RSA ||
        ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA )
    {
        if( ( ret = ssl_get_ecdh_params_from_cert( ssl ) ) != 0 )
        {
            MBEDTLS_SSL_DEBUG_RET( 1, "ssl_get_ecdh_params_from_cert", ret );
            mbedtls_ssl_send_alert_message( ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                            MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE );
            return( ret );
        }

        MBEDTLS_SSL_DEBUG_MSG( 2, ( "<= skip parse server key exchange" ) );
        ssl->state++;
        return( 0 );
    }
    ((void) p);
    ((void) end);
#endif /* MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED ||
          MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED */

    if( ( ret = mbedtls_ssl_read_record( ssl ) ) != 0 )
    {
        MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_ssl_read_record", ret );
        return( ret );
    }

    if( ssl->in_msgtype != MBEDTLS_SSL_MSG_HANDSHAKE )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "bad server key exchange message" ) );
        mbedtls_ssl_send_alert_message( ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                        MBEDTLS_SSL_ALERT_MSG_UNEXPECTED_MESSAGE );
        return( MBEDTLS_ERR_SSL_UNEXPECTED_MESSAGE );
    }

    /*
     * ServerKeyExchange may be skipped with PSK and RSA-PSK when the server
     * doesn't use a psk_identity_hint
     */
    if( ssl->in_msg[0] != MBEDTLS_SSL_HS_SERVER_KEY_EXCHANGE )
    {
        if( ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_PSK ||
            ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_RSA_PSK )
        {
            /* Current message is probably either
             * CertificateRequest or ServerHelloDone */
            ssl->keep_current_message = 1;
            goto exit;
        }

        MBEDTLS_SSL_DEBUG_MSG( 1, ( "server key exchange message must "
                                    "not be skipped" ) );
        mbedtls_ssl_send_alert_message( ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                        MBEDTLS_SSL_ALERT_MSG_UNEXPECTED_MESSAGE );

        return( MBEDTLS_ERR_SSL_UNEXPECTED_MESSAGE );
    }

    p   = ssl->in_msg + mbedtls_ssl_hs_hdr_len( ssl );
    end = ssl->in_msg + ssl->in_hslen;
    MBEDTLS_SSL_DEBUG_BUF( 3,   "server key exchange", p, end - p );

#if defined(MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED)
    if( ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_PSK ||
        ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_RSA_PSK ||
        ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_DHE_PSK ||
        ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_ECDHE_PSK )
    {
        if( ssl_parse_server_psk_hint( ssl, &p, end ) != 0 )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "bad server key exchange message" ) );
            mbedtls_ssl_send_alert_message( ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                            MBEDTLS_SSL_ALERT_MSG_ILLEGAL_PARAMETER );
            return( MBEDTLS_ERR_SSL_BAD_HS_SERVER_KEY_EXCHANGE );
        }
    } /* FALLTROUGH */
#endif /* MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED */

#if defined(MBEDTLS_KEY_EXCHANGE_PSK_ENABLED) ||                       \
    defined(MBEDTLS_KEY_EXCHANGE_RSA_PSK_ENABLED)
    if( ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_PSK ||
        ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_RSA_PSK )
        ; /* nothing more to do */
    else
#endif /* MBEDTLS_KEY_EXCHANGE_PSK_ENABLED ||
          MBEDTLS_KEY_EXCHANGE_RSA_PSK_ENABLED */
#if defined(MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED) ||                       \
    defined(MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED)
    if( ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_DHE_RSA ||
        ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_DHE_PSK )
    {
        if( ssl_parse_server_dh_params( ssl, &p, end ) != 0 )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "bad server key exchange message" ) );
            mbedtls_ssl_send_alert_message( ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                            MBEDTLS_SSL_ALERT_MSG_ILLEGAL_PARAMETER );
            return( MBEDTLS_ERR_SSL_BAD_HS_SERVER_KEY_EXCHANGE );
        }
    }
    else
#endif /* MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED ||
          MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED */
#if defined(MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED) ||                     \
    defined(MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED) ||                     \
    defined(MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED)
    if( ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_ECDHE_RSA ||
        ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_ECDHE_PSK ||
        ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA )
    {
        if( ssl_parse_server_ecdh_params( ssl, &p, end ) != 0 )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "bad server key exchange message" ) );
            mbedtls_ssl_send_alert_message( ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                            MBEDTLS_SSL_ALERT_MSG_ILLEGAL_PARAMETER );
            return( MBEDTLS_ERR_SSL_BAD_HS_SERVER_KEY_EXCHANGE );
        }
    }
    else
#endif /* MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED ||
          MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED ||
          MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED */
#if defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
    if( ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_ECJPAKE )
    {
        ret = mbedtls_ecjpake_read_round_two( &ssl->handshake->ecjpake_ctx,
                                              p, end - p );
        if( ret != 0 )
        {
            MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_ecjpake_read_round_two", ret );
            mbedtls_ssl_send_alert_message( ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                            MBEDTLS_SSL_ALERT_MSG_ILLEGAL_PARAMETER );
            return( MBEDTLS_ERR_SSL_BAD_HS_SERVER_KEY_EXCHANGE );
        }
    }
    else
#endif /* MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED */
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
        return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
    }

#if defined(MBEDTLS_KEY_EXCHANGE__WITH_SERVER_SIGNATURE__ENABLED)
    if( mbedtls_ssl_ciphersuite_uses_server_signature( ciphersuite_info ) )
    {
        size_t sig_len, hashlen;
        unsigned char hash[64];
        mbedtls_md_type_t md_alg = MBEDTLS_MD_NONE;
        mbedtls_pk_type_t pk_alg = MBEDTLS_PK_NONE;
        unsigned char *params = ssl->in_msg + mbedtls_ssl_hs_hdr_len( ssl );
        size_t params_len = p - params;

        /*
         * Handle the digitally-signed structure
         */
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
        if( ssl->minor_ver == MBEDTLS_SSL_MINOR_VERSION_3 )
        {
            if( ssl_parse_signature_algorithm( ssl, &p, end,
                                               &md_alg, &pk_alg ) != 0 )
            {
                MBEDTLS_SSL_DEBUG_MSG( 1, ( "bad server key exchange message" ) );
                mbedtls_ssl_send_alert_message( ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                                MBEDTLS_SSL_ALERT_MSG_ILLEGAL_PARAMETER );
                return( MBEDTLS_ERR_SSL_BAD_HS_SERVER_KEY_EXCHANGE );
            }

            if( pk_alg != mbedtls_ssl_get_ciphersuite_sig_pk_alg( ciphersuite_info ) )
            {
                MBEDTLS_SSL_DEBUG_MSG( 1, ( "bad server key exchange message" ) );
                mbedtls_ssl_send_alert_message( ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                                MBEDTLS_SSL_ALERT_MSG_ILLEGAL_PARAMETER );
                return( MBEDTLS_ERR_SSL_BAD_HS_SERVER_KEY_EXCHANGE );
            }
        }
        else
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */
#if defined(MBEDTLS_SSL_PROTO_SSL3) || defined(MBEDTLS_SSL_PROTO_TLS1) || \
    defined(MBEDTLS_SSL_PROTO_TLS1_1)
        if( ssl->minor_ver < MBEDTLS_SSL_MINOR_VERSION_3 )
        {
            pk_alg = mbedtls_ssl_get_ciphersuite_sig_pk_alg( ciphersuite_info );

            /* Default hash for ECDSA is SHA-1 */
            if( pk_alg == MBEDTLS_PK_ECDSA && md_alg == MBEDTLS_MD_NONE )
                md_alg = MBEDTLS_MD_SHA1;
        }
        else
#endif
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
        }

         /*
          * Read signature
          */
         sig_len = ( p[0] << 8 ) | p[1];
         p += 2;
 
        if( end != p + sig_len )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "bad server key exchange message" ) );
            mbedtls_ssl_send_alert_message( ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                            MBEDTLS_SSL_ALERT_MSG_DECODE_ERROR );
            return( MBEDTLS_ERR_SSL_BAD_HS_SERVER_KEY_EXCHANGE );
        }

        MBEDTLS_SSL_DEBUG_BUF( 3, "signature", p, sig_len );

        /*
         * Compute the hash that has been signed
         */
#if defined(MBEDTLS_SSL_PROTO_SSL3) || defined(MBEDTLS_SSL_PROTO_TLS1) || \
    defined(MBEDTLS_SSL_PROTO_TLS1_1)
        if( md_alg == MBEDTLS_MD_NONE )
        {
            hashlen = 36;
            ret = mbedtls_ssl_get_key_exchange_md_ssl_tls( ssl, hash, params,
                                                           params_len );
            if( ret != 0 )
                return( ret );
        }
        else
#endif /* MBEDTLS_SSL_PROTO_SSL3 || MBEDTLS_SSL_PROTO_TLS1 || \
          MBEDTLS_SSL_PROTO_TLS1_1 */
#if defined(MBEDTLS_SSL_PROTO_TLS1) || defined(MBEDTLS_SSL_PROTO_TLS1_1) || \
    defined(MBEDTLS_SSL_PROTO_TLS1_2)
        if( md_alg != MBEDTLS_MD_NONE )
        {
            /* Info from md_alg will be used instead */
            hashlen = 0;
            ret = mbedtls_ssl_get_key_exchange_md_tls1_2( ssl, hash, params,
                                                          params_len, md_alg );
            if( ret != 0 )
                return( ret );
        }
        else
#endif /* MBEDTLS_SSL_PROTO_TLS1 || MBEDTLS_SSL_PROTO_TLS1_1 || \
          MBEDTLS_SSL_PROTO_TLS1_2 */
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
        }

        MBEDTLS_SSL_DEBUG_BUF( 3, "parameters hash", hash, hashlen != 0 ? hashlen :
            (unsigned int) ( mbedtls_md_get_size( mbedtls_md_info_from_type( md_alg ) ) ) );

        if( ssl->session_negotiate->peer_cert == NULL )
        {
            MBEDTLS_SSL_DEBUG_MSG( 2, ( "certificate required" ) );
            mbedtls_ssl_send_alert_message( ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                            MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE );
            return( MBEDTLS_ERR_SSL_UNEXPECTED_MESSAGE );
        }

        /*
         * Verify signature
         */
        if( ! mbedtls_pk_can_do( &ssl->session_negotiate->peer_cert->pk, pk_alg ) )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "bad server key exchange message" ) );
            mbedtls_ssl_send_alert_message( ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                            MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE );
            return( MBEDTLS_ERR_SSL_PK_TYPE_MISMATCH );
        }

        if( ( ret = mbedtls_pk_verify( &ssl->session_negotiate->peer_cert->pk,
                               md_alg, hash, hashlen, p, sig_len ) ) != 0 )
        {
            mbedtls_ssl_send_alert_message( ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                            MBEDTLS_SSL_ALERT_MSG_DECRYPT_ERROR );
            MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_pk_verify", ret );
            return( ret );
        }
    }
#endif /* MBEDTLS_KEY_EXCHANGE__WITH_SERVER_SIGNATURE__ENABLED */

exit:
    ssl->state++;

    MBEDTLS_SSL_DEBUG_MSG( 2, ( "<= parse server key exchange" ) );

    return( 0 );
}
