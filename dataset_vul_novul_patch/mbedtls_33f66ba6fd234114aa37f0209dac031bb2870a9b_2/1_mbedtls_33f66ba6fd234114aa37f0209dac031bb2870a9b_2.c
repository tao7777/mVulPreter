int mbedtls_ecdsa_sign_det( mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                    const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                    mbedtls_md_type_t md_alg )
 {
     ECDSA_VALIDATE_RET( grp   != NULL );
     ECDSA_VALIDATE_RET( r     != NULL );
     ECDSA_VALIDATE_RET( s     != NULL );
     ECDSA_VALIDATE_RET( d     != NULL );
     ECDSA_VALIDATE_RET( buf   != NULL || blen == 0 );
 
    return( ecdsa_sign_det_restartable( grp, r, s, d, buf, blen, md_alg, NULL ) );
 }
