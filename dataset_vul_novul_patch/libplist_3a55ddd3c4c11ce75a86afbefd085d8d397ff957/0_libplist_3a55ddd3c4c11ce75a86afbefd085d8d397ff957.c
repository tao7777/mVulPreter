 unsigned char *base64decode(const char *buf, size_t *size)
 {
 	if (!buf || !size) return NULL;
	size_t len = (*size > 0) ? *size : strlen(buf);
	if (len <= 0) return NULL;
 	unsigned char *outbuf = (unsigned char*)malloc((len/4)*3+3);
 	const char *ptr = buf;
 	int p = 0;
	int wv, w1, w2, w3, w4;
	int tmpval[4];
	int tmpcnt = 0;
 
 	do {
		while (ptr < buf+len && (*ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r')) {
			ptr++;
		}
 		if (*ptr == '\0' || ptr >= buf+len) {
 			break;
 		}
		if ((wv = base64_table[(int)(unsigned char)*ptr++]) == -1) {
			continue;
		}
		tmpval[tmpcnt++] = wv;
		if (tmpcnt == 4) {
			tmpcnt = 0;
			w1 = tmpval[0];
			w2 = tmpval[1];
			w3 = tmpval[2];
			w4 = tmpval[3];

			if (w2 >= 0) {
				outbuf[p++] = (unsigned char)(((w1 << 2) + (w2 >> 4)) & 0xFF);
			}
			if (w3 >= 0) {
				outbuf[p++] = (unsigned char)(((w2 << 4) + (w3 >> 2)) & 0xFF);
			}
			if (w4 >= 0) {
				outbuf[p++] = (unsigned char)(((w3 << 6) + w4) & 0xFF);
			}
 		}
 	} while (1);
 
	outbuf[p] = 0;
	*size = p;
	return outbuf;
}
