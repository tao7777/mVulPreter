int wc_ecc_sign_hash_ex(const byte* in, word32 inlen, WC_RNG* rng,
                     ecc_key* key, mp_int *r, mp_int *s)
{
   int    err;
#ifndef WOLFSSL_SP_MATH
   mp_int* e;
#if !defined(WOLFSSL_ASYNC_CRYPT) || !defined(HAVE_CAVIUM_V)
   mp_int  e_lcl;
#endif
#endif /* !WOLFSSL_SP_MATH */

   DECLARE_CURVE_SPECS(1)

   if (in == NULL || r == NULL || s == NULL || key == NULL || rng == NULL)
       return ECC_BAD_ARG_E;

   /* is this a private key? */
   if (key->type != ECC_PRIVATEKEY && key->type != ECC_PRIVATEKEY_ONLY) {
      return ECC_BAD_ARG_E;
   }

   /* is the IDX valid ?  */
   if (wc_ecc_is_valid_idx(key->idx) != 1) {
      return ECC_BAD_ARG_E;
   }

#ifdef WOLFSSL_SP_MATH
    if (key->idx != ECC_CUSTOM_IDX && ecc_sets[key->idx].id == ECC_SECP256R1)
        return sp_ecc_sign_256(in, inlen, rng, &key->k, r, s, key->heap);
    else
        return WC_KEY_SIZE_E;
#else
#ifdef WOLFSSL_HAVE_SP_ECC
    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC) && \
           defined(WOLFSSL_ASYNC_CRYPT_TEST)
    if (key->asyncDev.marker != WOLFSSL_ASYNC_MARKER_ECC)
    #endif
    {
#ifndef WOLFSSL_SP_NO_256
        if (key->idx != ECC_CUSTOM_IDX && ecc_sets[key->idx].id == ECC_SECP256R1)
            return sp_ecc_sign_256(in, inlen, rng, &key->k, r, s, key->heap);
#endif
    }
#endif /* WOLFSSL_HAVE_SP_ECC */


#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC) && \
       defined(WOLFSSL_ASYNC_CRYPT_TEST)
    if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_ECC) {
        if (wc_AsyncTestInit(&key->asyncDev, ASYNC_TEST_ECC_SIGN)) {
            WC_ASYNC_TEST* testDev = &key->asyncDev.test;
            testDev->eccSign.in = in;
            testDev->eccSign.inSz = inlen;
            testDev->eccSign.rng = rng;
            testDev->eccSign.key = key;
            testDev->eccSign.r = r;
            testDev->eccSign.s = s;
            return WC_PENDING_E;
        }
    }
#endif

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(HAVE_CAVIUM_V)
   err = wc_ecc_alloc_mpint(key, &key->e);
   if (err != 0)
      return err;
   e = key->e;
#else
   e = &e_lcl;
#endif

   /* get the hash and load it as a bignum into 'e' */
   /* init the bignums */
   if ((err = mp_init(e)) != MP_OKAY) {
      return err;
   }

   /* load curve info */
   err = wc_ecc_curve_load(key->dp, &curve, ECC_CURVE_FIELD_ORDER);

   /* load digest into e */
   if (err == MP_OKAY) {
       /* we may need to truncate if hash is longer than key size */
       word32 orderBits = mp_count_bits(curve->order);

       /* truncate down to byte size, may be all that's needed */
       if ((WOLFSSL_BIT_SIZE * inlen) > orderBits)
           inlen = (orderBits + WOLFSSL_BIT_SIZE - 1) / WOLFSSL_BIT_SIZE;
       err = mp_read_unsigned_bin(e, (byte*)in, inlen);

       /* may still need bit truncation too */
       if (err == MP_OKAY && (WOLFSSL_BIT_SIZE * inlen) > orderBits)
           mp_rshb(e, WOLFSSL_BIT_SIZE - (orderBits & 0x7));
   }

   /* make up a key and export the public copy */
   if (err == MP_OKAY) {
       int loop_check = 0;
       ecc_key pubkey;

   #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC)
        if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_ECC) {
        #if defined(HAVE_CAVIUM_V) || defined(HAVE_INTEL_QA)
        #ifdef HAVE_CAVIUM_V
            if (NitroxEccIsCurveSupported(key))
        #endif
            {
               word32 keySz = key->dp->size;
               mp_int* k;
            #ifdef HAVE_CAVIUM_V
               err = wc_ecc_alloc_mpint(key, &key->signK);
               if (err != 0)
                  return err;
               k = key->signK;
            #else
               mp_int k_lcl;
               k = &k_lcl;
            #endif

               err = mp_init(k);

                /* make sure r and s are allocated */
           #ifdef HAVE_CAVIUM_V
               /* Nitrox V needs single buffer for R and S */
               if (err == MP_OKAY)
                   err = wc_bigint_alloc(&key->r->raw, NitroxEccGetSize(key)*2);
               /* Nitrox V only needs Prime and Order */
               if (err == MP_OKAY)
                   err = wc_ecc_curve_load(key->dp, &curve,
                        (ECC_CURVE_FIELD_PRIME | ECC_CURVE_FIELD_ORDER));
           #else
               if (err == MP_OKAY)
                   err = wc_bigint_alloc(&key->r->raw, key->dp->size);
               if (err == MP_OKAY)
                   err = wc_ecc_curve_load(key->dp, &curve, ECC_CURVE_FIELD_ALL);
           #endif
               if (err == MP_OKAY)
                   err = wc_bigint_alloc(&key->s->raw, key->dp->size);

               /* load e and k */
               if (err == MP_OKAY)
                   err = wc_mp_to_bigint_sz(e, &e->raw, keySz);
               if (err == MP_OKAY)
                   err = wc_mp_to_bigint_sz(&key->k, &key->k.raw, keySz);
               if (err == MP_OKAY)
                   err = wc_ecc_gen_k(rng, key->dp->size, k, curve->order);
               if (err == MP_OKAY)
                   err = wc_mp_to_bigint_sz(k, &k->raw, keySz);

           #ifdef HAVE_CAVIUM_V
               if (err == MP_OKAY)
                   err = NitroxEcdsaSign(key, &e->raw, &key->k.raw, &k->raw,
                    &r->raw, &s->raw, &curve->prime->raw, &curve->order->raw);
           #else
               if (err == MP_OKAY)
                   err = IntelQaEcdsaSign(&key->asyncDev, &e->raw, &key->k.raw,
                      &k->raw, &r->raw, &s->raw, &curve->Af->raw, &curve->Bf->raw,
                      &curve->prime->raw, &curve->order->raw, &curve->Gx->raw,
                      &curve->Gy->raw);
           #endif

           #ifndef HAVE_CAVIUM_V
               mp_clear(e);
               mp_clear(k);
           #endif
               wc_ecc_curve_free(curve);

               return err;
           }
       #endif /* HAVE_CAVIUM_V || HAVE_INTEL_QA */
       }
   #endif /* WOLFSSL_ASYNC_CRYPT */
 
        /* don't use async for key, since we don't support async return here */
        if ((err = wc_ecc_init_ex(&pubkey, key->heap, INVALID_DEVID)) == MP_OKAY) {
           mp_int b;

           if (err == MP_OKAY) {
               err = mp_init(&b);
           }

        #ifdef WOLFSSL_CUSTOM_CURVES
            /* if custom curve, apply params to pubkey */
           if (err == MP_OKAY && key->idx == ECC_CUSTOM_IDX) {
                err = wc_ecc_set_custom_curve(&pubkey, key->dp);
            }
        #endif
 
           if (err == MP_OKAY) {
               /* Generate blinding value - non-zero value. */
               do {
                   if (++loop_check > 64) {
                        err = RNG_FAILURE_E;
                        break;
                   }

                   err = wc_ecc_gen_k(rng, key->dp->size, &b, curve->order);
               }
               while (err == MP_ZERO_E);
               loop_check = 0;
           }

            for (; err == MP_OKAY;) {
                if (++loop_check > 64) {
                     err = RNG_FAILURE_E;
                     break;
                }
                err = wc_ecc_make_key_ex(rng, key->dp->size, &pubkey,
                                                                   key->dp->id);
                if (err != MP_OKAY) break;
 
                /* find r = x1 mod n */
               err = mp_mod(pubkey.pubkey.x, curve->order, r);
               if (err != MP_OKAY) break;

               if (mp_iszero(r) == MP_YES) {
                #ifndef ALT_ECC_SIZE
                   mp_clear(pubkey.pubkey.x);
                   mp_clear(pubkey.pubkey.y);
                   mp_clear(pubkey.pubkey.z);
                #endif
                    mp_forcezero(&pubkey.k);
                }
                else {
                   /* find s = (e + xr)/k
                             = b.(e/k.b + x.r/k.b) */

                   /* k = k.b */
                   err = mp_mulmod(&pubkey.k, &b, curve->order, &pubkey.k);
                   if (err != MP_OKAY) break;

                   /* k = 1/k.b */
                    err = mp_invmod(&pubkey.k, curve->order, &pubkey.k);
                    if (err != MP_OKAY) break;
 
                   /* s = x.r */
                    err = mp_mulmod(&key->k, r, curve->order, s);
                    if (err != MP_OKAY) break;
 
                   /* s = x.r/k.b */
                   err = mp_mulmod(&pubkey.k, s, curve->order, s);
                   if (err != MP_OKAY) break;

                   /* e = e/k.b */
                   err = mp_mulmod(&pubkey.k, e, curve->order, e);
                   if (err != MP_OKAY) break;

                   /* s = e/k.b + x.r/k.b
                        = (e + x.r)/k.b */
                    err = mp_add(e, s, s);
                    if (err != MP_OKAY) break;
 
                   /* s = b.(e + x.r)/k.b
                        = (e + x.r)/k */
                   err = mp_mulmod(s, &b, curve->order, s);
                    if (err != MP_OKAY) break;
 
                    /* s = (e + xr)/k */
                   err = mp_mod(s, curve->order, s);
                   if (err != MP_OKAY) break;
 
                    if (mp_iszero(s) == MP_NO)
                        break;
                 }
            }
            wc_ecc_free(&pubkey);
           mp_clear(&b);
           mp_free(&b);
        }
    }
 
   mp_clear(e);
   wc_ecc_curve_free(curve);
#endif /* WOLFSSL_SP_MATH */

   return err;
}
