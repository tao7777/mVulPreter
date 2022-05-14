static php_mb_regex_t *php_mbregex_compile_pattern(const char *pattern, int patlen, OnigOptionType options, OnigEncoding enc, OnigSyntaxType *syntax TSRMLS_DC)
{
	int err_code = 0;
	int found = 0;
	php_mb_regex_t *retval = NULL, **rc = NULL;
	OnigErrorInfo err_info;
	OnigUChar err_str[ONIG_MAX_ERROR_MESSAGE_LEN];

	found = zend_hash_find(&MBREX(ht_rc), (char *)pattern, patlen+1, (void **) &rc);
	if (found == FAILURE || (*rc)->options != options || (*rc)->enc != enc || (*rc)->syntax != syntax) {
		if ((err_code = onig_new(&retval, (OnigUChar *)pattern, (OnigUChar *)(pattern + patlen), options, enc, syntax, &err_info)) != ONIG_NORMAL) {
			onig_error_code_to_str(err_str, err_code, err_info);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "mbregex compile err: %s", err_str);
			retval = NULL;
			goto out;
		}
		zend_hash_update(&MBREX(ht_rc), (char *) pattern, patlen + 1, (void *) &retval, sizeof(retval), NULL);
	} else if (found == SUCCESS) {
 		retval = *rc;
 	}
 out:
	return retval;
 }
