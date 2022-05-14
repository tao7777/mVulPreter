_php_mb_regex_init_options(const char *parg, int narg, OnigOptionType *option, OnigSyntaxType **syntax, int *eval) 
_php_mb_regex_init_options(const char *parg, int narg, OnigOptionType *option, OnigSyntaxType **syntax, int *eval)
 {
 	int n;
 	char c;
	int optm = 0;
 
 	*syntax = ONIG_SYNTAX_RUBY;
 
	if (parg != NULL) {
		n = 0;
		while(n < narg) {
			c = parg[n++];
			switch (c) {
				case 'i':
					optm |= ONIG_OPTION_IGNORECASE;
					break;
				case 'x':
					optm |= ONIG_OPTION_EXTEND;
					break;
				case 'm':
					optm |= ONIG_OPTION_MULTILINE;
					break;
				case 's':
					optm |= ONIG_OPTION_SINGLELINE;
					break;
				case 'p':
					optm |= ONIG_OPTION_MULTILINE | ONIG_OPTION_SINGLELINE;
					break;
				case 'l':
					optm |= ONIG_OPTION_FIND_LONGEST;
					break;
				case 'n':
					optm |= ONIG_OPTION_FIND_NOT_EMPTY;
					break;
				case 'j':
					*syntax = ONIG_SYNTAX_JAVA;
					break;
				case 'u':
					*syntax = ONIG_SYNTAX_GNU_REGEX;
					break;
				case 'g':
					*syntax = ONIG_SYNTAX_GREP;
					break;
				case 'c':
					*syntax = ONIG_SYNTAX_EMACS;
					break;
				case 'r':
					*syntax = ONIG_SYNTAX_RUBY;
					break;
				case 'z':
					*syntax = ONIG_SYNTAX_PERL;
					break;
				case 'b':
					*syntax = ONIG_SYNTAX_POSIX_BASIC;
					break;
				case 'd':
 					*syntax = ONIG_SYNTAX_POSIX_EXTENDED;
 					break;
 				case 'e':
					if (eval != NULL) *eval = 1;
 					break;
 				default:
 					break;
 			}
 		}
		if (option != NULL) *option|=optm;
 	}
 }
