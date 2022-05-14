long jpc_bitstream_getbits(jpc_bitstream_t *bitstream, int n)
{
	long v;
	int u;
 
 	/* We can reliably get at most 31 bits since ISO/IEC 9899 only
 	  guarantees that a long can represent values up to 2^31-1. */
	//assert(n >= 0 && n < 32);
	if (n < 0 || n >= 32) {
		return -1;
	}
 
 	/* Get the number of bits requested from the specified bit stream. */
 	v = 0;
	while (--n >= 0) {
		if ((u = jpc_bitstream_getbit(bitstream)) < 0) {
			return -1;
		}
		v = (v << 1) | u;
	}
	return v;
}
