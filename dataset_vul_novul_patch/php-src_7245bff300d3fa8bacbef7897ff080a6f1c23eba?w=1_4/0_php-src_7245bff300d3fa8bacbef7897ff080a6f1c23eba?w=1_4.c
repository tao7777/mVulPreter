SPL_METHOD(SplFileInfo, getPath)
{
 	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
 	char *path;
 	int path_len;

 	if (zend_parse_parameters_none() == FAILURE) {
 		return;
 	}

  	path = spl_filesystem_object_get_path(intern, &path_len TSRMLS_CC);
	RETURN_STRINGL(path, path_len, 1);
}
