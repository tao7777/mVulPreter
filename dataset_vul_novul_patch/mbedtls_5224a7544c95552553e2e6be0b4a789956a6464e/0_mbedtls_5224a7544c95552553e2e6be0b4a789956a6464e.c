static int ssl_parse_server_psk_hint( mbedtls_ssl_context *ssl,
                                      unsigned char **p,
                                      unsigned char *end )
{
    int ret = MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE;
    size_t  len;
    ((void) ssl);

    /*
     * PSK parameters:
     *
     * opaque psk_identity_hint<0..2^16-1>;
     */
    if( (*p) > end - 2 )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "bad server key exchange message "
                                    "(psk_identity_hint length)" ) );
        return( MBEDTLS_ERR_SSL_BAD_HS_SERVER_KEY_EXCHANGE );
    }
     len = (*p)[0] << 8 | (*p)[1];
     *p += 2;
 
    if( (*p) > end - len )
     {
         MBEDTLS_SSL_DEBUG_MSG( 1, ( "bad server key exchange message "
                                     "(psk_identity_hint length)" ) );
        return( MBEDTLS_ERR_SSL_BAD_HS_SERVER_KEY_EXCHANGE );
    }

    /*
     * Note: we currently ignore the PKS identity hint, as we only allow one
     * PSK to be provisionned on the client. This could be changed later if
     * someone needs that feature.
     */
    *p += len;
    ret = 0;

    return( ret );
}
