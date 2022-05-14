_gcry_ecc_eddsa_sign (gcry_mpi_t input, ECC_secret_key *skey,
                      gcry_mpi_t r_r, gcry_mpi_t s, int hashalgo, gcry_mpi_t pk)
{
  int rc;
  mpi_ec_t ctx = NULL;
  int b;
  unsigned int tmp;
  unsigned char *digest = NULL;
  gcry_buffer_t hvec[3];
  const void *mbuf;
  size_t mlen;
  unsigned char *rawmpi = NULL;
  unsigned int rawmpilen;
  unsigned char *encpk = NULL; /* Encoded public key.  */
  unsigned int encpklen;
  mpi_point_struct I;          /* Intermediate value.  */
  mpi_point_struct Q;          /* Public key.  */
  gcry_mpi_t a, x, y, r;

  memset (hvec, 0, sizeof hvec);

  if (!mpi_is_opaque (input))
    return GPG_ERR_INV_DATA;

  /* Initialize some helpers.  */
  point_init (&I);
  point_init (&Q);
   a = mpi_snew (0);
   x = mpi_new (0);
   y = mpi_new (0);
  r = mpi_new (0);
   ctx = _gcry_mpi_ec_p_internal_new (skey->E.model, skey->E.dialect, 0,
                                      skey->E.p, skey->E.a, skey->E.b);
   b = (ctx->nbits+7)/8;
  if (b != 256/8) {
    rc = GPG_ERR_INTERNAL; /* We only support 256 bit. */
    goto leave;
  }

  rc = _gcry_ecc_eddsa_compute_h_d (&digest, skey->d, ctx);
  if (rc)
    goto leave;
  _gcry_mpi_set_buffer (a, digest, 32, 0);

  /* Compute the public key if it has not been supplied as optional
     parameter.  */
  if (pk)
    {
      rc = _gcry_ecc_eddsa_decodepoint (pk, ctx, &Q,  &encpk, &encpklen);
      if (rc)
        goto leave;
      if (DBG_CIPHER)
        log_printhex ("* e_pk", encpk, encpklen);
      if (!_gcry_mpi_ec_curve_point (&Q, ctx))
        {
          rc = GPG_ERR_BROKEN_PUBKEY;
          goto leave;
        }
    }
  else
    {
      _gcry_mpi_ec_mul_point (&Q, a, &skey->E.G, ctx);
      rc = _gcry_ecc_eddsa_encodepoint (&Q, ctx, x, y, 0, &encpk, &encpklen);
      if (rc)
        goto leave;
      if (DBG_CIPHER)
        log_printhex ("  e_pk", encpk, encpklen);
    }

  /* Compute R.  */
  mbuf = mpi_get_opaque (input, &tmp);
  mlen = (tmp +7)/8;
  if (DBG_CIPHER)
    log_printhex ("     m", mbuf, mlen);

  hvec[0].data = digest;
  hvec[0].off  = 32;
  hvec[0].len  = 32;
  hvec[1].data = (char*)mbuf;
  hvec[1].len  = mlen;
  rc = _gcry_md_hash_buffers (hashalgo, 0, digest, hvec, 2);
  if (rc)
    goto leave;
  reverse_buffer (digest, 64);
  if (DBG_CIPHER)
    log_printhex ("     r", digest, 64);
  _gcry_mpi_set_buffer (r, digest, 64, 0);
  _gcry_mpi_ec_mul_point (&I, r, &skey->E.G, ctx);
  if (DBG_CIPHER)
    log_printpnt ("   r", &I, ctx);

  /* Convert R into affine coordinates and apply encoding.  */
  rc = _gcry_ecc_eddsa_encodepoint (&I, ctx, x, y, 0, &rawmpi, &rawmpilen);
  if (rc)
    goto leave;
  if (DBG_CIPHER)
    log_printhex ("   e_r", rawmpi, rawmpilen);

  /* S = r + a * H(encodepoint(R) + encodepoint(pk) + m) mod n  */
  hvec[0].data = rawmpi;  /* (this is R) */
  hvec[0].off  = 0;
  hvec[0].len  = rawmpilen;
  hvec[1].data = encpk;
  hvec[1].off  = 0;
  hvec[1].len  = encpklen;
  hvec[2].data = (char*)mbuf;
  hvec[2].off  = 0;
  hvec[2].len  = mlen;
  rc = _gcry_md_hash_buffers (hashalgo, 0, digest, hvec, 3);
  if (rc)
    goto leave;

  /* No more need for RAWMPI thus we now transfer it to R_R.  */
  mpi_set_opaque (r_r, rawmpi, rawmpilen*8);
  rawmpi = NULL;

  reverse_buffer (digest, 64);
  if (DBG_CIPHER)
    log_printhex (" H(R+)", digest, 64);
  _gcry_mpi_set_buffer (s, digest, 64, 0);
  mpi_mulm (s, s, a, skey->E.n);
  mpi_addm (s, s, r, skey->E.n);
  rc = eddsa_encodempi (s, b, &rawmpi, &rawmpilen);
  if (rc)
    goto leave;
  if (DBG_CIPHER)
    log_printhex ("   e_s", rawmpi, rawmpilen);
  mpi_set_opaque (s, rawmpi, rawmpilen*8);
  rawmpi = NULL;

  rc = 0;

 leave:
  _gcry_mpi_release (a);
  _gcry_mpi_release (x);
  _gcry_mpi_release (y);
  _gcry_mpi_release (r);
  xfree (digest);
  _gcry_mpi_ec_free (ctx);
  point_free (&I);
  point_free (&Q);
  xfree (encpk);
  xfree (rawmpi);
  return rc;
}
