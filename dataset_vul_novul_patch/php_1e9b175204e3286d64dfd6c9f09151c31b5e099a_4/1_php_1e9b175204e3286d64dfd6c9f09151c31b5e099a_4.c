PHP_METHOD(Phar, unlinkArchive)
{
	char *fname, *error, *zname, *arch, *entry;
	size_t fname_len;
        int zname_len, arch_len, entry_len;
        phar_archive_data *phar;
 
       if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &fname, &fname_len) == FAILURE) {
                RETURN_FALSE;
        }
 
	if (!fname_len) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "Unknown phar archive \"\"");
		return;
	}

	if (FAILURE == phar_open_from_filename(fname, fname_len, NULL, 0, REPORT_ERRORS, &phar, &error)) {
		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0, "Unknown phar archive \"%s\": %s", fname, error);
			efree(error);
		} else {
			zend_throw_exception_ex(phar_ce_PharException, 0, "Unknown phar archive \"%s\"", fname);
		}
		return;
	}

	zname = (char*)zend_get_executed_filename();
	zname_len = strlen(zname);

	if (zname_len > 7 && !memcmp(zname, "phar://", 7) && SUCCESS == phar_split_fname(zname, zname_len, &arch, &arch_len, &entry, &entry_len, 2, 0)) {
		if (arch_len == fname_len && !memcmp(arch, fname, arch_len)) {
			zend_throw_exception_ex(phar_ce_PharException, 0, "phar archive \"%s\" cannot be unlinked from within itself", fname);
			efree(arch);
			efree(entry);
			return;
		}
		efree(arch);
		efree(entry);
	}

	if (phar->is_persistent) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "phar archive \"%s\" is in phar.cache_list, cannot unlinkArchive()", fname);
		return;
	}

	if (phar->refcount) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "phar archive \"%s\" has open file handles or objects.  fclose() all file handles, and unset() all objects prior to calling unlinkArchive()", fname);
		return;
	}

	fname = estrndup(phar->fname, phar->fname_len);

	/* invalidate phar cache */
	PHAR_G(last_phar) = NULL;
	PHAR_G(last_phar_name) = PHAR_G(last_alias) = NULL;

	phar_archive_delref(phar);
	unlink(fname);
	efree(fname);
	RETURN_TRUE;
}
