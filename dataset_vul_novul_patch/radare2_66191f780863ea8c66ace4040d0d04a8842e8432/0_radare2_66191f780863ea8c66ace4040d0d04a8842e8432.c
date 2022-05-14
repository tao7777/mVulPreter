 static x86newTokenType getToken(const char *str, size_t *begin, size_t *end) {
	if (*begin > strlen (str)) {
		return TT_EOF;
	}
	while (begin && str[*begin] && isspace ((ut8)str[*begin])) {
 		++(*begin);
 	}
 
 	if (!str[*begin]) {                // null byte
 		*end = *begin;
 		return TT_EOF;
	}
	if (isalpha ((ut8)str[*begin])) {   // word token
 		*end = *begin;
		while (end && str[*end] && isalnum ((ut8)str[*end])) {
 			++(*end);
 		}
 		return TT_WORD;
	}
	if (isdigit ((ut8)str[*begin])) {   // number token
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
