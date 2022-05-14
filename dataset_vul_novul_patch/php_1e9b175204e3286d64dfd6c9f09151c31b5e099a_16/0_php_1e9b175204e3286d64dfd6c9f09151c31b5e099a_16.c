PHP_METHOD(PharFileInfo, __construct)
{
	char *fname, *arch, *entry, *error;
	size_t fname_len;
	int arch_len, entry_len;
	phar_entry_object *entry_obj;
	phar_entry_info *entry_info;
        phar_archive_data *phar_data;
        zval *zobj = getThis(), arg1;
 
       if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "p", &fname, &fname_len) == FAILURE) {
                return;
        }
	entry_obj = (phar_entry_object*)((char*)Z_OBJ_P(zobj) - Z_OBJ_P(zobj)->handlers->offset);

	if (entry_obj->entry) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Cannot call constructor twice");
		return;
	}

	if (fname_len < 7 || memcmp(fname, "phar://", 7) || phar_split_fname(fname, (int)fname_len, &arch, &arch_len, &entry, &entry_len, 2, 0) == FAILURE) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0,
			"'%s' is not a valid phar archive URL (must have at least phar://filename.phar)", fname);
		return;
	}

	if (phar_open_from_filename(arch, arch_len, NULL, 0, REPORT_ERRORS, &phar_data, &error) == FAILURE) {
		efree(arch);
		efree(entry);
		if (error) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0,
				"Cannot open phar file '%s': %s", fname, error);
			efree(error);
		} else {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0,
				"Cannot open phar file '%s'", fname);
		}
		return;
	}

	if ((entry_info = phar_get_entry_info_dir(phar_data, entry, entry_len, 1, &error, 1)) == NULL) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0,
			"Cannot access phar file entry '%s' in archive '%s'%s%s", entry, arch, error ? ", " : "", error ? error : "");
		efree(arch);
		efree(entry);
		return;
	}

	efree(arch);
	efree(entry);

	entry_obj->entry = entry_info;

	ZVAL_STRINGL(&arg1, fname, fname_len);

	zend_call_method_with_1_params(zobj, Z_OBJCE_P(zobj),
		&spl_ce_SplFileInfo->constructor, "__construct", NULL, &arg1);

	zval_ptr_dtor(&arg1);
}
