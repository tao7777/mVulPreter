PHP_METHOD(Phar, offsetGet)
{
	char *fname, *error;
	size_t fname_len;
	zval zfname;
	phar_entry_info *entry;
        zend_string *sfname;
        PHAR_ARCHIVE_OBJECT();
 
       if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &fname, &fname_len) == FAILURE) {
                return;
        }
 
	/* security is 0 here so that we can get a better error message than "entry doesn't exist" */
	if (!(entry = phar_get_entry_info_dir(phar_obj->archive, fname, fname_len, 1, &error, 0))) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Entry %s does not exist%s%s", fname, error?", ":"", error?error:"");
	} else {
		if (fname_len == sizeof(".phar/stub.php")-1 && !memcmp(fname, ".phar/stub.php", sizeof(".phar/stub.php")-1)) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Cannot get stub \".phar/stub.php\" directly in phar \"%s\", use getStub", phar_obj->archive->fname);
			return;
		}

		if (fname_len == sizeof(".phar/alias.txt")-1 && !memcmp(fname, ".phar/alias.txt", sizeof(".phar/alias.txt")-1)) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Cannot get alias \".phar/alias.txt\" directly in phar \"%s\", use getAlias", phar_obj->archive->fname);
			return;
		}

		if (fname_len >= sizeof(".phar")-1 && !memcmp(fname, ".phar", sizeof(".phar")-1)) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Cannot directly get any files or directories in magic \".phar\" directory", phar_obj->archive->fname);
			return;
		}

		if (entry->is_temp_dir) {
			efree(entry->filename);
			efree(entry);
		}

		sfname = strpprintf(0, "phar://%s/%s", phar_obj->archive->fname, fname);
		ZVAL_NEW_STR(&zfname, sfname);
		spl_instantiate_arg_ex1(phar_obj->spl.info_class, return_value, &zfname);
		zval_ptr_dtor(&zfname);
	}
}
