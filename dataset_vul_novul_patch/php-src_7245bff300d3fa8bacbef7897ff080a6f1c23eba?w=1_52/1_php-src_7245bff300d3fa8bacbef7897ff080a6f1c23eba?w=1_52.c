 void spl_filesystem_info_set_filename(spl_filesystem_object *intern, char *path, int len, int use_copy TSRMLS_DC) /* {{{ */
 {
 	char *p1, *p2;
 	if (intern->file_name) {
 		efree(intern->file_name);
 	}

	intern->file_name = use_copy ? estrndup(path, len) : path;
	intern->file_name_len = len;

	while(IS_SLASH_AT(intern->file_name, intern->file_name_len-1) && intern->file_name_len > 1) {
		intern->file_name[intern->file_name_len-1] = 0;
		intern->file_name_len--;
	}

	p1 = strrchr(intern->file_name, '/');
#if defined(PHP_WIN32) || defined(NETWARE)
	p2 = strrchr(intern->file_name, '\\');
#else
	p2 = 0;
#endif
	if (p1 || p2) {
		intern->_path_len = (p1 > p2 ? p1 : p2) - intern->file_name;
 	} else {
 		intern->_path_len = 0;
 	}
 	if (intern->_path) {
 		efree(intern->_path);
 	}
	intern->_path = estrndup(path, intern->_path_len);
} /* }}} */
