int mbedtls_ecdsa_sign_det( mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
int mbedtls_ecdsa_sign_det( mbedtls_ecp_group *grp, mbedtls_mpi *r,
                            mbedtls_mpi *s, const mbedtls_mpi *d,
                            const unsigned char *buf, size_t blen,
                            mbedtls_md_type_t md_alg )
{
    ECDSA_VALIDATE_RET( grp   != NULL );
    ECDSA_VALIDATE_RET( r     != NULL );
    ECDSA_VALIDATE_RET( s     != NULL );
    ECDSA_VALIDATE_RET( d     != NULL );
    ECDSA_VALIDATE_RET( buf   != NULL || blen == 0 );

    return( ecdsa_sign_det_restartable( grp, r, s, d, buf, blen, md_alg,
                                        NULL, NULL, NULL ) );
}

int mbedtls_ecdsa_sign_det_ext( mbedtls_ecp_group *grp, mbedtls_mpi *r,
                                mbedtls_mpi *s, const mbedtls_mpi *d,
                                const unsigned char *buf, size_t blen,
                                mbedtls_md_type_t md_alg,
                                int (*f_rng_blind)(void *, unsigned char *,
                                                   size_t),
                                void *p_rng_blind )
 {
     ECDSA_VALIDATE_RET( grp   != NULL );
     ECDSA_VALIDATE_RET( r     != NULL );
     ECDSA_VALIDATE_RET( s     != NULL );
     ECDSA_VALIDATE_RET( d     != NULL );
     ECDSA_VALIDATE_RET( buf   != NULL || blen == 0 );
    ECDSA_VALIDATE_RET( f_rng_blind != NULL );
 
    return( ecdsa_sign_det_restartable( grp, r, s, d, buf, blen, md_alg,
                                        f_rng_blind, p_rng_blind, NULL ) );
 }
