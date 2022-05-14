static int wc_ecc_gen_k(WC_RNG* rng, int size, mp_int* k, mp_int* order)
{
    int err;
#ifdef WOLFSSL_SMALL_STACK
    byte* buf;
#else
    byte  buf[ECC_MAXSIZE_GEN];
#endif

#ifdef WOLFSSL_SMALL_STACK
    buf = (byte*)XMALLOC(ECC_MAXSIZE_GEN, NULL, DYNAMIC_TYPE_ECC_BUFFER);
    if (buf == NULL)
        return MEMORY_E;
#endif

    /*generate 8 extra bytes to mitigate bias from the modulo operation below*/
    /*see section A.1.2 in 'Suite B Implementor's Guide to FIPS 186-3 (ECDSA)'*/
    size += 8;

    /* make up random string */
    err = wc_RNG_GenerateBlock(rng, buf, size);

    /* load random buffer data into k */
     if (err == 0)
         err = mp_read_unsigned_bin(k, (byte*)buf, size);
 
     /* the key should be smaller than the order of base point */
     if (err == MP_OKAY) {
         if (mp_cmp(k, order) != MP_LT) {
             err = mp_mod(k, order, k);
         }
     }
 
    /* quick sanity check to make sure we're not dealing with a 0 key */
    if (err == MP_OKAY) {
        if (mp_iszero(k) == MP_YES)
          err = MP_ZERO_E;
    }

     ForceZero(buf, ECC_MAXSIZE);
 #ifdef WOLFSSL_SMALL_STACK
     XFREE(buf, NULL, DYNAMIC_TYPE_ECC_BUFFER);
#endif

    return err;
}
