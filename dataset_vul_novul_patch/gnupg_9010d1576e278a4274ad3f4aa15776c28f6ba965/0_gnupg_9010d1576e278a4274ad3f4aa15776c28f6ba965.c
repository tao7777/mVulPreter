_gcry_ecc_ecdsa_sign (gcry_mpi_t input, ECC_secret_key *skey,
                      gcry_mpi_t r, gcry_mpi_t s,
                      int flags, int hashalgo)
{
  gpg_err_code_t rc = 0;
  int extraloops = 0;
  gcry_mpi_t k, dr, sum, k_1, x;
  mpi_point_struct I;
  gcry_mpi_t hash;
   const void *abuf;
   unsigned int abits, qbits;
   mpi_ec_t ctx;
  gcry_mpi_t b;                /* Random number needed for blinding.  */
  gcry_mpi_t bi;               /* multiplicative inverse of B.        */
 
   if (DBG_CIPHER)
     log_mpidump ("ecdsa sign hash  ", input );

  /* Convert the INPUT into an MPI if needed.  */
  rc = _gcry_dsa_normalize_hash (input, &hash, qbits);
  if (rc)
    return rc;
   if (rc)
     return rc;
 
  b  = mpi_snew (qbits);
  bi = mpi_snew (qbits);
  do
    {
      _gcry_mpi_randomize (b, qbits, GCRY_WEAK_RANDOM);
      mpi_mod (b, b, skey->E.n);
    }
  while (!mpi_invm (bi, b, skey->E.n));

   k = NULL;
   dr = mpi_alloc (0);
   sum = mpi_alloc (0);
    {
      do
        {
          mpi_free (k);
          k = NULL;
          if ((flags & PUBKEY_FLAG_RFC6979) && hashalgo)
            {
              /* Use Pornin's method for deterministic DSA.  If this
                 flag is set, it is expected that HASH is an opaque
                 MPI with the to be signed hash.  That hash is also
                 used as h1 from 3.2.a.  */
              if (!mpi_is_opaque (input))
                {
                  rc = GPG_ERR_CONFLICT;
                  goto leave;
                }

              abuf = mpi_get_opaque (input, &abits);
              rc = _gcry_dsa_gen_rfc6979_k (&k, skey->E.n, skey->d,
                                            abuf, (abits+7)/8,
                                            hashalgo, extraloops);
              if (rc)
                goto leave;
              extraloops++;
            }
          else
            k = _gcry_dsa_gen_k (skey->E.n, GCRY_STRONG_RANDOM);

          _gcry_mpi_ec_mul_point (&I, k, &skey->E.G, ctx);
          if (_gcry_mpi_ec_get_affine (x, NULL, &I, ctx))
            {
              if (DBG_CIPHER)
                log_debug ("ecc sign: Failed to get affine coordinates\n");
              rc = GPG_ERR_BAD_SIGNATURE;
              goto leave;
            }
          mpi_mod (r, x, skey->E.n);  /* r = x mod n */
        }
      while (!mpi_cmp_ui (r, 0));

      mpi_mulm (dr, skey->d, r, skey->E.n); /* dr = d*r mod n  */
      mpi_addm (sum, hash, dr, skey->E.n);  /* sum = hash + (d*r) mod n  */
      mpi_invm (k_1, k, skey->E.n);         /* k_1 = k^(-1) mod n  */
      mpi_mulm (s, k_1, sum, skey->E.n);    /* s = k^(-1)*(hash+(d*r)) mod n */
    }
  while (!mpi_cmp_ui (s, 0));

  if (DBG_CIPHER)
         }
