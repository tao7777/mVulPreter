static void _php_mb_regex_set_options(OnigOptionType options, OnigSyntaxType *syntax, OnigOptionType *prev_options, OnigSyntaxType **prev_syntax TSRMLS_DC) 
 {
 	if (prev_options != NULL) {
 		*prev_options = MBREX(regex_default_options);
	}
	if (prev_syntax != NULL) {
		*prev_syntax = MBREX(regex_default_syntax);
	}
	MBREX(regex_default_options) = options;
	MBREX(regex_default_syntax) = syntax;
}
