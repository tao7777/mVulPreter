PHP_FUNCTION(readlink)
{
	char *link;
	int link_len;
        char buff[MAXPATHLEN];
        int ret;
 
       if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p", &link, &link_len) == FAILURE) {
                return;
        }
	if (php_check_open_basedir(link TSRMLS_CC)) {
		RETURN_FALSE;
	}

	ret = php_sys_readlink(link, buff, MAXPATHLEN-1);

	if (ret == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
		RETURN_FALSE;
	}
	/* Append NULL to the end of the string */
	buff[ret] = '\0';

	RETURN_STRING(buff, 1);
}
