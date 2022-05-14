int mbedtls_ecdsa_write_signature( mbedtls_ecdsa_context *ctx, mbedtls_md_type_t md_alg,
                           const unsigned char *hash, size_t hlen,
                           unsigned char *sig, size_t *slen,
                           int (*f_rng)(void *, unsigned char *, size_t),
                           void *p_rng )
{
    int ret;
    mbedtls_mpi r, s;

    mbedtls_mpi_init( &r );
     mbedtls_mpi_init( &s );
 
 #if defined(MBEDTLS_ECDSA_DETERMINISTIC)
    (void) f_rng;
    (void) p_rng;
    MBEDTLS_MPI_CHK( mbedtls_ecdsa_sign_det( &ctx->grp, &r, &s, &ctx->d,
                             hash, hlen, md_alg ) );
 #else
     (void) md_alg;
 
     MBEDTLS_MPI_CHK( mbedtls_ecdsa_sign( &ctx->grp, &r, &s, &ctx->d,
                          hash, hlen, f_rng, p_rng ) );
#endif
 
     MBEDTLS_MPI_CHK( ecdsa_signature_to_asn1( &r, &s, sig, slen ) );
 
cleanup:
    mbedtls_mpi_free( &r );
    mbedtls_mpi_free( &s );

    return( ret );
}
