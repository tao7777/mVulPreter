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
