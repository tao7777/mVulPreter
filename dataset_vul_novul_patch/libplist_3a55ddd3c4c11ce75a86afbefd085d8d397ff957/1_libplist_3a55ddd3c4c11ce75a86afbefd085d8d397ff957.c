 unsigned char *base64decode(const char *buf, size_t *size)
 {
 	if (!buf || !size) return NULL;
	size_t len = (*size > 0) ? *size : strlen(buf);
	if (len <= 0) return NULL;
 	unsigned char *outbuf = (unsigned char*)malloc((len/4)*3+3);
 	const char *ptr = buf;
 	int p = 0;
	size_t l = 0;
 
 	do {
		ptr += strspn(ptr, "\r\n\t ");
 		if (*ptr == '\0' || ptr >= buf+len) {
 			break;
 		}
		l = strcspn(ptr, "\r\n\t ");
		if (l > 3 && ptr+l <= buf+len) {
			p+=base64decode_block(outbuf+p, ptr, l);
			ptr += l;
		} else {
			break;
 		}
 	} while (1);
 
	outbuf[p] = 0;
	*size = p;
	return outbuf;
}
