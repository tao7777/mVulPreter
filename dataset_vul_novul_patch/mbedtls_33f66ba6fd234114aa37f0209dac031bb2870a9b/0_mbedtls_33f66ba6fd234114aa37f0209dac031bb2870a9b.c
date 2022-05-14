static int ecdsa_sign_det_restartable( mbedtls_ecp_group *grp,
                     mbedtls_mpi *r, mbedtls_mpi *s,
                     const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                     mbedtls_md_type_t md_alg,
                    int (*f_rng_blind)(void *, unsigned char *, size_t),
                    void *p_rng_blind,
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
    if( f_rng_blind != NULL )
        ret = ecdsa_sign_restartable( grp, r, s, d, buf, blen,
                                      mbedtls_hmac_drbg_random, p_rng,
                                      f_rng_blind, p_rng_blind, rs_ctx );
    else
    {
        mbedtls_hmac_drbg_context *p_rng_blind_det;

#if !defined(MBEDTLS_ECP_RESTARTABLE)
        /*
         * To avoid reusing rng_ctx and risking incorrect behavior we seed a
         * second HMAC-DRBG with the same seed. We also apply a label to avoid
         * reusing the bits of the ephemeral key for blinding and eliminate the
         * risk that they leak this way.
         */
        const char* blind_label = "BLINDING CONTEXT";
        mbedtls_hmac_drbg_context rng_ctx_blind;

        mbedtls_hmac_drbg_init( &rng_ctx_blind );
        p_rng_blind_det = &rng_ctx_blind;

        mbedtls_hmac_drbg_seed_buf( p_rng_blind_det, md_info,
                                    data, 2 * grp_len );
        ret = mbedtls_hmac_drbg_update_ret( p_rng_blind_det,
                                            (const unsigned char*) blind_label,
                                            strlen( blind_label ) );
        if( ret != 0 )
        {
            mbedtls_hmac_drbg_free( &rng_ctx_blind );
            goto cleanup;
        }
#else
        /*
         * In the case of restartable computations we would either need to store
         * the second RNG in the restart context too or set it up at every
         * restart. The first option would penalize the correct application of
         * the function and the second would defeat the purpose of the
         * restartable feature.
         *
         * Therefore in this case we reuse the original RNG. This comes with the
         * price that the resulting signature might not be a valid deterministic
         * ECDSA signature with a very low probability (same magnitude as
         * successfully guessing the private key). However even then it is still
         * a valid ECDSA signature.
         */
        p_rng_blind_det = p_rng;
#endif /* MBEDTLS_ECP_RESTARTABLE */

        /*
         * Since the output of the RNGs is always the same for the same key and
         * message, this limits the efficiency of blinding and leaks information
         * through side channels. After mbedtls_ecdsa_sign_det() is removed NULL
         * won't be a valid value for f_rng_blind anymore. Therefore it should
         * be checked by the caller and this branch and check can be removed.
         */
        ret = ecdsa_sign_restartable( grp, r, s, d, buf, blen,
                                      mbedtls_hmac_drbg_random, p_rng,
                                      mbedtls_hmac_drbg_random, p_rng_blind_det,
                                      rs_ctx );

#if !defined(MBEDTLS_ECP_RESTARTABLE)
        mbedtls_hmac_drbg_free( &rng_ctx_blind );
#endif
    }
 #endif /* MBEDTLS_ECDSA_SIGN_ALT */
 
 cleanup:
    mbedtls_hmac_drbg_free( &rng_ctx );
    mbedtls_mpi_free( &h );

    ECDSA_RS_LEAVE( det );

    return( ret );
 }
