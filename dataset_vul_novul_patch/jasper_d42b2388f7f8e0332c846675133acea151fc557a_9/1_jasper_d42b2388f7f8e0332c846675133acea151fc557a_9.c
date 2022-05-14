static inline long decode_twos_comp(ulong c, int prec)
 {
 	long result;
 	assert(prec >= 2);
	jas_eprintf("warning: support for signed data is untested\n");
	result = (c & ((1 << (prec - 1)) - 1)) - (c & (1 << (prec - 1)));
 	return result;
 }
