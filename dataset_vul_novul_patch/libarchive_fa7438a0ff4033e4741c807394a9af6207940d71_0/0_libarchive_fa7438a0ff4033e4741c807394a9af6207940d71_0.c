atol8(const char *p, size_t char_cnt)
 {
 	int64_t l;
 	int digit;

	if (char_cnt == 0)
		return (0);

 	l = 0;
 	while (char_cnt-- > 0) {
 		if (*p >= '0' && *p <= '7')
			digit = *p - '0';
		else
			break;
		p++;
		l <<= 3;
		l |= digit;
	}
	return (l);
}
