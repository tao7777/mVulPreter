PHP_METHOD(Phar, copy)
{
	char *oldfile, *newfile, *error;
	const char *pcr_error;
	size_t oldfile_len, newfile_len;
	phar_entry_info *oldentry, newentry = {0}, *temp;
	int tmp_len = 0;
 
        PHAR_ARCHIVE_OBJECT();
 
       if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &oldfile, &oldfile_len, &newfile, &newfile_len) == FAILURE) {
                return;
        }
 
	if (PHAR_G(readonly) && !phar_obj->archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
			"Cannot copy \"%s\" to \"%s\", phar is read-only", oldfile, newfile);
		RETURN_FALSE;
	}

	if (oldfile_len >= sizeof(".phar")-1 && !memcmp(oldfile, ".phar", sizeof(".phar")-1)) {
		/* can't copy a meta file */
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
			"file \"%s\" cannot be copied to file \"%s\", cannot copy Phar meta-file in %s", oldfile, newfile, phar_obj->archive->fname);
		RETURN_FALSE;
	}

	if (newfile_len >= sizeof(".phar")-1 && !memcmp(newfile, ".phar", sizeof(".phar")-1)) {
		/* can't copy a meta file */
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
			"file \"%s\" cannot be copied to file \"%s\", cannot copy to Phar meta-file in %s", oldfile, newfile, phar_obj->archive->fname);
		RETURN_FALSE;
	}

	if (!zend_hash_str_exists(&phar_obj->archive->manifest, oldfile, (uint) oldfile_len) || NULL == (oldentry = zend_hash_str_find_ptr(&phar_obj->archive->manifest, oldfile, (uint) oldfile_len)) || oldentry->is_deleted) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
			"file \"%s\" cannot be copied to file \"%s\", file does not exist in %s", oldfile, newfile, phar_obj->archive->fname);
		RETURN_FALSE;
	}

	if (zend_hash_str_exists(&phar_obj->archive->manifest, newfile, (uint) newfile_len)) {
		if (NULL != (temp = zend_hash_str_find_ptr(&phar_obj->archive->manifest, newfile, (uint) newfile_len)) || !temp->is_deleted) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
				"file \"%s\" cannot be copied to file \"%s\", file must not already exist in phar %s", oldfile, newfile, phar_obj->archive->fname);
			RETURN_FALSE;
		}
	}

	tmp_len = (int)newfile_len;
	if (phar_path_check(&newfile, &tmp_len, &pcr_error) > pcr_is_ok) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
				"file \"%s\" contains invalid characters %s, cannot be copied from \"%s\" in phar %s", newfile, pcr_error, oldfile, phar_obj->archive->fname);
		RETURN_FALSE;
	}
	newfile_len = tmp_len;

	if (phar_obj->archive->is_persistent) {
		if (FAILURE == phar_copy_on_write(&(phar_obj->archive))) {
			zend_throw_exception_ex(phar_ce_PharException, 0, "phar \"%s\" is persistent, unable to copy on write", phar_obj->archive->fname);
			return;
		}
		/* re-populate with copied-on-write entry */
		oldentry = zend_hash_str_find_ptr(&phar_obj->archive->manifest, oldfile, (uint) oldfile_len);
	}

	memcpy((void *) &newentry, oldentry, sizeof(phar_entry_info));

	if (Z_TYPE(newentry.metadata) != IS_UNDEF) {
		zval_copy_ctor(&newentry.metadata);
		newentry.metadata_str.s = NULL;
	}

	newentry.filename = estrndup(newfile, newfile_len);
	newentry.filename_len = newfile_len;
	newentry.fp_refcount = 0;

	if (oldentry->fp_type != PHAR_FP) {
		if (FAILURE == phar_copy_entry_fp(oldentry, &newentry, &error)) {
			efree(newentry.filename);
			php_stream_close(newentry.fp);
			zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
			efree(error);
			return;
		}
	}

	zend_hash_str_add_mem(&oldentry->phar->manifest, newfile, newfile_len, &newentry, sizeof(phar_entry_info));
	phar_obj->archive->is_modified = 1;
	phar_flush(phar_obj->archive, 0, 0, 0, &error);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
		efree(error);
	}

	RETURN_TRUE;
}
