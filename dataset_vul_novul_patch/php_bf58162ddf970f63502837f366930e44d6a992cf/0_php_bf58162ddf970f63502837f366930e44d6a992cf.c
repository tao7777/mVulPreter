PHP_METHOD(Phar, webPhar)
{
	zval *mimeoverride = NULL, *rewrite = NULL;
	char *alias = NULL, *error, *index_php = NULL, *f404 = NULL, *ru = NULL;
	int alias_len = 0, ret, f404_len = 0, free_pathinfo = 0, ru_len = 0;
	char *fname, *path_info, *mime_type = NULL, *entry, *pt;
	const char *basename;
	int fname_len, entry_len, code, index_php_len = 0, not_cgi;
	phar_archive_data *phar = NULL;
	phar_entry_info *info = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!s!saz", &alias, &alias_len, &index_php, &index_php_len, &f404, &f404_len, &mimeoverride, &rewrite) == FAILURE) {
		return;
	}

	phar_request_initialize(TSRMLS_C);
	fname = (char*)zend_get_executed_filename(TSRMLS_C);
	fname_len = strlen(fname);

	if (phar_open_executed_filename(alias, alias_len, &error TSRMLS_CC) != SUCCESS) {
		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			efree(error);
		}
		return;
	}

	/* retrieve requested file within phar */
	if (!(SG(request_info).request_method && SG(request_info).request_uri && (!strcmp(SG(request_info).request_method, "GET") || !strcmp(SG(request_info).request_method, "POST")))) {
		return;
	}

#ifdef PHP_WIN32
	fname = estrndup(fname, fname_len);
	phar_unixify_path_separators(fname, fname_len);
#endif
	basename = zend_memrchr(fname, '/', fname_len);

	if (!basename) {
		basename = fname;
	} else {
		++basename;
	}

	if ((strlen(sapi_module.name) == sizeof("cgi-fcgi")-1 && !strncmp(sapi_module.name, "cgi-fcgi", sizeof("cgi-fcgi")-1))
		|| (strlen(sapi_module.name) == sizeof("fpm-fcgi")-1 && !strncmp(sapi_module.name, "fpm-fcgi", sizeof("fpm-fcgi")-1))
		|| (strlen(sapi_module.name) == sizeof("cgi")-1 && !strncmp(sapi_module.name, "cgi", sizeof("cgi")-1))) {

		if (PG(http_globals)[TRACK_VARS_SERVER]) {
			HashTable *_server = Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_SERVER]);
			zval **z_script_name, **z_path_info;

			if (SUCCESS != zend_hash_find(_server, "SCRIPT_NAME", sizeof("SCRIPT_NAME"), (void**)&z_script_name) ||
				IS_STRING != Z_TYPE_PP(z_script_name) ||
				!strstr(Z_STRVAL_PP(z_script_name), basename)) {
				return;
			}

			if (SUCCESS == zend_hash_find(_server, "PATH_INFO", sizeof("PATH_INFO"), (void**)&z_path_info) &&
				IS_STRING == Z_TYPE_PP(z_path_info)) {
				entry_len = Z_STRLEN_PP(z_path_info);
				entry = estrndup(Z_STRVAL_PP(z_path_info), entry_len);
				path_info = emalloc(Z_STRLEN_PP(z_script_name) + entry_len + 1);
				memcpy(path_info, Z_STRVAL_PP(z_script_name), Z_STRLEN_PP(z_script_name));
				memcpy(path_info + Z_STRLEN_PP(z_script_name), entry, entry_len + 1);
				free_pathinfo = 1;
			} else {
				entry_len = 0;
				entry = estrndup("", 0);
				path_info = Z_STRVAL_PP(z_script_name);
			}

			pt = estrndup(Z_STRVAL_PP(z_script_name), Z_STRLEN_PP(z_script_name));

		} else {
			char *testit;

			testit = sapi_getenv("SCRIPT_NAME", sizeof("SCRIPT_NAME")-1 TSRMLS_CC);
			if (!(pt = strstr(testit, basename))) {
				efree(testit);
				return;
			}

			path_info = sapi_getenv("PATH_INFO", sizeof("PATH_INFO")-1 TSRMLS_CC);

			if (path_info) {
				entry = path_info;
				entry_len = strlen(entry);
				spprintf(&path_info, 0, "%s%s", testit, path_info);
				free_pathinfo = 1;
			} else {
				path_info = testit;
				free_pathinfo = 1;
				entry = estrndup("", 0);
				entry_len = 0;
			}

			pt = estrndup(testit, (pt - testit) + (fname_len - (basename - fname)));
		}
		not_cgi = 0;
	} else {
		path_info = SG(request_info).request_uri;

		if (!(pt = strstr(path_info, basename))) {
			/* this can happen with rewrite rules - and we have no idea what to do then, so return */
			return;
		}

		entry_len = strlen(path_info);
		entry_len -= (pt - path_info) + (fname_len - (basename - fname));
		entry = estrndup(pt + (fname_len - (basename - fname)), entry_len);

		pt = estrndup(path_info, (pt - path_info) + (fname_len - (basename - fname)));
		not_cgi = 1;
	}

	if (rewrite) {
		zend_fcall_info fci;
		zend_fcall_info_cache fcc;
		zval *params, *retval_ptr, **zp[1];

		MAKE_STD_ZVAL(params);
		ZVAL_STRINGL(params, entry, entry_len, 1);
		zp[0] = &params;

#if PHP_VERSION_ID < 50300
		if (FAILURE == zend_fcall_info_init(rewrite, &fci, &fcc TSRMLS_CC)) {
#else
		if (FAILURE == zend_fcall_info_init(rewrite, 0, &fci, &fcc, NULL, NULL TSRMLS_CC)) {
#endif
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar error: invalid rewrite callback");

			if (free_pathinfo) {
				efree(path_info);
			}

			return;
		}

		fci.param_count = 1;
		fci.params = zp;
#if PHP_VERSION_ID < 50300
		++(params->refcount);
#else
		Z_ADDREF_P(params);
#endif
		fci.retval_ptr_ptr = &retval_ptr;

		if (FAILURE == zend_call_function(&fci, &fcc TSRMLS_CC)) {
			if (!EG(exception)) {
				zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar error: failed to call rewrite callback");
			}

			if (free_pathinfo) {
				efree(path_info);
			}

			return;
		}

		if (!fci.retval_ptr_ptr || !retval_ptr) {
			if (free_pathinfo) {
				efree(path_info);
			}
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar error: rewrite callback must return a string or false");
			return;
		}

		switch (Z_TYPE_P(retval_ptr)) {
#if PHP_VERSION_ID >= 60000
			case IS_UNICODE:
				zval_unicode_to_string(retval_ptr TSRMLS_CC);
				/* break intentionally omitted */
#endif
			case IS_STRING:
				efree(entry);

				if (fci.retval_ptr_ptr != &retval_ptr) {
					entry = estrndup(Z_STRVAL_PP(fci.retval_ptr_ptr), Z_STRLEN_PP(fci.retval_ptr_ptr));
					entry_len = Z_STRLEN_PP(fci.retval_ptr_ptr);
				} else {
					entry = Z_STRVAL_P(retval_ptr);
					entry_len = Z_STRLEN_P(retval_ptr);
				}

				break;
			case IS_BOOL:
				phar_do_403(entry, entry_len TSRMLS_CC);

				if (free_pathinfo) {
					efree(path_info);
				}

				zend_bailout();
				return;
			default:
				efree(retval_ptr);

				if (free_pathinfo) {
					efree(path_info);
				}

				zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar error: rewrite callback must return a string or false");
				return;
		}
	}

	if (entry_len) {
		phar_postprocess_ru_web(fname, fname_len, &entry, &entry_len, &ru, &ru_len TSRMLS_CC);
	}

	if (!entry_len || (entry_len == 1 && entry[0] == '/')) {
		efree(entry);
		/* direct request */
		if (index_php_len) {
			entry = index_php;
			entry_len = index_php_len;
			if (entry[0] != '/') {
				spprintf(&entry, 0, "/%s", index_php);
				++entry_len;
			}
		} else {
			/* assume "index.php" is starting point */
			entry = estrndup("/index.php", sizeof("/index.php"));
			entry_len = sizeof("/index.php")-1;
		}

		if (FAILURE == phar_get_archive(&phar, fname, fname_len, NULL, 0, NULL TSRMLS_CC) ||
			(info = phar_get_entry_info(phar, entry, entry_len, NULL, 0 TSRMLS_CC)) == NULL) {
			phar_do_404(phar, fname, fname_len, f404, f404_len, entry, entry_len TSRMLS_CC);

			if (free_pathinfo) {
				efree(path_info);
			}

			zend_bailout();
		} else {
			char *tmp = NULL, sa = '\0';
			sapi_header_line ctr = {0};
			ctr.response_code = 301;
			ctr.line_len = sizeof("HTTP/1.1 301 Moved Permanently")-1;
			ctr.line = "HTTP/1.1 301 Moved Permanently";
			sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC);

			if (not_cgi) {
				tmp = strstr(path_info, basename) + fname_len;
				sa = *tmp;
				*tmp = '\0';
			}

			ctr.response_code = 0;

			if (path_info[strlen(path_info)-1] == '/') {
				ctr.line_len = spprintf(&(ctr.line), 4096, "Location: %s%s", path_info, entry + 1);
			} else {
				ctr.line_len = spprintf(&(ctr.line), 4096, "Location: %s%s", path_info, entry);
			}

			if (not_cgi) {
				*tmp = sa;
			}

			if (free_pathinfo) {
				efree(path_info);
			}

			sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC);
			sapi_send_headers(TSRMLS_C);
			efree(ctr.line);
			zend_bailout();
		}
	}

	if (FAILURE == phar_get_archive(&phar, fname, fname_len, NULL, 0, NULL TSRMLS_CC) ||
		(info = phar_get_entry_info(phar, entry, entry_len, NULL, 0 TSRMLS_CC)) == NULL) {
		phar_do_404(phar, fname, fname_len, f404, f404_len, entry, entry_len TSRMLS_CC);
#ifdef PHP_WIN32
		efree(fname);
#endif
		zend_bailout();
	}

	if (mimeoverride && zend_hash_num_elements(Z_ARRVAL_P(mimeoverride))) {
		const char *ext = zend_memrchr(entry, '.', entry_len);
		zval **val;

		if (ext) {
			++ext;

			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(mimeoverride), ext, strlen(ext)+1, (void **) &val)) {
				switch (Z_TYPE_PP(val)) {
					case IS_LONG:
						if (Z_LVAL_PP(val) == PHAR_MIME_PHP || Z_LVAL_PP(val) == PHAR_MIME_PHPS) {
							mime_type = "";
							code = Z_LVAL_PP(val);
						} else {
							zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Unknown mime type specifier used, only Phar::PHP, Phar::PHPS and a mime type string are allowed");
#ifdef PHP_WIN32
							efree(fname);
#endif
							RETURN_FALSE;
						}
						break;
					case IS_STRING:
						mime_type = Z_STRVAL_PP(val);
						code = PHAR_MIME_OTHER;
						break;
					default:
						zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Unknown mime type specifier used (not a string or int), only Phar::PHP, Phar::PHPS and a mime type string are allowed");
#ifdef PHP_WIN32
						efree(fname);
#endif
						RETURN_FALSE;
				}
			}
		}
	}

	if (!mime_type) {
		code = phar_file_type(&PHAR_G(mime_types), entry, &mime_type TSRMLS_CC);
	}
	ret = phar_file_action(phar, info, mime_type, code, entry, entry_len, fname, pt, ru, ru_len TSRMLS_CC);
}
/* }}} */

/* {{{ proto void Phar::mungServer(array munglist)
 * Defines a list of up to 4 $_SERVER variables that should be modified for execution
 * to mask the presence of the phar archive.  This should be used in conjunction with
 * Phar::webPhar(), and has no effect otherwise
 * SCRIPT_NAME, PHP_SELF, REQUEST_URI and SCRIPT_FILENAME
 */
PHP_METHOD(Phar, mungServer)
{
	zval *mungvalues;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &mungvalues) == FAILURE) {
		return;
	}

	if (!zend_hash_num_elements(Z_ARRVAL_P(mungvalues))) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "No values passed to Phar::mungServer(), expecting an array of any of these strings: PHP_SELF, REQUEST_URI, SCRIPT_FILENAME, SCRIPT_NAME");
		return;
	}

	if (zend_hash_num_elements(Z_ARRVAL_P(mungvalues)) > 4) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Too many values passed to Phar::mungServer(), expecting an array of any of these strings: PHP_SELF, REQUEST_URI, SCRIPT_FILENAME, SCRIPT_NAME");
		return;
	}

	phar_request_initialize(TSRMLS_C);

	for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(mungvalues)); SUCCESS == zend_hash_has_more_elements(Z_ARRVAL_P(mungvalues)); zend_hash_move_forward(Z_ARRVAL_P(mungvalues))) {
		zval **data = NULL;

		if (SUCCESS != zend_hash_get_current_data(Z_ARRVAL_P(mungvalues), (void **) &data)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "unable to retrieve array value in Phar::mungServer()");
			return;
		}

		if (Z_TYPE_PP(data) != IS_STRING) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Non-string value passed to Phar::mungServer(), expecting an array of any of these strings: PHP_SELF, REQUEST_URI, SCRIPT_FILENAME, SCRIPT_NAME");
			return;
		}

		if (Z_STRLEN_PP(data) == sizeof("PHP_SELF")-1 && !strncmp(Z_STRVAL_PP(data), "PHP_SELF", sizeof("PHP_SELF")-1)) {
			PHAR_GLOBALS->phar_SERVER_mung_list |= PHAR_MUNG_PHP_SELF;
		}

		if (Z_STRLEN_PP(data) == sizeof("REQUEST_URI")-1) {
			if (!strncmp(Z_STRVAL_PP(data), "REQUEST_URI", sizeof("REQUEST_URI")-1)) {
				PHAR_GLOBALS->phar_SERVER_mung_list |= PHAR_MUNG_REQUEST_URI;
			}
			if (!strncmp(Z_STRVAL_PP(data), "SCRIPT_NAME", sizeof("SCRIPT_NAME")-1)) {
				PHAR_GLOBALS->phar_SERVER_mung_list |= PHAR_MUNG_SCRIPT_NAME;
			}
		}

		if (Z_STRLEN_PP(data) == sizeof("SCRIPT_FILENAME")-1 && !strncmp(Z_STRVAL_PP(data), "SCRIPT_FILENAME", sizeof("SCRIPT_FILENAME")-1)) {
			PHAR_GLOBALS->phar_SERVER_mung_list |= PHAR_MUNG_SCRIPT_FILENAME;
		}
	}
}
/* }}} */

/* {{{ proto void Phar::interceptFileFuncs()
 * instructs phar to intercept fopen, file_get_contents, opendir, and all of the stat-related functions
 * and return stat on files within the phar for relative paths
 *
 * Once called, this cannot be reversed, and continue until the end of the request.
 *
 * This allows legacy scripts to be pharred unmodified
 */
PHP_METHOD(Phar, interceptFileFuncs)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	phar_intercept_functions(TSRMLS_C);
}
/* }}} */

/* {{{ proto array Phar::createDefaultStub([string indexfile[, string webindexfile]])
 * Return a stub that can be used to run a phar-based archive without the phar extension
 * indexfile is the CLI startup filename, which defaults to "index.php", webindexfile
 * is the web startup filename, and also defaults to "index.php"
 */
PHP_METHOD(Phar, createDefaultStub)
{
	char *index = NULL, *webindex = NULL, *stub, *error;
	int index_len = 0, webindex_len = 0;
	size_t stub_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ss", &index, &index_len, &webindex, &webindex_len) == FAILURE) {
		return;
	}

	stub = phar_create_default_stub(index, webindex, &stub_len, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
		return;
	}
	RETURN_STRINGL(stub, stub_len, 0);
}
/* }}} */

/* {{{ proto mixed Phar::mapPhar([string alias, [int dataoffset]])
 * Reads the currently executed file (a phar) and registers its manifest */
PHP_METHOD(Phar, mapPhar)
{
	char *alias = NULL, *error;
	int alias_len = 0;
	long dataoffset = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!l", &alias, &alias_len, &dataoffset) == FAILURE) {
		return;
	}

	phar_request_initialize(TSRMLS_C);

	RETVAL_BOOL(phar_open_executed_filename(alias, alias_len, &error TSRMLS_CC) == SUCCESS);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}
} /* }}} */

/* {{{ proto mixed Phar::loadPhar(string filename [, string alias])
 * Loads any phar archive with an alias */
PHP_METHOD(Phar, loadPhar)
{
	char *fname, *alias = NULL, *error;
	int fname_len, alias_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s!", &fname, &fname_len, &alias, &alias_len) == FAILURE) {
		return;
	}

	phar_request_initialize(TSRMLS_C);

	RETVAL_BOOL(phar_open_from_filename(fname, fname_len, alias, alias_len, REPORT_ERRORS, NULL, &error TSRMLS_CC) == SUCCESS);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}
} /* }}} */

/* {{{ proto string Phar::apiVersion()
 * Returns the api version */
PHP_METHOD(Phar, apiVersion)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_STRINGL(PHP_PHAR_API_VERSION, sizeof(PHP_PHAR_API_VERSION)-1, 1);
}
/* }}}*/

/* {{{ proto bool Phar::canCompress([int method])
 * Returns whether phar extension supports compression using zlib/bzip2 */
PHP_METHOD(Phar, canCompress)
{
	long method = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &method) == FAILURE) {
		return;
	}

	phar_request_initialize(TSRMLS_C);
	switch (method) {
	case PHAR_ENT_COMPRESSED_GZ:
		if (PHAR_G(has_zlib)) {
			RETURN_TRUE;
		} else {
			RETURN_FALSE;
		}
	case PHAR_ENT_COMPRESSED_BZ2:
		if (PHAR_G(has_bz2)) {
			RETURN_TRUE;
		} else {
			RETURN_FALSE;
		}
	default:
		if (PHAR_G(has_zlib) || PHAR_G(has_bz2)) {
			RETURN_TRUE;
		} else {
			RETURN_FALSE;
		}
	}
}
/* }}} */

/* {{{ proto bool Phar::canWrite()
 * Returns whether phar extension supports writing and creating phars */
PHP_METHOD(Phar, canWrite)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_BOOL(!PHAR_G(readonly));
}
/* }}} */

/* {{{ proto bool Phar::isValidPharFilename(string filename[, bool executable = true])
 * Returns whether the given filename is a valid phar filename */
PHP_METHOD(Phar, isValidPharFilename)
{
	char *fname;
	const char *ext_str;
	int fname_len, ext_len, is_executable;
	zend_bool executable = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &fname, &fname_len, &executable) == FAILURE) {
		return;
	}

	is_executable = executable;
	RETVAL_BOOL(phar_detect_phar_fname_ext(fname, fname_len, &ext_str, &ext_len, is_executable, 2, 1 TSRMLS_CC) == SUCCESS);
}
/* }}} */

#if HAVE_SPL
/**
 * from spl_directory
 */
static void phar_spl_foreign_dtor(spl_filesystem_object *object TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar = (phar_archive_data *) object->oth;

	if (!phar->is_persistent) {
		phar_archive_delref(phar TSRMLS_CC);
	}

	object->oth = NULL;
}
/* }}} */

/**
 * from spl_directory
 */
static void phar_spl_foreign_clone(spl_filesystem_object *src, spl_filesystem_object *dst TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar_data = (phar_archive_data *) dst->oth;

	if (!phar_data->is_persistent) {
		++(phar_data->refcount);
	}
}
/* }}} */

static spl_other_handler phar_spl_foreign_handler = {
	phar_spl_foreign_dtor,
	phar_spl_foreign_clone
};
#endif /* HAVE_SPL */

/* {{{ proto void Phar::__construct(string fname [, int flags [, string alias]])
 * Construct a Phar archive object
 *
 * proto void PharData::__construct(string fname [[, int flags [, string alias]], int file format = Phar::TAR])
 * Construct a PharData archive object
 *
 * This function is used as the constructor for both the Phar and PharData
 * classes, hence the two prototypes above.
 */
PHP_METHOD(Phar, __construct)
{
#if !HAVE_SPL
	zend_throw_exception_ex(zend_exception_get_default(TSRMLS_C), 0 TSRMLS_CC, "Cannot instantiate Phar object without SPL extension");
#else
	char *fname, *alias = NULL, *error, *arch = NULL, *entry = NULL, *save_fname;
	int fname_len, alias_len = 0, arch_len, entry_len, is_data;
#if PHP_VERSION_ID < 50300
	long flags = 0;
#else
	long flags = SPL_FILE_DIR_SKIPDOTS|SPL_FILE_DIR_UNIXPATHS;
#endif
	long format = 0;
	phar_archive_object *phar_obj;
	phar_archive_data   *phar_data;
	zval *zobj = getThis(), arg1, arg2;

	phar_obj = (phar_archive_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	is_data = instanceof_function(Z_OBJCE_P(zobj), phar_ce_data TSRMLS_CC);

	if (is_data) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ls!l", &fname, &fname_len, &flags, &alias, &alias_len, &format) == FAILURE) {
			return;
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ls!", &fname, &fname_len, &flags, &alias, &alias_len) == FAILURE) {
			return;
		}
	}

	if (phar_obj->arc.archive) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Cannot call constructor twice");
		return;
	}

	save_fname = fname;
	if (SUCCESS == phar_split_fname(fname, fname_len, &arch, &arch_len, &entry, &entry_len, !is_data, 2 TSRMLS_CC)) {
		/* use arch (the basename for the archive) for fname instead of fname */
		/* this allows support for RecursiveDirectoryIterator of subdirectories */
#ifdef PHP_WIN32
		phar_unixify_path_separators(arch, arch_len);
#endif
		fname = arch;
		fname_len = arch_len;
#ifdef PHP_WIN32
	} else {
		arch = estrndup(fname, fname_len);
		arch_len = fname_len;
		fname = arch;
		phar_unixify_path_separators(arch, arch_len);
#endif
	}

	if (phar_open_or_create_filename(fname, fname_len, alias, alias_len, is_data, REPORT_ERRORS, &phar_data, &error TSRMLS_CC) == FAILURE) {

		if (fname == arch && fname != save_fname) {
			efree(arch);
			fname = save_fname;
		}

		if (entry) {
			efree(entry);
		}

		if (error) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"%s", error);
			efree(error);
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"Phar creation or opening failed");
		}

		return;
	}

	if (is_data && phar_data->is_tar && phar_data->is_brandnew && format == PHAR_FORMAT_ZIP) {
		phar_data->is_zip = 1;
		phar_data->is_tar = 0;
	}

	if (fname == arch) {
		efree(arch);
		fname = save_fname;
	}

	if ((is_data && !phar_data->is_data) || (!is_data && phar_data->is_data)) {
		if (is_data) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"PharData class can only be used for non-executable tar and zip archives");
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"Phar class can only be used for executable tar and zip archives");
		}
		efree(entry);
		return;
	}

	is_data = phar_data->is_data;

	if (!phar_data->is_persistent) {
		++(phar_data->refcount);
	}

	phar_obj->arc.archive = phar_data;
	phar_obj->spl.oth_handler = &phar_spl_foreign_handler;

	if (entry) {
		fname_len = spprintf(&fname, 0, "phar://%s%s", phar_data->fname, entry);
		efree(entry);
	} else {
		fname_len = spprintf(&fname, 0, "phar://%s", phar_data->fname);
	}

	INIT_PZVAL(&arg1);
	ZVAL_STRINGL(&arg1, fname, fname_len, 0);
        INIT_PZVAL(&arg2);
        ZVAL_LONG(&arg2, flags);
 
       zend_call_method_with_2_params(&zobj, Z_OBJCE_P(zobj),
                &spl_ce_RecursiveDirectoryIterator->constructor, "__construct", NULL, &arg1, &arg2);
 
        if (!phar_data->is_persistent) {
		phar_obj->arc.archive->is_data = is_data;
	} else if (!EG(exception)) {
		/* register this guy so we can modify if necessary */
		zend_hash_add(&PHAR_GLOBALS->phar_persist_map, (const char *) phar_obj->arc.archive, sizeof(phar_obj->arc.archive), (void *) &phar_obj, sizeof(phar_archive_object **), NULL);
	}

	phar_obj->spl.info_class = phar_ce_entry;
	efree(fname);
#endif /* HAVE_SPL */
}
/* }}} */

/* {{{ proto array Phar::getSupportedSignatures()
 * Return array of supported signature types
 */
PHP_METHOD(Phar, getSupportedSignatures)
{
        if (zend_parse_parameters_none() == FAILURE) {
                return;
        }

        array_init(return_value);
 
        add_next_index_stringl(return_value, "MD5", 3, 1);
	add_next_index_stringl(return_value, "SHA-1", 5, 1);
#ifdef PHAR_HASH_OK
	add_next_index_stringl(return_value, "SHA-256", 7, 1);
	add_next_index_stringl(return_value, "SHA-512", 7, 1);
#endif
#if PHAR_HAVE_OPENSSL
	add_next_index_stringl(return_value, "OpenSSL", 7, 1);
#else
	if (zend_hash_exists(&module_registry, "openssl", sizeof("openssl"))) {
		add_next_index_stringl(return_value, "OpenSSL", 7, 1);
	}
#endif
}
/* }}} */

/* {{{ proto array Phar::getSupportedCompression()
 * Return array of supported comparession algorithms
 */
PHP_METHOD(Phar, getSupportedCompression)
{
        if (zend_parse_parameters_none() == FAILURE) {
                return;
        }

        array_init(return_value);
        phar_request_initialize(TSRMLS_C);
 
	if (PHAR_G(has_zlib)) {
		add_next_index_stringl(return_value, "GZ", 2, 1);
	}

	if (PHAR_G(has_bz2)) {
		add_next_index_stringl(return_value, "BZIP2", 5, 1);
	}
}
/* }}} */

/* {{{ proto array Phar::unlinkArchive(string archive)
 * Completely remove a phar archive from memory and disk
 */
PHP_METHOD(Phar, unlinkArchive)
{
	char *fname, *error, *zname, *arch, *entry;
	int fname_len, zname_len, arch_len, entry_len;
	phar_archive_data *phar;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &fname, &fname_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (!fname_len) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Unknown phar archive \"\"");
		return;
	}

	if (FAILURE == phar_open_from_filename(fname, fname_len, NULL, 0, REPORT_ERRORS, &phar, &error TSRMLS_CC)) {
		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Unknown phar archive \"%s\": %s", fname, error);
			efree(error);
		} else {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Unknown phar archive \"%s\"", fname);
		}
		return;
	}

	zname = (char*)zend_get_executed_filename(TSRMLS_C);
	zname_len = strlen(zname);

	if (zname_len > 7 && !memcmp(zname, "phar://", 7) && SUCCESS == phar_split_fname(zname, zname_len, &arch, &arch_len, &entry, &entry_len, 2, 0 TSRMLS_CC)) {
		if (arch_len == fname_len && !memcmp(arch, fname, arch_len)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar archive \"%s\" cannot be unlinked from within itself", fname);
			efree(arch);
			efree(entry);
			return;
		}
		efree(arch);
		efree(entry);
	}

	if (phar->is_persistent) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar archive \"%s\" is in phar.cache_list, cannot unlinkArchive()", fname);
		return;
	}

	if (phar->refcount) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar archive \"%s\" has open file handles or objects.  fclose() all file handles, and unset() all objects prior to calling unlinkArchive()", fname);
		return;
	}

	fname = estrndup(phar->fname, phar->fname_len);

	/* invalidate phar cache */
	PHAR_G(last_phar) = NULL;
	PHAR_G(last_phar_name) = PHAR_G(last_alias) = NULL;

	phar_archive_delref(phar TSRMLS_CC);
	unlink(fname);
	efree(fname);
	RETURN_TRUE;
}
/* }}} */

#if HAVE_SPL

#define PHAR_ARCHIVE_OBJECT() \
	phar_archive_object *phar_obj = (phar_archive_object*)zend_object_store_get_object(getThis() TSRMLS_CC); \
	if (!phar_obj->arc.archive) { \
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Cannot call method on an uninitialized Phar object"); \
		return; \
	}

/* {{{ proto void Phar::__destruct()
 * if persistent, remove from the cache
 */
PHP_METHOD(Phar, __destruct)
{
	phar_archive_object *phar_obj = (phar_archive_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (phar_obj->arc.archive && phar_obj->arc.archive->is_persistent) {
		zend_hash_del(&PHAR_GLOBALS->phar_persist_map, (const char *) phar_obj->arc.archive, sizeof(phar_obj->arc.archive));
	}
}
/* }}} */

struct _phar_t {
	phar_archive_object *p;
	zend_class_entry *c;
	char *b;
	uint l;
	zval *ret;
	int count;
	php_stream *fp;
};

static int phar_build(zend_object_iterator *iter, void *puser TSRMLS_DC) /* {{{ */
{
	zval **value;
	zend_uchar key_type;
	zend_bool close_fp = 1;
	ulong int_key;
	struct _phar_t *p_obj = (struct _phar_t*) puser;
	uint str_key_len, base_len = p_obj->l, fname_len;
	phar_entry_data *data;
	php_stream *fp;
	size_t contents_len;
	char *fname, *error = NULL, *base = p_obj->b, *opened, *save = NULL, *temp = NULL;
	phar_zstr key;
	char *str_key;
	zend_class_entry *ce = p_obj->c;
	phar_archive_object *phar_obj = p_obj->p;
	char *str = "[stream]";

	iter->funcs->get_current_data(iter, &value TSRMLS_CC);

	if (EG(exception)) {
		return ZEND_HASH_APPLY_STOP;
	}

	if (!value) {
		/* failure in get_current_data */
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Iterator %v returned no value", ce->name);
		return ZEND_HASH_APPLY_STOP;
	}

	switch (Z_TYPE_PP(value)) {
#if PHP_VERSION_ID >= 60000
		case IS_UNICODE:
			zval_unicode_to_string(*(value) TSRMLS_CC);
			/* break intentionally omitted */
#endif
		case IS_STRING:
			break;
		case IS_RESOURCE:
			php_stream_from_zval_no_verify(fp, value);

			if (!fp) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Iterator %v returned an invalid stream handle", ce->name);
				return ZEND_HASH_APPLY_STOP;
			}

			if (iter->funcs->get_current_key) {
				key_type = iter->funcs->get_current_key(iter, &key, &str_key_len, &int_key TSRMLS_CC);

				if (EG(exception)) {
					return ZEND_HASH_APPLY_STOP;
				}

				if (key_type == HASH_KEY_IS_LONG) {
					zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Iterator %v returned an invalid key (must return a string)", ce->name);
					return ZEND_HASH_APPLY_STOP;
				}

				if (key_type > 9) { /* IS_UNICODE == 10 */
#if PHP_VERSION_ID < 60000
/* this can never happen, but fixes a compile warning */
					spprintf(&str_key, 0, "%s", key);
#else
					spprintf(&str_key, 0, "%v", key);
					ezfree(key);
#endif
				} else {
					PHAR_STR(key, str_key);
				}

				save = str_key;

				if (str_key[str_key_len - 1] == '\0') {
					str_key_len--;
				}

			} else {
				zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Iterator %v returned an invalid key (must return a string)", ce->name);
				return ZEND_HASH_APPLY_STOP;
			}

			close_fp = 0;
			opened = (char *) estrndup(str, sizeof("[stream]") - 1);
			goto after_open_fp;
		case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_PP(value), spl_ce_SplFileInfo TSRMLS_CC)) {
				char *test = NULL;
				zval dummy;
				spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(*value TSRMLS_CC);

				if (!base_len) {
					zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Iterator %v returns an SplFileInfo object, so base directory must be specified", ce->name);
					return ZEND_HASH_APPLY_STOP;
				}

				switch (intern->type) {
					case SPL_FS_DIR:
#if PHP_VERSION_ID >= 60000
						test = spl_filesystem_object_get_path(intern, NULL, NULL TSRMLS_CC).s;
#elif PHP_VERSION_ID >= 50300
						test = spl_filesystem_object_get_path(intern, NULL TSRMLS_CC);
#else
						test = intern->path;
#endif
						fname_len = spprintf(&fname, 0, "%s%c%s", test, DEFAULT_SLASH, intern->u.dir.entry.d_name);
						php_stat(fname, fname_len, FS_IS_DIR, &dummy TSRMLS_CC);

						if (Z_BVAL(dummy)) {
							/* ignore directories */
							efree(fname);
							return ZEND_HASH_APPLY_KEEP;
						}

						test = expand_filepath(fname, NULL TSRMLS_CC);
						efree(fname);

						if (test) {
							fname = test;
							fname_len = strlen(fname);
						} else {
							zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Could not resolve file path");
							return ZEND_HASH_APPLY_STOP;
						}

						save = fname;
						goto phar_spl_fileinfo;
					case SPL_FS_INFO:
					case SPL_FS_FILE:
#if PHP_VERSION_ID >= 60000
						if (intern->file_name_type == IS_UNICODE) {
							zval zv;

							INIT_ZVAL(zv);
							Z_UNIVAL(zv) = intern->file_name;
							Z_UNILEN(zv) = intern->file_name_len;
							Z_TYPE(zv) = IS_UNICODE;

							zval_copy_ctor(&zv);
							zval_unicode_to_string(&zv TSRMLS_CC);
							fname = expand_filepath(Z_STRVAL(zv), NULL TSRMLS_CC);
							ezfree(Z_UNIVAL(zv));
						} else {
							fname = expand_filepath(intern->file_name.s, NULL TSRMLS_CC);
						}
#else
						fname = expand_filepath(intern->file_name, NULL TSRMLS_CC);
#endif
						if (!fname) {
							zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Could not resolve file path");
							return ZEND_HASH_APPLY_STOP;
						}

						fname_len = strlen(fname);
						save = fname;
						goto phar_spl_fileinfo;
				}
			}
			/* fall-through */
		default:
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Iterator %v returned an invalid value (must return a string)", ce->name);
			return ZEND_HASH_APPLY_STOP;
	}

	fname = Z_STRVAL_PP(value);
	fname_len = Z_STRLEN_PP(value);

phar_spl_fileinfo:
	if (base_len) {
		temp = expand_filepath(base, NULL TSRMLS_CC);
		if (!temp) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Could not resolve file path");
			if (save) {
				efree(save);
                        }
                        return ZEND_HASH_APPLY_STOP;
                }

                base = temp;
                base_len = strlen(base);
 
		if (strstr(fname, base)) {
			str_key_len = fname_len - base_len;

			if (str_key_len <= 0) {
				if (save) {
					efree(save);
					efree(temp);
				}
				return ZEND_HASH_APPLY_KEEP;
			}

			str_key = fname + base_len;

			if (*str_key == '/' || *str_key == '\\') {
				str_key++;
				str_key_len--;
			}

		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Iterator %v returned a path \"%s\" that is not in the base directory \"%s\"", ce->name, fname, base);

			if (save) {
				efree(save);
				efree(temp);
			}

			return ZEND_HASH_APPLY_STOP;
		}
	} else {
		if (iter->funcs->get_current_key) {
			key_type = iter->funcs->get_current_key(iter, &key, &str_key_len, &int_key TSRMLS_CC);

			if (EG(exception)) {
				return ZEND_HASH_APPLY_STOP;
			}

			if (key_type == HASH_KEY_IS_LONG) {
				zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Iterator %v returned an invalid key (must return a string)", ce->name);
				return ZEND_HASH_APPLY_STOP;
			}

			if (key_type > 9) { /* IS_UNICODE == 10 */
#if PHP_VERSION_ID < 60000
/* this can never happen, but fixes a compile warning */
				spprintf(&str_key, 0, "%s", key);
#else
				spprintf(&str_key, 0, "%v", key);
				ezfree(key);
#endif
			} else {
				PHAR_STR(key, str_key);
			}

			save = str_key;

			if (str_key[str_key_len - 1] == '\0') str_key_len--;
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Iterator %v returned an invalid key (must return a string)", ce->name);
			return ZEND_HASH_APPLY_STOP;
		}
	}
#if PHP_API_VERSION < 20100412
	if (PG(safe_mode) && (!php_checkuid(fname, NULL, CHECKUID_ALLOW_ONLY_FILE))) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Iterator %v returned a path \"%s\" that safe mode prevents opening", ce->name, fname);

		if (save) {
			efree(save);
		}

		if (temp) {
			efree(temp);
		}

		return ZEND_HASH_APPLY_STOP;
	}
#endif

	if (php_check_open_basedir(fname TSRMLS_CC)) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Iterator %v returned a path \"%s\" that open_basedir prevents opening", ce->name, fname);

		if (save) {
			efree(save);
		}

		if (temp) {
			efree(temp);
		}

		return ZEND_HASH_APPLY_STOP;
	}

	/* try to open source file, then create internal phar file and copy contents */
	fp = php_stream_open_wrapper(fname, "rb", STREAM_MUST_SEEK|0, &opened);

	if (!fp) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Iterator %v returned a file that could not be opened \"%s\"", ce->name, fname);

		if (save) {
			efree(save);
		}

		if (temp) {
			efree(temp);
		}

		return ZEND_HASH_APPLY_STOP;
	}
after_open_fp:
	if (str_key_len >= sizeof(".phar")-1 && !memcmp(str_key, ".phar", sizeof(".phar")-1)) {
		/* silently skip any files that would be added to the magic .phar directory */
		if (save) {
			efree(save);
		}

		if (temp) {
			efree(temp);
		}

		if (opened) {
			efree(opened);
		}

		if (close_fp) {
			php_stream_close(fp);
		}

		return ZEND_HASH_APPLY_KEEP;
	}

	if (!(data = phar_get_or_create_entry_data(phar_obj->arc.archive->fname, phar_obj->arc.archive->fname_len, str_key, str_key_len, "w+b", 0, &error, 1 TSRMLS_CC))) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s cannot be created: %s", str_key, error);
		efree(error);

		if (save) {
			efree(save);
		}

		if (opened) {
			efree(opened);
		}

		if (temp) {
			efree(temp);
		}

		if (close_fp) {
			php_stream_close(fp);
		}

		return ZEND_HASH_APPLY_STOP;

	} else {
		if (error) {
			efree(error);
		}
		/* convert to PHAR_UFP */
		if (data->internal_file->fp_type == PHAR_MOD) {
			php_stream_close(data->internal_file->fp);
		}

		data->internal_file->fp = NULL;
		data->internal_file->fp_type = PHAR_UFP;
		data->internal_file->offset_abs = data->internal_file->offset = php_stream_tell(p_obj->fp);
		data->fp = NULL;
		phar_stream_copy_to_stream(fp, p_obj->fp, PHP_STREAM_COPY_ALL, &contents_len);
		data->internal_file->uncompressed_filesize = data->internal_file->compressed_filesize =
			php_stream_tell(p_obj->fp) - data->internal_file->offset;
	}

	if (close_fp) {
		php_stream_close(fp);
	}

	add_assoc_string(p_obj->ret, str_key, opened, 0);

	if (save) {
		efree(save);
	}

	if (temp) {
		efree(temp);
	}

	data->internal_file->compressed_filesize = data->internal_file->uncompressed_filesize = contents_len;
	phar_entry_delref(data TSRMLS_CC);

	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

 /* {{{ proto array Phar::buildFromDirectory(string base_dir[, string regex])
  * Construct a phar archive from an existing directory, recursively.
  * Optional second parameter is a regular expression for filtering directory contents.
 *
  * Return value is an array mapping phar index to actual files added.
  */
 PHP_METHOD(Phar, buildFromDirectory)
{
	char *dir, *error, *regex = NULL;
	int dir_len, regex_len = 0;
	zend_bool apply_reg = 0;
	zval arg, arg2, *iter, *iteriter, *regexiter = NULL;
	struct _phar_t pass;

	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot write to archive - write operations restricted by INI setting");
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &dir, &dir_len, &regex, &regex_len) == FAILURE) {
		RETURN_FALSE;
	}

	MAKE_STD_ZVAL(iter);

	if (SUCCESS != object_init_ex(iter, spl_ce_RecursiveDirectoryIterator)) {
		zval_ptr_dtor(&iter);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Unable to instantiate directory iterator for %s", phar_obj->arc.archive->fname);
		RETURN_FALSE;
	}

	INIT_PZVAL(&arg);
	ZVAL_STRINGL(&arg, dir, dir_len, 0);
	INIT_PZVAL(&arg2);
#if PHP_VERSION_ID < 50300
	ZVAL_LONG(&arg2, 0);
#else
        ZVAL_LONG(&arg2, SPL_FILE_DIR_SKIPDOTS|SPL_FILE_DIR_UNIXPATHS);
 #endif
 
       zend_call_method_with_2_params(&iter, spl_ce_RecursiveDirectoryIterator,
                        &spl_ce_RecursiveDirectoryIterator->constructor, "__construct", NULL, &arg, &arg2);
 
        if (EG(exception)) {
		zval_ptr_dtor(&iter);
		RETURN_FALSE;
	}

	MAKE_STD_ZVAL(iteriter);

	if (SUCCESS != object_init_ex(iteriter, spl_ce_RecursiveIteratorIterator)) {
		zval_ptr_dtor(&iter);
		zval_ptr_dtor(&iteriter);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Unable to instantiate directory iterator for %s", phar_obj->arc.archive->fname);
                RETURN_FALSE;
        }
 
       zend_call_method_with_1_params(&iteriter, spl_ce_RecursiveIteratorIterator,
                        &spl_ce_RecursiveIteratorIterator->constructor, "__construct", NULL, iter);
 
        if (EG(exception)) {
		zval_ptr_dtor(&iter);
		zval_ptr_dtor(&iteriter);
		RETURN_FALSE;
	}

	zval_ptr_dtor(&iter);

	if (regex_len > 0) {
		apply_reg = 1;
		MAKE_STD_ZVAL(regexiter);

		if (SUCCESS != object_init_ex(regexiter, spl_ce_RegexIterator)) {
			zval_ptr_dtor(&iteriter);
			zval_dtor(regexiter);
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Unable to instantiate regex iterator for %s", phar_obj->arc.archive->fname);
			RETURN_FALSE;
		}

                INIT_PZVAL(&arg2);
                ZVAL_STRINGL(&arg2, regex, regex_len, 0);
 
               zend_call_method_with_2_params(&regexiter, spl_ce_RegexIterator,
                        &spl_ce_RegexIterator->constructor, "__construct", NULL, iteriter, &arg2);
        }
 
	array_init(return_value);

	pass.c = apply_reg ? Z_OBJCE_P(regexiter) : Z_OBJCE_P(iteriter);
	pass.p = phar_obj;
	pass.b = dir;
	pass.l = dir_len;
	pass.count = 0;
	pass.ret = return_value;
	pass.fp = php_stream_fopen_tmpfile();
	if (pass.fp == NULL) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" unable to create temporary file", phar_obj->arc.archive->fname);
		return;
	}

	if (phar_obj->arc.archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
		zval_ptr_dtor(&iteriter);
		if (apply_reg) {
			zval_ptr_dtor(&regexiter);
		}
		php_stream_close(pass.fp);
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
		return;
	}

	if (SUCCESS == spl_iterator_apply((apply_reg ? regexiter : iteriter), (spl_iterator_apply_func_t) phar_build, (void *) &pass TSRMLS_CC)) {
		zval_ptr_dtor(&iteriter);

		if (apply_reg) {
			zval_ptr_dtor(&regexiter);
		}

		phar_obj->arc.archive->ufp = pass.fp;
		phar_flush(phar_obj->arc.archive, 0, 0, 0, &error TSRMLS_CC);

		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			efree(error);
		}

	} else {
		zval_ptr_dtor(&iteriter);
		if (apply_reg) {
			zval_ptr_dtor(&regexiter);
		}
		php_stream_close(pass.fp);
	}
}
/* }}} */

/* {{{ proto array Phar::buildFromIterator(Iterator iter[, string base_directory])
 * Construct a phar archive from an iterator.  The iterator must return a series of strings
 * that are full paths to files that should be added to the phar.  The iterator key should
 * be the path that the file will have within the phar archive.
 *
 * If base directory is specified, then the key will be ignored, and instead the portion of
 * the current value minus the base directory will be used
 *
 * Returned is an array mapping phar index to actual file added
 */
PHP_METHOD(Phar, buildFromIterator)
{
	zval *obj;
	char *error;
	uint base_len = 0;
	char *base = NULL;
	struct _phar_t pass;

	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot write out phar archive, phar is read-only");
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|s", &obj, zend_ce_traversable, &base, &base_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (phar_obj->arc.archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
		return;
	}

	array_init(return_value);

	pass.c = Z_OBJCE_P(obj);
	pass.p = phar_obj;
	pass.b = base;
	pass.l = base_len;
	pass.ret = return_value;
	pass.count = 0;
	pass.fp = php_stream_fopen_tmpfile();
	if (pass.fp == NULL) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\": unable to create temporary file", phar_obj->arc.archive->fname);
		return;
	}

	if (SUCCESS == spl_iterator_apply(obj, (spl_iterator_apply_func_t) phar_build, (void *) &pass TSRMLS_CC)) {
		phar_obj->arc.archive->ufp = pass.fp;
		phar_flush(phar_obj->arc.archive, 0, 0, 0, &error TSRMLS_CC);
		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			efree(error);
		}
	} else {
		php_stream_close(pass.fp);
	}
}
/* }}} */

/* {{{ proto int Phar::count()
 * Returns the number of entries in the Phar archive
 */
 PHP_METHOD(Phar, count)
 {
        PHAR_ARCHIVE_OBJECT();

        if (zend_parse_parameters_none() == FAILURE) {
                return;
        }

	RETURN_LONG(zend_hash_num_elements(&phar_obj->arc.archive->manifest));
}
/* }}} */

/* {{{ proto bool Phar::isFileFormat(int format)
 * Returns true if the phar archive is based on the tar/zip/phar file format depending
 * on whether Phar::TAR, Phar::ZIP or Phar::PHAR was passed in
 */
PHP_METHOD(Phar, isFileFormat)
{
	long type;
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &type) == FAILURE) {
		RETURN_FALSE;
	}

	switch (type) {
		case PHAR_FORMAT_TAR:
			RETURN_BOOL(phar_obj->arc.archive->is_tar);
		case PHAR_FORMAT_ZIP:
			RETURN_BOOL(phar_obj->arc.archive->is_zip);
		case PHAR_FORMAT_PHAR:
			RETURN_BOOL(!phar_obj->arc.archive->is_tar && !phar_obj->arc.archive->is_zip);
		default:
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Unknown file format specified");
	}
}
/* }}} */

static int phar_copy_file_contents(phar_entry_info *entry, php_stream *fp TSRMLS_DC) /* {{{ */
{
	char *error;
	off_t offset;
	phar_entry_info *link;

	if (FAILURE == phar_open_entry_fp(entry, &error, 1 TSRMLS_CC)) {
		if (error) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"Cannot convert phar archive \"%s\", unable to open entry \"%s\" contents: %s", entry->phar->fname, entry->filename, error);
			efree(error);
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"Cannot convert phar archive \"%s\", unable to open entry \"%s\" contents", entry->phar->fname, entry->filename);
		}
		return FAILURE;
	}

	/* copy old contents in entirety */
	phar_seek_efp(entry, 0, SEEK_SET, 0, 1 TSRMLS_CC);
	offset = php_stream_tell(fp);
	link = phar_get_link_source(entry TSRMLS_CC);

	if (!link) {
		link = entry;
	}

	if (SUCCESS != phar_stream_copy_to_stream(phar_get_efp(link, 0 TSRMLS_CC), fp, link->uncompressed_filesize, NULL)) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot convert phar archive \"%s\", unable to copy entry \"%s\" contents", entry->phar->fname, entry->filename);
		return FAILURE;
	}

	if (entry->fp_type == PHAR_MOD) {
		/* save for potential restore on error */
		entry->cfp = entry->fp;
		entry->fp = NULL;
	}

	/* set new location of file contents */
	entry->fp_type = PHAR_FP;
	entry->offset = offset;
	return SUCCESS;
}
/* }}} */

static zval *phar_rename_archive(phar_archive_data *phar, char *ext, zend_bool compress TSRMLS_DC) /* {{{ */
{
	const char *oldname = NULL;
	char *oldpath = NULL;
	char *basename = NULL, *basepath = NULL;
	char *newname = NULL, *newpath = NULL;
	zval *ret, arg1;
	zend_class_entry *ce;
	char *error;
	const char *pcr_error;
	int ext_len = ext ? strlen(ext) : 0;
	int oldname_len;
	phar_archive_data **pphar = NULL;
	php_stream_statbuf ssb;

	if (!ext) {
		if (phar->is_zip) {

			if (phar->is_data) {
				ext = "zip";
			} else {
				ext = "phar.zip";
			}

		} else if (phar->is_tar) {

			switch (phar->flags) {
				case PHAR_FILE_COMPRESSED_GZ:
					if (phar->is_data) {
						ext = "tar.gz";
					} else {
						ext = "phar.tar.gz";
					}
					break;
				case PHAR_FILE_COMPRESSED_BZ2:
					if (phar->is_data) {
						ext = "tar.bz2";
					} else {
						ext = "phar.tar.bz2";
					}
					break;
				default:
					if (phar->is_data) {
						ext = "tar";
					} else {
						ext = "phar.tar";
					}
			}
		} else {

			switch (phar->flags) {
				case PHAR_FILE_COMPRESSED_GZ:
					ext = "phar.gz";
					break;
				case PHAR_FILE_COMPRESSED_BZ2:
					ext = "phar.bz2";
					break;
				default:
					ext = "phar";
			}
		}
	} else if (phar_path_check(&ext, &ext_len, &pcr_error) > pcr_is_ok) {

		if (phar->is_data) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "data phar converted from \"%s\" has invalid extension %s", phar->fname, ext);
		} else {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "phar converted from \"%s\" has invalid extension %s", phar->fname, ext);
		}
		return NULL;
	}

	if (ext[0] == '.') {
		++ext;
	}

	oldpath = estrndup(phar->fname, phar->fname_len);
	oldname = zend_memrchr(phar->fname, '/', phar->fname_len);
	++oldname;
	oldname_len = strlen(oldname);

	basename = estrndup(oldname, oldname_len);
        spprintf(&newname, 0, "%s.%s", strtok(basename, "."), ext);
        efree(basename);
 

 
        basepath = estrndup(oldpath, (strlen(oldpath) - oldname_len));
        phar->fname_len = spprintf(&newpath, 0, "%s%s", basepath, newname);
	phar->fname = newpath;
	phar->ext = newpath + phar->fname_len - strlen(ext) - 1;
	efree(basepath);
	efree(newname);

	if (PHAR_G(manifest_cached) && SUCCESS == zend_hash_find(&cached_phars, newpath, phar->fname_len, (void **) &pphar)) {
		efree(oldpath);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Unable to add newly converted phar \"%s\" to the list of phars, new phar name is in phar.cache_list", phar->fname);
		return NULL;
	}

	if (SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_fname_map), newpath, phar->fname_len, (void **) &pphar)) {
		if ((*pphar)->fname_len == phar->fname_len && !memcmp((*pphar)->fname, phar->fname, phar->fname_len)) {
			if (!zend_hash_num_elements(&phar->manifest)) {
				(*pphar)->is_tar = phar->is_tar;
				(*pphar)->is_zip = phar->is_zip;
				(*pphar)->is_data = phar->is_data;
				(*pphar)->flags = phar->flags;
				(*pphar)->fp = phar->fp;
				phar->fp = NULL;
				phar_destroy_phar_data(phar TSRMLS_CC);
				phar = *pphar;
				phar->refcount++;
				newpath = oldpath;
				goto its_ok;
			}
		}

		efree(oldpath);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Unable to add newly converted phar \"%s\" to the list of phars, a phar with that name already exists", phar->fname);
		return NULL;
	}
its_ok:
	if (SUCCESS == php_stream_stat_path(newpath, &ssb)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "phar \"%s\" exists and must be unlinked prior to conversion", newpath);
		efree(oldpath);
		return NULL;
	}
	if (!phar->is_data) {
		if (SUCCESS != phar_detect_phar_fname_ext(newpath, phar->fname_len, (const char **) &(phar->ext), &(phar->ext_len), 1, 1, 1 TSRMLS_CC)) {
			efree(oldpath);
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "phar \"%s\" has invalid extension %s", phar->fname, ext);
			return NULL;
		}

		if (phar->alias) {
			if (phar->is_temporary_alias) {
				phar->alias = NULL;
				phar->alias_len = 0;
			} else {
				phar->alias = estrndup(newpath, strlen(newpath));
				phar->alias_len = strlen(newpath);
				phar->is_temporary_alias = 1;
				zend_hash_update(&(PHAR_GLOBALS->phar_alias_map), newpath, phar->fname_len, (void*)&phar, sizeof(phar_archive_data*), NULL);
			}
		}

	} else {

		if (SUCCESS != phar_detect_phar_fname_ext(newpath, phar->fname_len, (const char **) &(phar->ext), &(phar->ext_len), 0, 1, 1 TSRMLS_CC)) {
			efree(oldpath);
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "data phar \"%s\" has invalid extension %s", phar->fname, ext);
			return NULL;
		}

		phar->alias = NULL;
		phar->alias_len = 0;
	}

	if ((!pphar || phar == *pphar) && SUCCESS != zend_hash_update(&(PHAR_GLOBALS->phar_fname_map), newpath, phar->fname_len, (void*)&phar, sizeof(phar_archive_data*), NULL)) {
		efree(oldpath);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Unable to add newly converted phar \"%s\" to the list of phars", phar->fname);
		return NULL;
	}

	phar_flush(phar, 0, 0, 1, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "%s", error);
		efree(error);
		efree(oldpath);
		return NULL;
	}

	efree(oldpath);

	if (phar->is_data) {
		ce = phar_ce_data;
	} else {
		ce = phar_ce_archive;
	}

	MAKE_STD_ZVAL(ret);

	if (SUCCESS != object_init_ex(ret, ce)) {
		zval_dtor(ret);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Unable to instantiate phar object when converting archive \"%s\"", phar->fname);
		return NULL;
	}

	INIT_PZVAL(&arg1);
	ZVAL_STRINGL(&arg1, phar->fname, phar->fname_len, 0);

	zend_call_method_with_1_params(&ret, ce, &ce->constructor, "__construct", NULL, &arg1);
	return ret;
}
/* }}} */

static zval *phar_convert_to_other(phar_archive_data *source, int convert, char *ext, php_uint32 flags TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar;
	phar_entry_info *entry, newentry;
	zval *ret;

	/* invalidate phar cache */
	PHAR_G(last_phar) = NULL;
	PHAR_G(last_phar_name) = PHAR_G(last_alias) = NULL;

	phar = (phar_archive_data *) ecalloc(1, sizeof(phar_archive_data));
	/* set whole-archive compression and type from parameter */
	phar->flags = flags;
	phar->is_data = source->is_data;

	switch (convert) {
		case PHAR_FORMAT_TAR:
			phar->is_tar = 1;
			break;
		case PHAR_FORMAT_ZIP:
			phar->is_zip = 1;
			break;
		default:
			phar->is_data = 0;
			break;
	}

	zend_hash_init(&(phar->manifest), sizeof(phar_entry_info),
		zend_get_hash_value, destroy_phar_manifest_entry, 0);
	zend_hash_init(&phar->mounted_dirs, sizeof(char *),
		zend_get_hash_value, NULL, 0);
	zend_hash_init(&phar->virtual_dirs, sizeof(char *),
		zend_get_hash_value, NULL, 0);

	phar->fp = php_stream_fopen_tmpfile();
	if (phar->fp == NULL) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "unable to create temporary file");
		return NULL;
	}
	phar->fname = source->fname;
	phar->fname_len = source->fname_len;
	phar->is_temporary_alias = source->is_temporary_alias;
	phar->alias = source->alias;

	if (source->metadata) {
		zval *t;

		t = source->metadata;
		ALLOC_ZVAL(phar->metadata);
		*phar->metadata = *t;
		zval_copy_ctor(phar->metadata);
#if PHP_VERSION_ID < 50300
		phar->metadata->refcount = 1;
#else
		Z_SET_REFCOUNT_P(phar->metadata, 1);
#endif

		phar->metadata_len = 0;
	}

	/* first copy each file's uncompressed contents to a temporary file and set per-file flags */
	for (zend_hash_internal_pointer_reset(&source->manifest); SUCCESS == zend_hash_has_more_elements(&source->manifest); zend_hash_move_forward(&source->manifest)) {

		if (FAILURE == zend_hash_get_current_data(&source->manifest, (void **) &entry)) {
			zend_hash_destroy(&(phar->manifest));
			php_stream_close(phar->fp);
			efree(phar);
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"Cannot convert phar archive \"%s\"", source->fname);
			return NULL;
		}

		newentry = *entry;

		if (newentry.link) {
			newentry.link = estrdup(newentry.link);
			goto no_copy;
		}

		if (newentry.tmp) {
			newentry.tmp = estrdup(newentry.tmp);
			goto no_copy;
		}

		newentry.metadata_str.c = 0;

		if (FAILURE == phar_copy_file_contents(&newentry, phar->fp TSRMLS_CC)) {
			zend_hash_destroy(&(phar->manifest));
			php_stream_close(phar->fp);
			efree(phar);
			/* exception already thrown */
			return NULL;
		}
no_copy:
		newentry.filename = estrndup(newentry.filename, newentry.filename_len);

		if (newentry.metadata) {
			zval *t;

			t = newentry.metadata;
			ALLOC_ZVAL(newentry.metadata);
			*newentry.metadata = *t;
			zval_copy_ctor(newentry.metadata);
#if PHP_VERSION_ID < 50300
			newentry.metadata->refcount = 1;
#else
			Z_SET_REFCOUNT_P(newentry.metadata, 1);
#endif

			newentry.metadata_str.c = NULL;
			newentry.metadata_str.len = 0;
		}

		newentry.is_zip = phar->is_zip;
		newentry.is_tar = phar->is_tar;

		if (newentry.is_tar) {
			newentry.tar_type = (entry->is_dir ? TAR_DIR : TAR_FILE);
		}

		newentry.is_modified = 1;
		newentry.phar = phar;
		newentry.old_flags = newentry.flags & ~PHAR_ENT_COMPRESSION_MASK; /* remove compression from old_flags */
		phar_set_inode(&newentry TSRMLS_CC);
		zend_hash_add(&(phar->manifest), newentry.filename, newentry.filename_len, (void*)&newentry, sizeof(phar_entry_info), NULL);
		phar_add_virtual_dirs(phar, newentry.filename, newentry.filename_len TSRMLS_CC);
	}

	if ((ret = phar_rename_archive(phar, ext, 0 TSRMLS_CC))) {
		return ret;
	} else {
                zend_hash_destroy(&(phar->manifest));
                zend_hash_destroy(&(phar->mounted_dirs));
                zend_hash_destroy(&(phar->virtual_dirs));
               if (phar->fp) {
                       php_stream_close(phar->fp);
               }
                efree(phar->fname);
                efree(phar);
                return NULL;
/* }}} */
