PHP_METHOD(Phar, loadPhar)
{
        char *fname, *alias = NULL, *error;
        size_t fname_len, alias_len = 0;
 
       if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s!", &fname, &fname_len, &alias, &alias_len) == FAILURE) {
                return;
        }
 
	phar_request_initialize();

	RETVAL_BOOL(phar_open_from_filename(fname, fname_len, alias, alias_len, REPORT_ERRORS, NULL, &error) == SUCCESS);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
		efree(error);
	}
} /* }}} */

/* {{{ proto string Phar::apiVersion()
