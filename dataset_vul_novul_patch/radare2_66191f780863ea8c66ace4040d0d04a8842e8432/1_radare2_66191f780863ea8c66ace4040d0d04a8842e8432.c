 static x86newTokenType getToken(const char *str, size_t *begin, size_t *end) {
	while (begin && isspace ((ut8)str[*begin])) {
 		++(*begin);
 	}
 
 	if (!str[*begin]) {                // null byte
 		*end = *begin;
 		return TT_EOF;
	} else if (isalpha ((ut8)str[*begin])) {   // word token
 		*end = *begin;
		while (end && isalnum ((ut8)str[*end])) {
 			++(*end);
 		}
 		return TT_WORD;
	} else if (isdigit ((ut8)str[*begin])) {   // number token
 		*end = *begin;
 		while (end && isalnum ((ut8)str[*end])) {     // accept alphanumeric characters, because hex.
 			++(*end);
		}
		return TT_NUMBER;
	} else {                             // special character: [, ], +, *, ...
		*end = *begin + 1;
		return TT_SPECIAL;
	}
}
