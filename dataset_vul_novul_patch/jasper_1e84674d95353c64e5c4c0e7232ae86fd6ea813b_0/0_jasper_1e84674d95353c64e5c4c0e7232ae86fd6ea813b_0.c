int jpc_bitstream_putbits(jpc_bitstream_t *bitstream, int n, long v)
{
	int m;
 
 	/* We can reliably put at most 31 bits since ISO/IEC 9899 only
 	  guarantees that a long can represent values up to 2^31-1. */
	//assert(n >= 0 && n < 32);
	if (n < 0 || n >= 32) {
		return EOF;
	}
 	/* Ensure that only the bits to be output are nonzero. */
 	assert(!(v & (~JAS_ONES(n))));
 
	/* Put the desired number of bits to the specified bit stream. */
	m = n - 1;
	while (--n >= 0) {
		if (jpc_bitstream_putbit(bitstream, (v >> m) & 1) == EOF) {
			return EOF;
		}
		v <<= 1;
	}
	return 0;
}
