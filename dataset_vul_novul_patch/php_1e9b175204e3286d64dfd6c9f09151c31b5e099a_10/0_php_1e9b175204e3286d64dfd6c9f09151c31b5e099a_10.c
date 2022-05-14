PHP_METHOD(Phar, offsetSet)
{
	char *fname, *cont_str = NULL;
	size_t fname_len, cont_len;
	zval *zresource;
	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->archive->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Write operations disabled by the php.ini setting phar.readonly");
                return;
        }
 
       if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "pr", &fname, &fname_len, &zresource) == FAILURE
       && zend_parse_parameters(ZEND_NUM_ARGS(), "ps", &fname, &fname_len, &cont_str, &cont_len) == FAILURE) {
                return;
        }
 
	if (fname_len == sizeof(".phar/stub.php")-1 && !memcmp(fname, ".phar/stub.php", sizeof(".phar/stub.php")-1)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Cannot set stub \".phar/stub.php\" directly in phar \"%s\", use setStub", phar_obj->archive->fname);
		return;
	}

	if (fname_len == sizeof(".phar/alias.txt")-1 && !memcmp(fname, ".phar/alias.txt", sizeof(".phar/alias.txt")-1)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Cannot set alias \".phar/alias.txt\" directly in phar \"%s\", use setAlias", phar_obj->archive->fname);
		return;
	}

	if (fname_len >= sizeof(".phar")-1 && !memcmp(fname, ".phar", sizeof(".phar")-1)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Cannot set any files or directories in magic \".phar\" directory", phar_obj->archive->fname);
		return;
	}

	phar_add_file(&(phar_obj->archive), fname, fname_len, cont_str, cont_len, zresource);
}
