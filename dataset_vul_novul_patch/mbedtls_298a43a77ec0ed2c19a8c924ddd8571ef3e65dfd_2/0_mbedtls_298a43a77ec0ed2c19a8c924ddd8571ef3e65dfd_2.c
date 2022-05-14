int mbedtls_ecp_gen_keypair_base( mbedtls_ecp_group *grp,
int mbedtls_ecp_gen_privkey( const mbedtls_ecp_group *grp,
                     mbedtls_mpi *d,
                      int (*f_rng)(void *, unsigned char *, size_t),
                      void *p_rng )
 {
    int ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
     size_t n_size = ( grp->nbits + 7 ) / 8;
 
 #if defined(ECP_MONTGOMERY)
    if( ecp_get_type( grp ) == ECP_TYPE_MONTGOMERY )
    {
        /* [M225] page 5 */
        size_t b;

        do {
            MBEDTLS_MPI_CHK( mbedtls_mpi_fill_random( d, n_size, f_rng, p_rng ) );
        } while( mbedtls_mpi_bitlen( d ) == 0);

        /* Make sure the most significant bit is nbits */
        b = mbedtls_mpi_bitlen( d ) - 1; /* mbedtls_mpi_bitlen is one-based */
        if( b > grp->nbits )
            MBEDTLS_MPI_CHK( mbedtls_mpi_shift_r( d, b - grp->nbits ) );
        else
            MBEDTLS_MPI_CHK( mbedtls_mpi_set_bit( d, grp->nbits, 1 ) );

        /* Make sure the last three bits are unset */
        MBEDTLS_MPI_CHK( mbedtls_mpi_set_bit( d, 0, 0 ) );
         MBEDTLS_MPI_CHK( mbedtls_mpi_set_bit( d, 1, 0 ) );
         MBEDTLS_MPI_CHK( mbedtls_mpi_set_bit( d, 2, 0 ) );
     }
 #endif /* ECP_MONTGOMERY */

 #if defined(ECP_SHORTWEIERSTRASS)
     if( ecp_get_type( grp ) == ECP_TYPE_SHORT_WEIERSTRASS )
     {
        /* SEC1 3.2.1: Generate d such that 1 <= n < N */
        int count = 0;

        /*
         * Match the procedure given in RFC 6979 (deterministic ECDSA):
         * - use the same byte ordering;
         * - keep the leftmost nbits bits of the generated octet string;
         * - try until result is in the desired range.
         * This also avoids any biais, which is especially important for ECDSA.
         */
        do
        {
            MBEDTLS_MPI_CHK( mbedtls_mpi_fill_random( d, n_size, f_rng, p_rng ) );
            MBEDTLS_MPI_CHK( mbedtls_mpi_shift_r( d, 8 * n_size - grp->nbits ) );

            /*
             * Each try has at worst a probability 1/2 of failing (the msb has
             * a probability 1/2 of being 0, and then the result will be < N),
             * so after 30 tries failure probability is a most 2**(-30).
             *
             * For most curves, 1 try is enough with overwhelming probability,
             * since N starts with a lot of 1s in binary, but some curves
             * such as secp224k1 are actually very close to the worst case.
             */
            if( ++count > 30 )
                return( MBEDTLS_ERR_ECP_RANDOM_FAILED );
        }
         while( mbedtls_mpi_cmp_int( d, 1 ) < 0 ||
                mbedtls_mpi_cmp_mpi( d, &grp->N ) >= 0 );
     }
 #endif /* ECP_SHORTWEIERSTRASS */
 
 cleanup:
    return( ret );
}
 
/*
 * Generate a keypair with configurable base point
 */
int mbedtls_ecp_gen_keypair_base( mbedtls_ecp_group *grp,
                     const mbedtls_ecp_point *G,
                     mbedtls_mpi *d, mbedtls_ecp_point *Q,
                     int (*f_rng)(void *, unsigned char *, size_t),
                     void *p_rng )
{
    int ret;

    MBEDTLS_MPI_CHK( mbedtls_ecp_gen_privkey( grp, d, f_rng, p_rng ) );
    MBEDTLS_MPI_CHK( mbedtls_ecp_mul( grp, Q, d, G, f_rng, p_rng ) );

cleanup:
    return( ret );
 }
