PHP_FUNCTION(mb_ereg_search_init)
{
	size_t argc = ZEND_NUM_ARGS();
	zval *arg_str;
	char *arg_pattern = NULL, *arg_options = NULL;
	int arg_pattern_len = 0, arg_options_len = 0;
	OnigSyntaxType *syntax = NULL;
	OnigOptionType option;

 	if (zend_parse_parameters(argc TSRMLS_CC, "z|ss", &arg_str, &arg_pattern, &arg_pattern_len, &arg_options, &arg_options_len) == FAILURE) {
 		return;
 	}

 	if (argc > 1 && arg_pattern_len == 0) {
 		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty pattern");
 		RETURN_FALSE;
	}

	option = MBREX(regex_default_options);
	syntax = MBREX(regex_default_syntax);

	if (argc == 3) {
		option = 0;
		_php_mb_regex_init_options(arg_options, arg_options_len, &option, &syntax, NULL);
	}

	if (argc > 1) {
		/* create regex pattern buffer */
		if ((MBREX(search_re) = php_mbregex_compile_pattern(arg_pattern, arg_pattern_len, option, MBREX(current_mbctype), syntax TSRMLS_CC)) == NULL) {
			RETURN_FALSE;
		}
	}

	if (MBREX(search_str) != NULL) {
		zval_ptr_dtor(&MBREX(search_str));
		MBREX(search_str) = (zval *)NULL;
	}

	MBREX(search_str) = arg_str;
	Z_ADDREF_P(MBREX(search_str));
	SEPARATE_ZVAL_IF_NOT_REF(&MBREX(search_str));

	MBREX(search_pos) = 0;

	if (MBREX(search_regs) != NULL) {
		onig_region_free(MBREX(search_regs), 1);
		MBREX(search_regs) = (OnigRegion *) NULL;
	}

	RETURN_TRUE;
}
