static inline int map_from_unicode(unsigned code, enum entity_charset charset, unsigned *res)
{
	unsigned char found;
	const uni_to_enc *table;
	size_t table_size;

	switch (charset) {
	case cs_8859_1:
 		/* identity mapping of code points to unicode */
 		if (code > 0xFF) {
 			return FAILURE;
		}
 		*res = code;
 		break;
 
	case cs_8859_5:
		if (code <= 0xA0 || code == 0xAD /* soft hyphen */) {
			*res = code;
		} else if (code == 0x2116) {
			*res = 0xF0; /* numero sign */
		} else if (code == 0xA7) {
			*res = 0xFD; /* section sign */
		} else if (code >= 0x0401 && code <= 0x044F) {
			if (code == 0x040D || code == 0x0450 || code == 0x045D)
				return FAILURE;
			*res = code - 0x360;
		} else {
 			return FAILURE;
 		}
 		break;

 	case cs_8859_15:
 		if (code < 0xA4 || (code > 0xBE && code <= 0xFF)) {
 			*res = code;
		} else { /* between A4 and 0xBE */
			found = unimap_bsearch(unimap_iso885915,
				code, sizeof(unimap_iso885915) / sizeof(*unimap_iso885915));
			if (found)
				*res = found;
			else
				return FAILURE;
		}
		break;

	case cs_cp1252:
		if (code <= 0x7F || (code >= 0xA0 && code <= 0xFF)) {
			*res = code;
		} else {
			found = unimap_bsearch(unimap_win1252,
				code, sizeof(unimap_win1252) / sizeof(*unimap_win1252));
			if (found)
				*res = found;
			else
				return FAILURE;
		}
		break;

	case cs_macroman:
		if (code == 0x7F)
			return FAILURE;
		table = unimap_macroman;
		table_size = sizeof(unimap_macroman) / sizeof(*unimap_macroman);
		goto table_over_7F;
	case cs_cp1251:
		table = unimap_win1251;
		table_size = sizeof(unimap_win1251) / sizeof(*unimap_win1251);
		goto table_over_7F;
	case cs_koi8r:
		table = unimap_koi8r;
		table_size = sizeof(unimap_koi8r) / sizeof(*unimap_koi8r);
		goto table_over_7F;
 	case cs_cp866:
 		table = unimap_cp866;
 		table_size = sizeof(unimap_cp866) / sizeof(*unimap_cp866);

 table_over_7F:
 		if (code <= 0x7F) {
 			*res = code;
		} else {
			found = unimap_bsearch(table, code, table_size);
			if (found)
				*res = found;
			else
				return FAILURE;
		}
		break;

	/* from here on, only map the possible characters in the ASCII range.
	 * to improve support here, it's a matter of building the unicode mappings.
	 * See <http://www.unicode.org/Public/6.0.0/ucd/Unihan.zip> */
	case cs_sjis:
	case cs_eucjp:
		/* we interpret 0x5C as the Yen symbol. This is not universal.
		 * See <http://www.w3.org/Submission/japanese-xml/#ambiguity_of_yen> */
		if (code >= 0x20 && code <= 0x7D) {
			if (code == 0x5C)
				return FAILURE;
			*res = code;
		} else {
			return FAILURE;
		}
		break;

	case cs_big5:
	case cs_big5hkscs:
	case cs_gb2312:
		if (code >= 0x20 && code <= 0x7D) {
			*res = code;
		} else {
			return FAILURE;
		}
		break;

	default:
		return FAILURE;
	}

	return SUCCESS;
}
