xlate_to_uni(const unsigned char *name, int len, unsigned char *outname,
	     int *longlen, int *outlen, int escape, int utf8,
	     struct nls_table *nls)
{
	const unsigned char *ip;
	unsigned char nc;
	unsigned char *op;
	unsigned int ec;
	int i, k, fill;
 	int charlen;
 
 	if (utf8) {
		*outlen = utf8s_to_utf16s(name, len, (wchar_t *)outname);
 		if (*outlen < 0)
 			return *outlen;
 		else if (*outlen > FAT_LFN_LEN)
			return -ENAMETOOLONG;

		op = &outname[*outlen * sizeof(wchar_t)];
	} else {
		if (nls) {
			for (i = 0, ip = name, op = outname, *outlen = 0;
			     i < len && *outlen <= FAT_LFN_LEN;
			     *outlen += 1)
			{
				if (escape && (*ip == ':')) {
					if (i > len - 5)
						return -EINVAL;
					ec = 0;
					for (k = 1; k < 5; k++) {
						nc = ip[k];
						ec <<= 4;
						if (nc >= '0' && nc <= '9') {
							ec |= nc - '0';
							continue;
						}
						if (nc >= 'a' && nc <= 'f') {
							ec |= nc - ('a' - 10);
							continue;
						}
						if (nc >= 'A' && nc <= 'F') {
							ec |= nc - ('A' - 10);
							continue;
						}
						return -EINVAL;
					}
					*op++ = ec & 0xFF;
					*op++ = ec >> 8;
					ip += 5;
					i += 5;
				} else {
					if ((charlen = nls->char2uni(ip, len - i, (wchar_t *)op)) < 0)
						return -EINVAL;
					ip += charlen;
					i += charlen;
					op += 2;
				}
			}
			if (i < len)
				return -ENAMETOOLONG;
		} else {
			for (i = 0, ip = name, op = outname, *outlen = 0;
			     i < len && *outlen <= FAT_LFN_LEN;
			     i++, *outlen += 1)
			{
				*op++ = *ip++;
				*op++ = 0;
			}
			if (i < len)
				return -ENAMETOOLONG;
		}
	}

	*longlen = *outlen;
	if (*outlen % 13) {
		*op++ = 0;
		*op++ = 0;
		*outlen += 1;
		if (*outlen % 13) {
			fill = 13 - (*outlen % 13);
			for (i = 0; i < fill; i++) {
				*op++ = 0xff;
				*op++ = 0xff;
			}
			*outlen += fill;
		}
	}

	return 0;
}
