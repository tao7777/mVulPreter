 make_random_bytes(png_uint_32* seed, void* pv, size_t size)
 {
    png_uint_32 u0 = seed[0], u1 = seed[1];
   png_bytep bytes = voidcast(png_bytep, pv);
 
    /* There are thirty-three bits; the next bit in the sequence is bit-33 XOR
     * bit-20.  The top 1 bit is in u1, the bottom 32 are in u0.
    */
 size_t i;
 for (i=0; i<size; ++i)
 {
 /* First generate 8 new bits then shift them in at the end. */
      png_uint_32 u = ((u0 >> (20-8)) ^ ((u1 << 7) | (u0 >> (32-7)))) & 0xff;
      u1 <<= 8;
      u1 |= u0 >> 24;
      u0 <<= 8;
      u0 |= u;
 *bytes++ = (png_byte)u;
 }

   seed[0] = u0;
   seed[1] = u1;
}
