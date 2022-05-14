static size_t _php_mb_regex_get_option_string(char *str, size_t len, OnigOptionType option, OnigSyntaxType *syntax)
{
	size_t len_left = len;
	size_t len_req = 0;
	char *p = str;
	char c;

	if ((option & ONIG_OPTION_IGNORECASE) != 0) {
		if (len_left > 0) {
 			--len_left;
 			*(p++) = 'i';
 		}
		++len_req;	
 	}
 
 	if ((option & ONIG_OPTION_EXTEND) != 0) {
 		if (len_left > 0) {
 			--len_left;
 			*(p++) = 'x';
 		}
		++len_req;	
 	}
 
 	if ((option & (ONIG_OPTION_MULTILINE | ONIG_OPTION_SINGLELINE)) ==
			(ONIG_OPTION_MULTILINE | ONIG_OPTION_SINGLELINE)) {
		if (len_left > 0) {
 			--len_left;
 			*(p++) = 'p';
 		}
		++len_req;	
 	} else {
 		if ((option & ONIG_OPTION_MULTILINE) != 0) {
 			if (len_left > 0) {
 				--len_left;
 				*(p++) = 'm';
 			}
			++len_req;	
 		}
 
 		if ((option & ONIG_OPTION_SINGLELINE) != 0) {
 			if (len_left > 0) {
 				--len_left;
 				*(p++) = 's';
 			}
			++len_req;	
 		}
	}	
 	if ((option & ONIG_OPTION_FIND_LONGEST) != 0) {
 		if (len_left > 0) {
 			--len_left;
 			*(p++) = 'l';
 		}
		++len_req;	
 	}
 	if ((option & ONIG_OPTION_FIND_NOT_EMPTY) != 0) {
 		if (len_left > 0) {
 			--len_left;
 			*(p++) = 'n';
 		}
		++len_req;	
 	}
 
 	c = 0;

	if (syntax == ONIG_SYNTAX_JAVA) {
		c = 'j';
	} else if (syntax == ONIG_SYNTAX_GNU_REGEX) {
		c = 'u';
	} else if (syntax == ONIG_SYNTAX_GREP) {
		c = 'g';
	} else if (syntax == ONIG_SYNTAX_EMACS) {
		c = 'c';
	} else if (syntax == ONIG_SYNTAX_RUBY) {
		c = 'r';
	} else if (syntax == ONIG_SYNTAX_PERL) {
		c = 'z';
	} else if (syntax == ONIG_SYNTAX_POSIX_BASIC) {
		c = 'b';
	} else if (syntax == ONIG_SYNTAX_POSIX_EXTENDED) {
		c = 'd';
	}

	if (c != 0) {
		if (len_left > 0) {
			--len_left;
			*(p++) = c;
		}
		++len_req;
	}


	if (len_left > 0) {
 		--len_left;
 		*(p++) = '\0';
 	}
	++len_req;	
 	if (len < len_req) {
 		return len_req;
 	}

	return 0;
}
