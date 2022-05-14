int mbedtls_ecdsa_sign( mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{
    ECDSA_VALIDATE_RET( grp   != NULL );
    ECDSA_VALIDATE_RET( r     != NULL );
    ECDSA_VALIDATE_RET( s     != NULL );
    ECDSA_VALIDATE_RET( d     != NULL );
     ECDSA_VALIDATE_RET( f_rng != NULL );
     ECDSA_VALIDATE_RET( buf   != NULL || blen == 0 );
 
    /* Use the same RNG for both blinding and ephemeral key generation */
     return( ecdsa_sign_restartable( grp, r, s, d, buf, blen,
                                    f_rng, p_rng, f_rng, p_rng, NULL ) );
 }
