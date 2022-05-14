static int ecdsa_sign_restartable( mbedtls_ecp_group *grp,
                 mbedtls_mpi *r, mbedtls_mpi *s,
                 const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                 int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
                 mbedtls_ecdsa_restart_ctx *rs_ctx )
 {
     int ret, key_tries, sign_tries;
    int *p_sign_tries = &sign_tries, *p_key_tries = &key_tries;
    mbedtls_ecp_point R;
    mbedtls_mpi k, e, t;
    mbedtls_mpi *pk = &k, *pr = r;

    /* Fail cleanly on curves such as Curve25519 that can't be used for ECDSA */
    if( grp->N.p == NULL )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    /* Make sure d is in range 1..n-1 */
    if( mbedtls_mpi_cmp_int( d, 1 ) < 0 || mbedtls_mpi_cmp_mpi( d, &grp->N ) >= 0 )
        return( MBEDTLS_ERR_ECP_INVALID_KEY );

    mbedtls_ecp_point_init( &R );
    mbedtls_mpi_init( &k ); mbedtls_mpi_init( &e ); mbedtls_mpi_init( &t );

    ECDSA_RS_ENTER( sig );

#if defined(MBEDTLS_ECP_RESTARTABLE)
    if( rs_ctx != NULL && rs_ctx->sig != NULL )
    {
        /* redirect to our context */
        p_sign_tries = &rs_ctx->sig->sign_tries;
        p_key_tries = &rs_ctx->sig->key_tries;
        pk = &rs_ctx->sig->k;
        pr = &rs_ctx->sig->r;

        /* jump to current step */
        if( rs_ctx->sig->state == ecdsa_sig_mul )
            goto mul;
        if( rs_ctx->sig->state == ecdsa_sig_modn )
            goto modn;
    }
#endif /* MBEDTLS_ECP_RESTARTABLE */

    *p_sign_tries = 0;
    do
    {
        if( *p_sign_tries++ > 10 )
        {
            ret = MBEDTLS_ERR_ECP_RANDOM_FAILED;
            goto cleanup;
        }

        /*
         * Steps 1-3: generate a suitable ephemeral keypair
         * and set r = xR mod n
         */
        *p_key_tries = 0;
        do
        {
            if( *p_key_tries++ > 10 )
            {
                ret = MBEDTLS_ERR_ECP_RANDOM_FAILED;
                goto cleanup;
            }

            MBEDTLS_MPI_CHK( mbedtls_ecp_gen_privkey( grp, pk, f_rng, p_rng ) );

#if defined(MBEDTLS_ECP_RESTARTABLE)
            if( rs_ctx != NULL && rs_ctx->sig != NULL )
                rs_ctx->sig->state = ecdsa_sig_mul;

 mul:
 #endif
             MBEDTLS_MPI_CHK( mbedtls_ecp_mul_restartable( grp, &R, pk, &grp->G,
                                                  f_rng, p_rng, ECDSA_RS_ECP ) );
             MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( pr, &R.X, &grp->N ) );
         }
         while( mbedtls_mpi_cmp_int( pr, 0 ) == 0 );

#if defined(MBEDTLS_ECP_RESTARTABLE)
        if( rs_ctx != NULL && rs_ctx->sig != NULL )
            rs_ctx->sig->state = ecdsa_sig_modn;

modn:
#endif
        /*
         * Accounting for everything up to the end of the loop
         * (step 6, but checking now avoids saving e and t)
         */
        ECDSA_BUDGET( MBEDTLS_ECP_OPS_INV + 4 );

        /*
         * Step 5: derive MPI from hashed message
         */
        MBEDTLS_MPI_CHK( derive_mpi( grp, &e, buf, blen ) );

        /*
          * Generate a random value to blind inv_mod in next step,
          * avoiding a potential timing leak.
          */
        MBEDTLS_MPI_CHK( mbedtls_ecp_gen_privkey( grp, &t, f_rng, p_rng ) );
 
         /*
          * Step 6: compute s = (e + r * d) / k = t (e + rd) / (kt) mod n
         */
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( s, pr, d ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_add_mpi( &e, &e, s ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &e, &e, &t ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( pk, pk, &t ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_inv_mod( s, pk, &grp->N ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( s, s, &e ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( s, s, &grp->N ) );
    }
    while( mbedtls_mpi_cmp_int( s, 0 ) == 0 );

#if defined(MBEDTLS_ECP_RESTARTABLE)
    if( rs_ctx != NULL && rs_ctx->sig != NULL )
        mbedtls_mpi_copy( r, pr );
#endif

cleanup:
    mbedtls_ecp_point_free( &R );
    mbedtls_mpi_free( &k ); mbedtls_mpi_free( &e ); mbedtls_mpi_free( &t );

    ECDSA_RS_LEAVE( sig );

    return( ret );
}
