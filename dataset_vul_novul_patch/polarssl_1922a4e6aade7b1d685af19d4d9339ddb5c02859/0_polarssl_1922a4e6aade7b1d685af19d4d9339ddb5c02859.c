int ssl_parse_certificate( ssl_context *ssl )
{
    int ret;
    size_t i, n;

    SSL_DEBUG_MSG( 2, ( "=> parse certificate" ) );

    if( ssl->endpoint == SSL_IS_SERVER &&
        ssl->authmode == SSL_VERIFY_NONE )
    {
        ssl->verify_result = BADCERT_SKIP_VERIFY;
        SSL_DEBUG_MSG( 2, ( "<= skip parse certificate" ) );
        ssl->state++;
        return( 0 );
    }

    if( ( ret = ssl_read_record( ssl ) ) != 0 )
    {
        SSL_DEBUG_RET( 1, "ssl_read_record", ret );
        return( ret );
    }

    ssl->state++;

    /*
     * Check if the client sent an empty certificate
     */
    if( ssl->endpoint  == SSL_IS_SERVER &&
        ssl->minor_ver == SSL_MINOR_VERSION_0 )
    {
        if( ssl->in_msglen  == 2                        &&
            ssl->in_msgtype == SSL_MSG_ALERT            &&
            ssl->in_msg[0]  == SSL_ALERT_LEVEL_WARNING  &&
            ssl->in_msg[1]  == SSL_ALERT_MSG_NO_CERT )
        {
            SSL_DEBUG_MSG( 1, ( "SSLv3 client has no certificate" ) );

            ssl->verify_result = BADCERT_MISSING;
            if( ssl->authmode == SSL_VERIFY_OPTIONAL )
                return( 0 );
            else
                return( POLARSSL_ERR_SSL_NO_CLIENT_CERTIFICATE );
        }
    }

    if( ssl->endpoint  == SSL_IS_SERVER &&
        ssl->minor_ver != SSL_MINOR_VERSION_0 )
    {
        if( ssl->in_hslen   == 7                    &&
            ssl->in_msgtype == SSL_MSG_HANDSHAKE    &&
            ssl->in_msg[0]  == SSL_HS_CERTIFICATE   &&
            memcmp( ssl->in_msg + 4, "\0\0\0", 3 ) == 0 )
        {
            SSL_DEBUG_MSG( 1, ( "TLSv1 client has no certificate" ) );

            ssl->verify_result = BADCERT_MISSING;
            if( ssl->authmode == SSL_VERIFY_REQUIRED )
                return( POLARSSL_ERR_SSL_NO_CLIENT_CERTIFICATE );
            else
                return( 0 );
        }
    }

    if( ssl->in_msgtype != SSL_MSG_HANDSHAKE )
    {
        SSL_DEBUG_MSG( 1, ( "bad certificate message" ) );
        return( POLARSSL_ERR_SSL_UNEXPECTED_MESSAGE );
    }

    if( ssl->in_msg[0] != SSL_HS_CERTIFICATE || ssl->in_hslen < 10 )
    {
        SSL_DEBUG_MSG( 1, ( "bad certificate message" ) );
        return( POLARSSL_ERR_SSL_BAD_HS_CERTIFICATE );
    }

    /*
     * Same message structure as in ssl_write_certificate()
     */
    n = ( ssl->in_msg[5] << 8 ) | ssl->in_msg[6];

    if( ssl->in_msg[4] != 0 || ssl->in_hslen != 7 + n )
    {
        SSL_DEBUG_MSG( 1, ( "bad certificate message" ) );
        return( POLARSSL_ERR_SSL_BAD_HS_CERTIFICATE );
    }

    if( ( ssl->session_negotiate->peer_cert = (x509_cert *) malloc(
                    sizeof( x509_cert ) ) ) == NULL )
    {
        SSL_DEBUG_MSG( 1, ( "malloc(%d bytes) failed",
                       sizeof( x509_cert ) ) );
        return( POLARSSL_ERR_SSL_MALLOC_FAILED );
    }

    memset( ssl->session_negotiate->peer_cert, 0, sizeof( x509_cert ) );

    i = 7;

    while( i < ssl->in_hslen )
    {
        if( ssl->in_msg[i] != 0 )
        {
            SSL_DEBUG_MSG( 1, ( "bad certificate message" ) );
            return( POLARSSL_ERR_SSL_BAD_HS_CERTIFICATE );
        }

        n = ( (unsigned int) ssl->in_msg[i + 1] << 8 )
            | (unsigned int) ssl->in_msg[i + 2];
        i += 3;

        if( n < 128 || i + n > ssl->in_hslen )
        {
            SSL_DEBUG_MSG( 1, ( "bad certificate message" ) );
             return( POLARSSL_ERR_SSL_BAD_HS_CERTIFICATE );
         }
 
        ret = x509parse_crt_der( ssl->session_negotiate->peer_cert,
                                 ssl->in_msg + i, n );
         if( ret != 0 )
         {
             SSL_DEBUG_RET( 1, " x509parse_crt", ret );
            return( ret );
        }

        i += n;
    }

    SSL_DEBUG_CRT( 3, "peer certificate", ssl->session_negotiate->peer_cert );

    if( ssl->authmode != SSL_VERIFY_NONE )
    {
        if( ssl->ca_chain == NULL )
        {
            SSL_DEBUG_MSG( 1, ( "got no CA chain" ) );
            return( POLARSSL_ERR_SSL_CA_CHAIN_REQUIRED );
        }

        ret = x509parse_verify( ssl->session_negotiate->peer_cert,
                                ssl->ca_chain, ssl->ca_crl,
                                ssl->peer_cn,  &ssl->verify_result,
                                ssl->f_vrfy, ssl->p_vrfy );

        if( ret != 0 )
            SSL_DEBUG_RET( 1, "x509_verify_cert", ret );

        if( ssl->authmode != SSL_VERIFY_REQUIRED )
            ret = 0;
    }

    SSL_DEBUG_MSG( 2, ( "<= parse certificate" ) );

    return( ret );
}
