PHP_FUNCTION(mcrypt_list_algorithms)
{
	char **modules;
	char *lib_dir = MCG(algorithms_dir);
	int   lib_dir_len;
	int   i, count;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s",
 		&lib_dir, &lib_dir_len) == FAILURE) {
 		return;
 	}
 	array_init(return_value);
 	modules = mcrypt_list_algorithms(lib_dir, &count);
 
	if (count == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No algorithms found in module dir");
	}
	for (i = 0; i < count; i++) {
		add_index_string(return_value, i, modules[i], 1);
	}
	mcrypt_free_p(modules, count);
}
