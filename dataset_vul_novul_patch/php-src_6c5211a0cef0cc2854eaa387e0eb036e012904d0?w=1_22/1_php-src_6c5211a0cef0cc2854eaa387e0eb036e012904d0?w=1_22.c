PHP_MINFO_FUNCTION(mcrypt) /* {{{ */
{
	char **modules;
	char mcrypt_api_no[16];
	int i, count;
	smart_str tmp1 = {0};
	smart_str tmp2 = {0};

	modules = mcrypt_list_algorithms(MCG(algorithms_dir), &count);
	if (count == 0) {
		smart_str_appends(&tmp1, "none");
	}
	for (i = 0; i < count; i++) {
		smart_str_appends(&tmp1, modules[i]);
		smart_str_appendc(&tmp1, ' ');
	}
	smart_str_0(&tmp1);
	mcrypt_free_p(modules, count);

	modules = mcrypt_list_modes(MCG(modes_dir), &count);
	if (count == 0) {
		smart_str_appends(&tmp2, "none");
	}
	for (i = 0; i < count; i++) {
		smart_str_appends(&tmp2, modules[i]);
		smart_str_appendc(&tmp2, ' ');
	}
	smart_str_0 (&tmp2);
	mcrypt_free_p (modules, count);

	snprintf (mcrypt_api_no, 16, "%d", MCRYPT_API_VERSION);

	php_info_print_table_start();
	php_info_print_table_header(2, "mcrypt support", "enabled");
	php_info_print_table_header(2, "mcrypt_filter support", "enabled");
	php_info_print_table_row(2, "Version", LIBMCRYPT_VERSION);
	php_info_print_table_row(2, "Api No", mcrypt_api_no);
	php_info_print_table_row(2, "Supported ciphers", tmp1.c);
	php_info_print_table_row(2, "Supported modes", tmp2.c);
 	smart_str_free(&tmp1);
 	smart_str_free(&tmp2);
 	php_info_print_table_end();
 	DISPLAY_INI_ENTRIES();
 }
 /* }}} */
