int mbedtls_ecdsa_sign_det( mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
static int ecdsa_sign_det_internal( mbedtls_ecp_group *grp, mbedtls_mpi *r,
                                    mbedtls_mpi *s, const mbedtls_mpi *d,
                                    const unsigned char *buf, size_t blen,
                                    mbedtls_md_type_t md_alg,
                                    int (*f_rng_blind)(void *, unsigned char *,
                                                       size_t),
                                    void *p_rng_blind )
 {
     int ret;
     mbedtls_hmac_drbg_context rng_ctx;
     unsigned char data[2 * MBEDTLS_ECP_MAX_BYTES];
     size_t grp_len = ( grp->nbits + 7 ) / 8;
     const mbedtls_md_info_t *md_info;
     mbedtls_mpi h;
    /* Variables for deterministic blinding fallback */
    const char* blind_label = "BLINDING CONTEXT";
    mbedtls_hmac_drbg_context rng_ctx_blind;
 
     if( ( md_info = mbedtls_md_info_from_type( md_alg ) ) == NULL )
         return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
 
     mbedtls_mpi_init( &h );
     mbedtls_hmac_drbg_init( &rng_ctx );
    mbedtls_hmac_drbg_init( &rng_ctx_blind );
 
     /* Use private key and message hash (reduced) to initialize HMAC_DRBG */
     MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( d, data, grp_len ) );
     MBEDTLS_MPI_CHK( derive_mpi( grp, &h, buf, blen ) );
     MBEDTLS_MPI_CHK( mbedtls_mpi_write_binary( &h, data + grp_len, grp_len ) );
     mbedtls_hmac_drbg_seed_buf( &rng_ctx, md_info, data, 2 * grp_len );
 
    if( f_rng_blind != NULL )
        ret = ecdsa_sign_internal( grp, r, s, d, buf, blen,
                                   mbedtls_hmac_drbg_random, &rng_ctx,
                                   f_rng_blind, p_rng_blind );
    else
    {
        /*
         * To avoid reusing rng_ctx and risking incorrect behavior we seed a
         * second HMAC-DRBG with the same seed. We also apply a label to avoid
         * reusing the bits of the ephemeral key for blinding and eliminate the
         * risk that they leak this way.
         */

        mbedtls_hmac_drbg_seed_buf( &rng_ctx_blind, md_info,
                                    data, 2 * grp_len );
        ret = mbedtls_hmac_drbg_update_ret( &rng_ctx_blind,
                                            (const unsigned char*) blind_label,
                                            strlen( blind_label ) );
        if( ret != 0 )
            goto cleanup;

        /*
         * Since the output of the RNGs is always the same for the same key and
         * message, this limits the efficiency of blinding and leaks information
         * through side channels. After mbedtls_ecdsa_sign_det() is removed NULL
         * won't be a valid value for f_rng_blind anymore. Therefore it should
         * be checked by the caller and this branch and check can be removed.
         */
        ret = ecdsa_sign_internal( grp, r, s, d, buf, blen,
                                   mbedtls_hmac_drbg_random, &rng_ctx,
                                   mbedtls_hmac_drbg_random, &rng_ctx_blind );

    }
 
 cleanup:
     mbedtls_hmac_drbg_free( &rng_ctx );
    mbedtls_hmac_drbg_free( &rng_ctx_blind );
     mbedtls_mpi_free( &h );
 
     return( ret );
 }
