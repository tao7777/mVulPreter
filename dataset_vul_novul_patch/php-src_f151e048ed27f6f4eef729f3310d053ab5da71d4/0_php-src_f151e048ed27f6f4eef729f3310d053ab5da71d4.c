 PHP_FUNCTION(linkinfo)
 {
 	char *link;
	char *dirname;
 	size_t link_len;
 	zend_stat_t sb;
 	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p", &link, &link_len) == FAILURE) {
 		return;
 	}
 
	dirname = estrndup(link, link_len);
	php_dirname(dirname, link_len);

	if (php_check_open_basedir(dirname)) {
		efree(dirname);
		RETURN_FALSE;
	}

 	ret = VCWD_STAT(link, &sb);
 	if (ret == -1) {
 		php_error_docref(NULL, E_WARNING, "%s", strerror(errno));
		efree(dirname);
 		RETURN_LONG(Z_L(-1));
 	}
 
	efree(dirname);
 	RETURN_LONG((zend_long) sb.st_dev);
 }
