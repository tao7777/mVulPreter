static bigint *sig_verify(BI_CTX *ctx, const uint8_t *sig, int sig_len,
static bigint *sig_verify(BI_CTX *ctx, const uint8_t *sig, int sig_len, uint8_t sig_type,
         bigint *modulus, bigint *pub_exp)
 {
    int i;
     bigint *decrypted_bi, *dat_bi;
     bigint *bir = NULL;
     uint8_t *block = (uint8_t *)malloc(sig_len);
 
    const uint8_t *sig_prefix = NULL;
    uint8_t sig_prefix_size = 0, hash_len = 0;
    /* adjust our expections */
    switch (sig_type)
    {
        case SIG_TYPE_MD5:
            sig_prefix = sig_prefix_md5;
            sig_prefix_size = sizeof(sig_prefix_md5);
        break;
        case SIG_TYPE_SHA1:
            sig_prefix = sig_prefix_sha1;
            sig_prefix_size = sizeof(sig_prefix_sha1);
        break;
        case SIG_TYPE_SHA256:
            sig_prefix = sig_prefix_sha256;
            sig_prefix_size = sizeof(sig_prefix_sha256);
        break;
        case SIG_TYPE_SHA384:
            sig_prefix = sig_prefix_sha384;
            sig_prefix_size = sizeof(sig_prefix_sha384);
        break;
        case SIG_TYPE_SHA512:
            sig_prefix = sig_prefix_sha512;
            sig_prefix_size = sizeof(sig_prefix_sha512);
        break;
    }
    if (sig_prefix)
        hash_len = sig_prefix[sig_prefix_size - 1];

    /* check length (#A) */
    if (sig_len < 2 + 8 + 1 + sig_prefix_size + hash_len)
        goto err;

     /* decrypt */
     dat_bi = bi_import(ctx, sig, sig_len);
     ctx->mod_offset = BIGINT_M_OFFSET;

    /* convert to a normal block */
    decrypted_bi = bi_mod_power2(ctx, dat_bi, modulus, pub_exp);

     bi_export(ctx, decrypted_bi, block, sig_len);
     ctx->mod_offset = BIGINT_M_OFFSET;
 
    /* check the first 2 bytes */
    if (block[0] != 0 || block[1] != 1)
        goto err;
 
    /* check the padding */
    i = 2; /* start at the first padding byte */
    while (i < sig_len - 1 - sig_prefix_size - hash_len)
    { /* together with (#A), we require at least 8 bytes of padding */
        if (block[i++] != 0xFF)
            goto err;
     }

    /* check end of padding */
    if (block[i++] != 0)
        goto err;

    /* check the ASN.1 metadata */
    if (memcmp_P(block+i, sig_prefix, sig_prefix_size))
        goto err;

    /* now we can get the hash we need */
    bir = bi_import(ctx, block + i + sig_prefix_size, hash_len);

err:
     free(block);
     /* save a few bytes of memory */
     bi_clear_cache(ctx);
    return bir;
}
