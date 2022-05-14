static int generate_key(DH *dh)
{
     int ok = 0;
     int generate_new_key = 0;
     unsigned l;
    BN_CTX *ctx = NULL;
     BN_MONT_CTX *mont = NULL;
     BIGNUM *pub_key = NULL, *priv_key = NULL;
 
    if (BN_num_bits(dh->p) > OPENSSL_DH_MAX_MODULUS_BITS) {
        DHerr(DH_F_GENERATE_KEY, DH_R_MODULUS_TOO_LARGE);
        return 0;
    }

     ctx = BN_CTX_new();
     if (ctx == NULL)
         goto err;
        generate_new_key = 1;
    } else
