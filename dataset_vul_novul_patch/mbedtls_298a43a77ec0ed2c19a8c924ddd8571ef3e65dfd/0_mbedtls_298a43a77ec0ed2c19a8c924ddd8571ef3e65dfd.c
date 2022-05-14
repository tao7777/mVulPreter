int mbedtls_ecdsa_sign( mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
static int ecdsa_sign_internal( mbedtls_ecp_group *grp, mbedtls_mpi *r,
                                mbedtls_mpi *s, const mbedtls_mpi *d,
                                const unsigned char *buf, size_t blen,
                                int (*f_rng)(void *, unsigned char *, size_t),
                                void *p_rng,
                                int (*f_rng_blind)(void *, unsigned char *,
                                                   size_t),
                                void *p_rng_blind )
 {
     int ret, key_tries, sign_tries, blind_tries;
     mbedtls_ecp_point R;
    mbedtls_mpi k, e, t;

    /* Fail cleanly on curves such as Curve25519 that can't be used for ECDSA */
    if( grp->N.p == NULL )
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );

    /* Make sure d is in range 1..n-1 */
    if( mbedtls_mpi_cmp_int( d, 1 ) < 0 || mbedtls_mpi_cmp_mpi( d, &grp->N ) >= 0 )
        return( MBEDTLS_ERR_ECP_INVALID_KEY );

    mbedtls_ecp_point_init( &R );
    mbedtls_mpi_init( &k ); mbedtls_mpi_init( &e ); mbedtls_mpi_init( &t );

    sign_tries = 0;
    do
    {
        /*
         * Steps 1-3: generate a suitable ephemeral keypair
         * and set r = xR mod n
         */
         key_tries = 0;
         do
         {
            MBEDTLS_MPI_CHK( mbedtls_ecp_gen_privkey( grp, &k, f_rng, p_rng ) );

            MBEDTLS_MPI_CHK( mbedtls_ecp_mul( grp, &R, &k, &grp->G,
                                              f_rng_blind, p_rng_blind ) );
             MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( r, &R.X, &grp->N ) );
 
             if( key_tries++ > 10 )
            {
                ret = MBEDTLS_ERR_ECP_RANDOM_FAILED;
                goto cleanup;
            }
        }
        while( mbedtls_mpi_cmp_int( r, 0 ) == 0 );

        /*
         * Step 5: derive MPI from hashed message
         */
        MBEDTLS_MPI_CHK( derive_mpi( grp, &e, buf, blen ) );

         /*
          * Generate a random value to blind inv_mod in next step,
          * avoiding a potential timing leak.
         *
         * This loop does the same job as mbedtls_ecp_gen_privkey() and it is
         * replaced by a call to it in the mainline. This change is not
         * necessary to backport the fix separating the blinding and ephemeral
         * key generating RNGs, therefore the original code is kept.
          */
         blind_tries = 0;
         do
         {
             size_t n_size = ( grp->nbits + 7 ) / 8;
            MBEDTLS_MPI_CHK( mbedtls_mpi_fill_random( &t, n_size, f_rng_blind,
                                                      p_rng_blind ) );
             MBEDTLS_MPI_CHK( mbedtls_mpi_shift_r( &t, 8 * n_size - grp->nbits ) );
 
             if( ++blind_tries > 30 )
                 return( MBEDTLS_ERR_ECP_RANDOM_FAILED );
         }
        while( mbedtls_mpi_cmp_int( &t, 1 ) < 0 ||
               mbedtls_mpi_cmp_mpi( &t, &grp->N ) >= 0 );

        /*
         * Step 6: compute s = (e + r * d) / k = t (e + rd) / (kt) mod n
         */
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( s, r, d ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_add_mpi( &e, &e, s ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &e, &e, &t ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &k, &k, &t ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_inv_mod( s, &k, &grp->N ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( s, s, &e ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( s, s, &grp->N ) );

        if( sign_tries++ > 10 )
        {
            ret = MBEDTLS_ERR_ECP_RANDOM_FAILED;
            goto cleanup;
        }
    }
    while( mbedtls_mpi_cmp_int( s, 0 ) == 0 );

cleanup:
    mbedtls_ecp_point_free( &R );
    mbedtls_mpi_free( &k ); mbedtls_mpi_free( &e ); mbedtls_mpi_free( &t );
 
     return( ret );
 }
