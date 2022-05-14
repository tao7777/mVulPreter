secret_core_crt (gcry_mpi_t M, gcry_mpi_t C,
                 gcry_mpi_t D, unsigned int Nlimbs,
                 gcry_mpi_t P, gcry_mpi_t Q, gcry_mpi_t U)
{
   gcry_mpi_t m1 = mpi_alloc_secure ( Nlimbs + 1 );
   gcry_mpi_t m2 = mpi_alloc_secure ( Nlimbs + 1 );
   gcry_mpi_t h  = mpi_alloc_secure ( Nlimbs + 1 );
  gcry_mpi_t D_blind = mpi_alloc_secure ( Nlimbs + 1 );
  gcry_mpi_t r;
  unsigned int r_nbits;

  r_nbits = mpi_get_nbits (P) / 4;
  if (r_nbits < 96)
    r_nbits = 96;
  r = mpi_alloc_secure ( (r_nbits + BITS_PER_MPI_LIMB-1)/BITS_PER_MPI_LIMB );

  /* d_blind = (d mod (p-1)) + (p-1) * r            */
  /* m1 = c ^ d_blind mod p */
  _gcry_mpi_randomize (r, r_nbits, GCRY_WEAK_RANDOM);
  mpi_set_highbit (r, r_nbits - 1);
   mpi_sub_ui ( h, P, 1 );
  mpi_mul ( D_blind, h, r );
   mpi_fdiv_r ( h, D, h );
  mpi_add ( D_blind, D_blind, h );
  mpi_powm ( m1, C, D_blind, P );
 
  /* d_blind = (d mod (q-1)) + (q-1) * r            */
  /* m2 = c ^ d_blind mod q */
  _gcry_mpi_randomize (r, r_nbits, GCRY_WEAK_RANDOM);
  mpi_set_highbit (r, r_nbits - 1);
   mpi_sub_ui ( h, Q, 1  );
  mpi_mul ( D_blind, h, r );
   mpi_fdiv_r ( h, D, h );
  mpi_add ( D_blind, D_blind, h );
  mpi_powm ( m2, C, D_blind, Q );

  mpi_free ( r );
  mpi_free ( D_blind );
 
   /* h = u * ( m2 - m1 ) mod q */
   mpi_sub ( h, m2, m1 );
  /* Remove superfluous leading zeroes from INPUT.  */
  mpi_normalize (input);

  if (!skey->p || !skey->q || !skey->u)
    {
      secret_core_std (output, input, skey->d, skey->n);
    }
  else
    {
      secret_core_crt (output, input, skey->d, mpi_get_nlimbs (skey->n),
                       skey->p, skey->q, skey->u);
    }
}
