PHP_METHOD(Phar, __construct)
{
#if !HAVE_SPL
	zend_throw_exception_ex(zend_ce_exception, 0, "Cannot instantiate Phar object without SPL extension");
#else
	char *fname, *alias = NULL, *error, *arch = NULL, *entry = NULL, *save_fname;
	size_t fname_len, alias_len = 0;
	int arch_len, entry_len, is_data;
	zend_long flags = SPL_FILE_DIR_SKIPDOTS|SPL_FILE_DIR_UNIXPATHS;
	zend_long format = 0;
	phar_archive_object *phar_obj;
	phar_archive_data   *phar_data;
	zval *zobj = getThis(), arg1, arg2;

	phar_obj = (phar_archive_object*)((char*)Z_OBJ_P(zobj) - Z_OBJ_P(zobj)->handlers->offset);

        is_data = instanceof_function(Z_OBJCE_P(zobj), phar_ce_data);
 
        if (is_data) {
               if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "p|ls!l", &fname, &fname_len, &flags, &alias, &alias_len, &format) == FAILURE) {
                        return;
                }
        } else {
               if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "p|ls!", &fname, &fname_len, &flags, &alias, &alias_len) == FAILURE) {
                        return;
                }
        }

	if (phar_obj->archive) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Cannot call constructor twice");
		return;
	}

	save_fname = fname;
	if (SUCCESS == phar_split_fname(fname, (int)fname_len, &arch, &arch_len, &entry, &entry_len, !is_data, 2)) {
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

	if (phar_open_or_create_filename(fname, fname_len, alias, alias_len, is_data, REPORT_ERRORS, &phar_data, &error) == FAILURE) {

		if (fname == arch && fname != save_fname) {
			efree(arch);
			fname = save_fname;
		}

		if (entry) {
			efree(entry);
		}

		if (error) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
				"%s", error);
			efree(error);
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
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
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
				"PharData class can only be used for non-executable tar and zip archives");
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
				"Phar class can only be used for executable tar and zip archives");
		}
		efree(entry);
		return;
	}

	is_data = phar_data->is_data;

	if (!phar_data->is_persistent) {
		++(phar_data->refcount);
	}

	phar_obj->archive = phar_data;
	phar_obj->spl.oth_handler = &phar_spl_foreign_handler;

	if (entry) {
		fname_len = spprintf(&fname, 0, "phar://%s%s", phar_data->fname, entry);
		efree(entry);
	} else {
		fname_len = spprintf(&fname, 0, "phar://%s", phar_data->fname);
	}

	ZVAL_STRINGL(&arg1, fname, fname_len);
	ZVAL_LONG(&arg2, flags);

	zend_call_method_with_2_params(zobj, Z_OBJCE_P(zobj),
		&spl_ce_RecursiveDirectoryIterator->constructor, "__construct", NULL, &arg1, &arg2);

	zval_ptr_dtor(&arg1);

	if (!phar_data->is_persistent) {
		phar_obj->archive->is_data = is_data;
	} else if (!EG(exception)) {
		/* register this guy so we can modify if necessary */
		zend_hash_str_add_ptr(&PHAR_G(phar_persist_map), (const char *) phar_obj->archive, sizeof(phar_obj->archive), phar_obj);
	}

	phar_obj->spl.info_class = phar_ce_entry;
	efree(fname);
#endif /* HAVE_SPL */
}
