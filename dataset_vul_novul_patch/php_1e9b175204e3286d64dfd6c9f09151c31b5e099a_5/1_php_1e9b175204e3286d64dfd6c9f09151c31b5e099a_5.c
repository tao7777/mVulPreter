PHP_METHOD(Phar, buildFromDirectory)
{
	char *dir, *error, *regex = NULL;
	size_t dir_len, regex_len = 0;
	zend_bool apply_reg = 0;
	zval arg, arg2, iter, iteriter, regexiter;
	struct _phar_t pass;

	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
			"Cannot write to archive - write operations restricted by INI setting");
                return;
        }
 
       if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s", &dir, &dir_len, &regex, &regex_len) == FAILURE) {
                RETURN_FALSE;
        }
 
	if (SUCCESS != object_init_ex(&iter, spl_ce_RecursiveDirectoryIterator)) {
		zval_ptr_dtor(&iter);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Unable to instantiate directory iterator for %s", phar_obj->archive->fname);
		RETURN_FALSE;
	}

	ZVAL_STRINGL(&arg, dir, dir_len);
	ZVAL_LONG(&arg2, SPL_FILE_DIR_SKIPDOTS|SPL_FILE_DIR_UNIXPATHS);

	zend_call_method_with_2_params(&iter, spl_ce_RecursiveDirectoryIterator,
			&spl_ce_RecursiveDirectoryIterator->constructor, "__construct", NULL, &arg, &arg2);

	zval_ptr_dtor(&arg);
	if (EG(exception)) {
		zval_ptr_dtor(&iter);
		RETURN_FALSE;
	}

	if (SUCCESS != object_init_ex(&iteriter, spl_ce_RecursiveIteratorIterator)) {
		zval_ptr_dtor(&iter);
		zval_ptr_dtor(&iteriter);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Unable to instantiate directory iterator for %s", phar_obj->archive->fname);
		RETURN_FALSE;
	}

	zend_call_method_with_1_params(&iteriter, spl_ce_RecursiveIteratorIterator,
			&spl_ce_RecursiveIteratorIterator->constructor, "__construct", NULL, &iter);

	if (EG(exception)) {
		zval_ptr_dtor(&iter);
		zval_ptr_dtor(&iteriter);
		RETURN_FALSE;
	}

	zval_ptr_dtor(&iter);

	if (regex_len > 0) {
		apply_reg = 1;

		if (SUCCESS != object_init_ex(&regexiter, spl_ce_RegexIterator)) {
			zval_ptr_dtor(&iteriter);
			zval_dtor(&regexiter);
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Unable to instantiate regex iterator for %s", phar_obj->archive->fname);
			RETURN_FALSE;
		}

		ZVAL_STRINGL(&arg2, regex, regex_len);

		zend_call_method_with_2_params(&regexiter, spl_ce_RegexIterator,
			&spl_ce_RegexIterator->constructor, "__construct", NULL, &iteriter, &arg2);
		zval_ptr_dtor(&arg2);
	}

	array_init(return_value);

	pass.c = apply_reg ? Z_OBJCE(regexiter) : Z_OBJCE(iteriter);
	pass.p = phar_obj;
	pass.b = dir;
	pass.l = dir_len;
	pass.count = 0;
	pass.ret = return_value;
	pass.fp = php_stream_fopen_tmpfile();
	if (pass.fp == NULL) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "phar \"%s\" unable to create temporary file", phar_obj->archive->fname);
		return;
	}

	if (phar_obj->archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->archive))) {
		zval_ptr_dtor(&iteriter);
		if (apply_reg) {
			zval_ptr_dtor(&regexiter);
		}
		php_stream_close(pass.fp);
		zend_throw_exception_ex(phar_ce_PharException, 0, "phar \"%s\" is persistent, unable to copy on write", phar_obj->archive->fname);
		return;
	}

	if (SUCCESS == spl_iterator_apply((apply_reg ? &regexiter : &iteriter), (spl_iterator_apply_func_t) phar_build, (void *) &pass)) {
		zval_ptr_dtor(&iteriter);

		if (apply_reg) {
			zval_ptr_dtor(&regexiter);
		}

		phar_obj->archive->ufp = pass.fp;
		phar_flush(phar_obj->archive, 0, 0, 0, &error);

		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
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
