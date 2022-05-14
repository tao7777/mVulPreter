sc_parse_ef_gdo_content(const unsigned char *gdo, size_t gdo_len,
		unsigned char *iccsn, size_t *iccsn_len,
		unsigned char *chn, size_t *chn_len)
{
	int r = SC_SUCCESS, iccsn_found = 0, chn_found = 0;
	const unsigned char *p = gdo;
	size_t left = gdo_len;

	while (left >= 2) {
		unsigned int cla, tag;
		size_t tag_len;

		r = sc_asn1_read_tag(&p, left, &cla, &tag, &tag_len);
		if (r != SC_SUCCESS) {
			if (r == SC_ERROR_ASN1_END_OF_CONTENTS) {
				/* not enough data */
				r = SC_SUCCESS;
			}
			break;
		}
		if (p == NULL) {
			/* done parsing */
			break;
		}

		if (cla == SC_ASN1_TAG_APPLICATION) {
			switch (tag) {
				case 0x1A:
					iccsn_found = 1;
					if (iccsn && iccsn_len) {
						memcpy(iccsn, p, MIN(tag_len, *iccsn_len));
						*iccsn_len = MIN(tag_len, *iccsn_len);
					}
					break;
				case 0x1F20:
					chn_found = 1;
					if (chn && chn_len) {
						memcpy(chn, p, MIN(tag_len, *chn_len));
						*chn_len = MIN(tag_len, *chn_len);
					}
					break;
			}
 		}
 
 		p += tag_len;
		left -= (p - gdo);
 	}
 
 	if (!iccsn_found && iccsn_len)
		*iccsn_len = 0;
	if (!chn_found && chn_len)
		*chn_len = 0;

	return r;
}
