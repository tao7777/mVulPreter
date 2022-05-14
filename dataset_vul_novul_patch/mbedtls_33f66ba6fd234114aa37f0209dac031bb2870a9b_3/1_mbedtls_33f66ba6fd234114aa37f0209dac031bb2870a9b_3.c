int mbedtls_ecdsa_write_signature_restartable( mbedtls_ecdsa_context *ctx,
                           mbedtls_md_type_t md_alg,
                           const unsigned char *hash, size_t hlen,
                           unsigned char *sig, size_t *slen,
                           int (*f_rng)(void *, unsigned char *, size_t),
                           void *p_rng,
                           mbedtls_ecdsa_restart_ctx *rs_ctx )
{
    int ret;
    mbedtls_mpi r, s;
    ECDSA_VALIDATE_RET( ctx  != NULL );
    ECDSA_VALIDATE_RET( hash != NULL );
    ECDSA_VALIDATE_RET( sig  != NULL );
    ECDSA_VALIDATE_RET( slen != NULL );

    mbedtls_mpi_init( &r );
     mbedtls_mpi_init( &s );
 
 #if defined(MBEDTLS_ECDSA_DETERMINISTIC)
    (void) f_rng;
    (void) p_rng;
     MBEDTLS_MPI_CHK( ecdsa_sign_det_restartable( &ctx->grp, &r, &s, &ctx->d,
                             hash, hlen, md_alg, rs_ctx ) );
 #else
     (void) md_alg;
 
 #if defined(MBEDTLS_ECDSA_SIGN_ALT)
     MBEDTLS_MPI_CHK( mbedtls_ecdsa_sign( &ctx->grp, &r, &s, &ctx->d,
                          hash, hlen, f_rng, p_rng ) );
 #else
     MBEDTLS_MPI_CHK( ecdsa_sign_restartable( &ctx->grp, &r, &s, &ctx->d,
                         hash, hlen, f_rng, p_rng, rs_ctx ) );
 #endif /* MBEDTLS_ECDSA_SIGN_ALT */
 #endif /* MBEDTLS_ECDSA_DETERMINISTIC */
 
    MBEDTLS_MPI_CHK( ecdsa_signature_to_asn1( &r, &s, sig, slen ) );

cleanup:
    mbedtls_mpi_free( &r );
    mbedtls_mpi_free( &s );

    return( ret );
}
