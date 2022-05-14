SPL_METHOD(RecursiveDirectoryIterator, getSubPathname)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
 	char *sub_name;
 	int len;
 	char slash = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_UNIXPATHS) ? '/' : DEFAULT_SLASH;
 	if (zend_parse_parameters_none() == FAILURE) {
 		return;
 	}

	if (intern->u.dir.sub_path) {
		len = spprintf(&sub_name, 0, "%s%c%s", intern->u.dir.sub_path, slash, intern->u.dir.entry.d_name);
		RETURN_STRINGL(sub_name, len, 0);
	} else {
		RETURN_STRING(intern->u.dir.entry.d_name, 1);
	}
}
