static int ecdsa_sign_det_restartable( mbedtls_ecp_group *grp,
                     mbedtls_mpi *r, mbedtls_mpi *s,
                     const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                     mbedtls_md_type_t md_alg,
                     mbedtls_ecdsa_restart_ctx *rs_ctx )
 {
     int ret;
    mbedtls_hmac_drbg_context rng_ctx;
    mbedtls_hmac_drbg_context *p_rng = &rng_ctx;
    unsigned char data[2 * MBEDTLS_ECP_MAX_BYTES];
    size_t grp_len = ( grp->nbits + 7 ) / 8;
    const mbedtls_md_info_t *md_info;
    mbedtls_mpi h;

    if( ( md_info = mbedtls_md_info_from_type( md_alg ) ) == NULL )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    mbedtls_mpi_init( &h );
    mbedtls_hmac_drbg_init( &rng_ctx );

    ECDSA_RS_ENTER( det );

#if defined(MBEDTLS_ECP_RESTARTABLE)
    if( rs_ctx != NULL && rs_ctx->det != NULL )
    {
        /* redirect to our context */
        p_rng = &rs_ctx->det->rng_ctx;

        /* jump to current step */
        if( rs_ctx->det->state == ecdsa_det_sign )
            goto sign;
    }
#endif /* MBEDTLS_ECP_RESTARTABLE */

    /* Use private key and message hash (reduced) to initialize HMAC_DRBG */
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( d, data, grp_len ) );
    MBEDTLS_MPI_CHK( derive_mpi( grp, &h, buf, blen ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &h, data + grp_len, grp_len ) );
    mbedtls_hmac_drbg_seed_buf( p_rng, md_info, data, 2 * grp_len );

#if defined(MBEDTLS_ECP_RESTARTABLE)
    if( rs_ctx != NULL && rs_ctx->det != NULL )
        rs_ctx->det->state = ecdsa_det_sign;

sign:
#endif
#if defined(MBEDTLS_ECDSA_SIGN_ALT)
     ret = mbedtls_ecdsa_sign( grp, r, s, d, buf, blen,
                               mbedtls_hmac_drbg_random, p_rng );
 #else
    ret = ecdsa_sign_restartable( grp, r, s, d, buf, blen,
                      mbedtls_hmac_drbg_random, p_rng, rs_ctx );
 #endif /* MBEDTLS_ECDSA_SIGN_ALT */
 
 cleanup:
    mbedtls_hmac_drbg_free( &rng_ctx );
    mbedtls_mpi_free( &h );

    ECDSA_RS_LEAVE( det );

    return( ret );
 }
