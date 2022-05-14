int sc_asn1_read_tag(const u8 ** buf, size_t buflen, unsigned int *cla_out,
		     unsigned int *tag_out, size_t *taglen)
{
	const u8 *p = *buf;
	size_t left = buflen, len;
	unsigned int cla, tag, i;

	if (left < 2)
		return SC_ERROR_INVALID_ASN1_OBJECT;
	*buf = NULL;
	if (*p == 0xff || *p == 0) {
		/* end of data reached */
		*taglen = 0;
		*tag_out = SC_ASN1_TAG_EOC;
		return SC_SUCCESS;
	}
	/* parse tag byte(s)
	 * Resulted tag is presented by integer that has not to be
	 * confused with the 'tag number' part of ASN.1 tag.
	 */
	cla = (*p & SC_ASN1_TAG_CLASS) | (*p & SC_ASN1_TAG_CONSTRUCTED);
	tag = *p & SC_ASN1_TAG_PRIMITIVE;
	p++;
	left--;
	if (tag == SC_ASN1_TAG_PRIMITIVE) {
		/* high tag number */
		size_t n = SC_ASN1_TAGNUM_SIZE - 1;
		/* search the last tag octet */
		while (left-- != 0 && n != 0) {
			tag <<= 8;
			tag |= *p;
			if ((*p++ & 0x80) == 0)
				break;
			n--;
		}
		if (left == 0 || n == 0)
			/* either an invalid tag or it doesn't fit in
			 * unsigned int */
			return SC_ERROR_INVALID_ASN1_OBJECT;
	}

	/* parse length byte(s) */
 	len = *p & 0x7f;
 	if (*p++ & 0x80) {
 		unsigned int a = 0;
 		if (len > 4 || len > left)
 			return SC_ERROR_INVALID_ASN1_OBJECT;
 		left -= len;
		for (i = 0; i < len; i++) {
			a <<= 8;
			a |= *p;
			p++;
		}
		len = a;
	}

	*cla_out = cla;
	*tag_out = tag;
	*taglen = len;
	*buf = p;

	if (len > left)
		return SC_ERROR_ASN1_END_OF_CONTENTS;

	return SC_SUCCESS;
}
