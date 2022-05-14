atol10(const char *p, size_t char_cnt)
{
 	uint64_t l;
 	int digit;
 
 	l = 0;
 	digit = *p - '0';
 	while (digit >= 0 && digit < 10  && char_cnt-- > 0) {
		l = (l * 10) + digit;
		digit = *++p - '0';
	}
	return (l);
}
