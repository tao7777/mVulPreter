SPL_METHOD(DirectoryIterator, getBasename)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
 	char *suffix = 0, *fname;
 	int slen = 0;
 	size_t flen;

 	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &suffix, &slen) == FAILURE) {
 		return;
 	}

	php_basename(intern->u.dir.entry.d_name, strlen(intern->u.dir.entry.d_name), suffix, slen, &fname, &flen TSRMLS_CC);

	RETURN_STRINGL(fname, flen, 0);
}
