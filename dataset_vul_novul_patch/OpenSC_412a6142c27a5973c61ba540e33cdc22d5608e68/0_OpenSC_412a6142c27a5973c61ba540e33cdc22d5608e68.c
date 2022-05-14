static int decode_bit_string(const u8 * inbuf, size_t inlen, void *outbuf,
			     size_t outlen, int invert)
 {
 	const u8 *in = inbuf;
 	u8 *out = (u8 *) outbuf;
 	int i, count = 0;
	int zero_bits;
	size_t octets_left;
 
 	if (outlen < octets_left)
 		return SC_ERROR_BUFFER_TOO_SMALL;
 	if (inlen < 1)
 		return SC_ERROR_INVALID_ASN1_OBJECT;

	zero_bits = *in & 0x07;
	octets_left = inlen - 1;
	in++;
	memset(outbuf, 0, outlen);

 	while (octets_left) {
 		/* 1st octet of input:  ABCDEFGH, where A is the MSB */
 		/* 1st octet of output: HGFEDCBA, where A is the LSB */
		/* first bit in bit string is the LSB in first resulting octet */
		int bits_to_go;

		*out = 0;
		if (octets_left == 1)
			bits_to_go = 8 - zero_bits;
		else
			bits_to_go = 8;
		if (invert)
			for (i = 0; i < bits_to_go; i++) {
				*out |= ((*in >> (7 - i)) & 1) << i;
			}
		else {
			*out = *in;
		}
		out++;
		in++;
		octets_left--;
		count++;
	}
	return (count * 8) - zero_bits;
}
