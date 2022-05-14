int phar_get_entry_data(phar_entry_data **ret, char *fname, int fname_len, char *path, int path_len, char *mode, char allow_dir, char **error, int security TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar;
	phar_entry_info *entry;
	int for_write  = mode[0] != 'r' || mode[1] == '+';
	int for_append = mode[0] == 'a';
	int for_create = mode[0] != 'r';
	int for_trunc  = mode[0] == 'w';

	if (!ret) {
		return FAILURE;
	}

	*ret = NULL;

	if (error) {
		*error = NULL;
	}

	if (FAILURE == phar_get_archive(&phar, fname, fname_len, NULL, 0, error TSRMLS_CC)) {
		return FAILURE;
	}

	if (for_write && PHAR_G(readonly) && !phar->is_data) {
		if (error) {
			spprintf(error, 4096, "phar error: file \"%s\" in phar \"%s\" cannot be opened for writing, disabled by ini setting", path, fname);
		}
		return FAILURE;
	}

	if (!path_len) {
		if (error) {
			spprintf(error, 4096, "phar error: file \"\" in phar \"%s\" cannot be empty", fname);
		}
		return FAILURE;
	}
really_get_entry:
	if (allow_dir) {
		if ((entry = phar_get_entry_info_dir(phar, path, path_len, allow_dir, for_create && !PHAR_G(readonly) && !phar->is_data ? NULL : error, security TSRMLS_CC)) == NULL) {
			if (for_create && (!PHAR_G(readonly) || phar->is_data)) {
				return SUCCESS;
			}
			return FAILURE;
		}
	} else {
		if ((entry = phar_get_entry_info(phar, path, path_len, for_create && !PHAR_G(readonly) && !phar->is_data ? NULL : error, security TSRMLS_CC)) == NULL) {
			if (for_create && (!PHAR_G(readonly) || phar->is_data)) {
				return SUCCESS;
			}
			return FAILURE;
		}
	}

	if (for_write && phar->is_persistent) {
		if (FAILURE == phar_copy_on_write(&phar TSRMLS_CC)) {
			if (error) {
				spprintf(error, 4096, "phar error: file \"%s\" in phar \"%s\" cannot be opened for writing, could not make cached phar writeable", path, fname);
			}
			return FAILURE;
		} else {
			goto really_get_entry;
		}
	}

	if (entry->is_modified && !for_write) {
		if (error) {
			spprintf(error, 4096, "phar error: file \"%s\" in phar \"%s\" cannot be opened for reading, writable file pointers are open", path, fname);
		}
		return FAILURE;
	}

	if (entry->fp_refcount && for_write) {
		if (error) {
			spprintf(error, 4096, "phar error: file \"%s\" in phar \"%s\" cannot be opened for writing, readable file pointers are open", path, fname);
		}
		return FAILURE;
	}

	if (entry->is_deleted) {
		if (!for_create) {
			return FAILURE;
		}
		entry->is_deleted = 0;
	}

	if (entry->is_dir) {
		*ret = (phar_entry_data *) emalloc(sizeof(phar_entry_data));
		(*ret)->position = 0;
		(*ret)->fp = NULL;
		(*ret)->phar = phar;
		(*ret)->for_write = for_write;
		(*ret)->internal_file = entry;
		(*ret)->is_zip = entry->is_zip;
		(*ret)->is_tar = entry->is_tar;

		if (!phar->is_persistent) {
			++(entry->phar->refcount);
			++(entry->fp_refcount);
		}

		return SUCCESS;
	}

	if (entry->fp_type == PHAR_MOD) {
		if (for_trunc) {
			if (FAILURE == phar_create_writeable_entry(phar, entry, error TSRMLS_CC)) {
				return FAILURE;
			}
		} else if (for_append) {
			phar_seek_efp(entry, 0, SEEK_END, 0, 0 TSRMLS_CC);
		}
	} else {
		if (for_write) {
			if (entry->link) {
				efree(entry->link);
				entry->link = NULL;
				entry->tar_type = (entry->is_tar ? TAR_FILE : '\0');
			}

			if (for_trunc) {
				if (FAILURE == phar_create_writeable_entry(phar, entry, error TSRMLS_CC)) {
					return FAILURE;
				}
			} else {
				if (FAILURE == phar_separate_entry_fp(entry, error TSRMLS_CC)) {
					return FAILURE;
				}
			}
		} else {
			if (FAILURE == phar_open_entry_fp(entry, error, 1 TSRMLS_CC)) {
				return FAILURE;
			}
		}
	}

	*ret = (phar_entry_data *) emalloc(sizeof(phar_entry_data));
	(*ret)->position = 0;
	(*ret)->phar = phar;
	(*ret)->for_write = for_write;
	(*ret)->internal_file = entry;
	(*ret)->is_zip = entry->is_zip;
        (*ret)->is_tar = entry->is_tar;
        (*ret)->fp = phar_get_efp(entry, 1 TSRMLS_CC);
        if (entry->link) {
               phar_entry_info *link = phar_get_link_source(entry TSRMLS_CC);
               if(!link) {
                       return FAILURE;
               }
               (*ret)->zero = phar_get_fp_offset(link TSRMLS_CC);
        } else {
                (*ret)->zero = phar_get_fp_offset(entry TSRMLS_CC);
        }
	}

	return SUCCESS;
}
/* }}} */

/**
 * Create a new dummy file slot within a writeable phar for a newly created file
 */
phar_entry_data *phar_get_or_create_entry_data(char *fname, int fname_len, char *path, int path_len, char *mode, char allow_dir, char **error, int security TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar;
	phar_entry_info *entry, etemp;
	phar_entry_data *ret;
	const char *pcr_error;
	char is_dir;

#ifdef PHP_WIN32
	phar_unixify_path_separators(path, path_len);
#endif

	is_dir = (path_len && path[path_len - 1] == '/') ? 1 : 0;

	if (FAILURE == phar_get_archive(&phar, fname, fname_len, NULL, 0, error TSRMLS_CC)) {
		return NULL;
	}

	if (FAILURE == phar_get_entry_data(&ret, fname, fname_len, path, path_len, mode, allow_dir, error, security TSRMLS_CC)) {
		return NULL;
	} else if (ret) {
		return ret;
	}

	if (phar_path_check(&path, &path_len, &pcr_error) > pcr_is_ok) {
		if (error) {
			spprintf(error, 0, "phar error: invalid path \"%s\" contains %s", path, pcr_error);
		}
		return NULL;
	}

	if (phar->is_persistent && FAILURE == phar_copy_on_write(&phar TSRMLS_CC)) {
		if (error) {
			spprintf(error, 4096, "phar error: file \"%s\" in phar \"%s\" cannot be created, could not make cached phar writeable", path, fname);
		}
		return NULL;
	}

	/* create a new phar data holder */
	ret = (phar_entry_data *) emalloc(sizeof(phar_entry_data));

	/* create an entry, this is a new file */
	memset(&etemp, 0, sizeof(phar_entry_info));
	etemp.filename_len = path_len;
	etemp.fp_type = PHAR_MOD;
	etemp.fp = php_stream_fopen_tmpfile();

	if (!etemp.fp) {
		if (error) {
			spprintf(error, 0, "phar error: unable to create temporary file");
		}
		efree(ret);
		return NULL;
	}

	etemp.fp_refcount = 1;

	if (allow_dir == 2) {
		etemp.is_dir = 1;
		etemp.flags = etemp.old_flags = PHAR_ENT_PERM_DEF_DIR;
	} else {
		etemp.flags = etemp.old_flags = PHAR_ENT_PERM_DEF_FILE;
	}
	if (is_dir) {
		etemp.filename_len--; /* strip trailing / */
		path_len--;
	}

	phar_add_virtual_dirs(phar, path, path_len TSRMLS_CC);
	etemp.is_modified = 1;
	etemp.timestamp = time(0);
	etemp.is_crc_checked = 1;
	etemp.phar = phar;
	etemp.filename = estrndup(path, path_len);
	etemp.is_zip = phar->is_zip;

	if (phar->is_tar) {
		etemp.is_tar = phar->is_tar;
		etemp.tar_type = etemp.is_dir ? TAR_DIR : TAR_FILE;
	}

	if (FAILURE == zend_hash_add(&phar->manifest, etemp.filename, path_len, (void*)&etemp, sizeof(phar_entry_info), (void **) &entry)) {
		php_stream_close(etemp.fp);
		if (error) {
			spprintf(error, 0, "phar error: unable to add new entry \"%s\" to phar \"%s\"", etemp.filename, phar->fname);
		}
		efree(ret);
		efree(etemp.filename);
		return NULL;
	}

	if (!entry) {
		php_stream_close(etemp.fp);
		efree(etemp.filename);
		efree(ret);
		return NULL;
	}

	++(phar->refcount);
	ret->phar = phar;
	ret->fp = entry->fp;
	ret->position = ret->zero = 0;
	ret->for_write = 1;
	ret->is_zip = entry->is_zip;
	ret->is_tar = entry->is_tar;
	ret->internal_file = entry;

	return ret;
}
/* }}} */

/* initialize a phar_archive_data's read-only fp for existing phar data */
int phar_open_archive_fp(phar_archive_data *phar TSRMLS_DC) /* {{{ */
{
	if (phar_get_pharfp(phar TSRMLS_CC)) {
		return SUCCESS;
	}

	if (php_check_open_basedir(phar->fname TSRMLS_CC)) {
		return FAILURE;
	}

	phar_set_pharfp(phar, php_stream_open_wrapper(phar->fname, "rb", IGNORE_URL|STREAM_MUST_SEEK|0, NULL) TSRMLS_CC);

	if (!phar_get_pharfp(phar TSRMLS_CC)) {
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* copy file data from an existing to a new phar_entry_info that is not in the manifest */
int phar_copy_entry_fp(phar_entry_info *source, phar_entry_info *dest, char **error TSRMLS_DC) /* {{{ */
{
	phar_entry_info *link;

	if (FAILURE == phar_open_entry_fp(source, error, 1 TSRMLS_CC)) {
		return FAILURE;
	}

	if (dest->link) {
		efree(dest->link);
		dest->link = NULL;
		dest->tar_type = (dest->is_tar ? TAR_FILE : '\0');
	}

	dest->fp_type = PHAR_MOD;
	dest->offset = 0;
	dest->is_modified = 1;
	dest->fp = php_stream_fopen_tmpfile();
	if (dest->fp == NULL) {
		spprintf(error, 0, "phar error: unable to create temporary file");
		return EOF;
	}
	phar_seek_efp(source, 0, SEEK_SET, 0, 1 TSRMLS_CC);
	link = phar_get_link_source(source TSRMLS_CC);

	if (!link) {
		link = source;
	}

	if (SUCCESS != phar_stream_copy_to_stream(phar_get_efp(link, 0 TSRMLS_CC), dest->fp, link->uncompressed_filesize, NULL)) {
		php_stream_close(dest->fp);
		dest->fp_type = PHAR_FP;
		if (error) {
			spprintf(error, 4096, "phar error: unable to copy contents of file \"%s\" to \"%s\" in phar archive \"%s\"", source->filename, dest->filename, source->phar->fname);
		}
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* open and decompress a compressed phar entry
 */
int phar_open_entry_fp(phar_entry_info *entry, char **error, int follow_links TSRMLS_DC) /* {{{ */
{
	php_stream_filter *filter;
	phar_archive_data *phar = entry->phar;
	char *filtername;
	off_t loc;
	php_stream *ufp;
	phar_entry_data dummy;

	if (follow_links && entry->link) {
		phar_entry_info *link_entry = phar_get_link_source(entry TSRMLS_CC);
		if (link_entry && link_entry != entry) {
			return phar_open_entry_fp(link_entry, error, 1 TSRMLS_CC);
		}
	}

	if (entry->is_modified) {
		return SUCCESS;
	}

	if (entry->fp_type == PHAR_TMP) {
		if (!entry->fp) {
			entry->fp = php_stream_open_wrapper(entry->tmp, "rb", STREAM_MUST_SEEK|0, NULL);
		}
		return SUCCESS;
	}

	if (entry->fp_type != PHAR_FP) {
		/* either newly created or already modified */
		return SUCCESS;
	}

	if (!phar_get_pharfp(phar TSRMLS_CC)) {
		if (FAILURE == phar_open_archive_fp(phar TSRMLS_CC)) {
			spprintf(error, 4096, "phar error: Cannot open phar archive \"%s\" for reading", phar->fname);
			return FAILURE;
		}
	}

	if ((entry->old_flags && !(entry->old_flags & PHAR_ENT_COMPRESSION_MASK)) || !(entry->flags & PHAR_ENT_COMPRESSION_MASK)) {
		dummy.internal_file = entry;
		dummy.phar = phar;
		dummy.zero = entry->offset;
		dummy.fp = phar_get_pharfp(phar TSRMLS_CC);
		if (FAILURE == phar_postprocess_file(&dummy, entry->crc32, error, 1 TSRMLS_CC)) {
			return FAILURE;
		}
		return SUCCESS;
	}

	if (!phar_get_entrypufp(entry TSRMLS_CC)) {
		phar_set_entrypufp(entry, php_stream_fopen_tmpfile() TSRMLS_CC);
		if (!phar_get_entrypufp(entry TSRMLS_CC)) {
			spprintf(error, 4096, "phar error: Cannot open temporary file for decompressing phar archive \"%s\" file \"%s\"", phar->fname, entry->filename);
			return FAILURE;
		}
	}

	dummy.internal_file = entry;
	dummy.phar = phar;
	dummy.zero = entry->offset;
	dummy.fp = phar_get_pharfp(phar TSRMLS_CC);
	if (FAILURE == phar_postprocess_file(&dummy, entry->crc32, error, 1 TSRMLS_CC)) {
		return FAILURE;
	}

	ufp = phar_get_entrypufp(entry TSRMLS_CC);

	if ((filtername = phar_decompress_filter(entry, 0)) != NULL) {
		filter = php_stream_filter_create(filtername, NULL, 0 TSRMLS_CC);
	} else {
		filter = NULL;
	}

	if (!filter) {
		spprintf(error, 4096, "phar error: unable to read phar \"%s\" (cannot create %s filter while decompressing file \"%s\")", phar->fname, phar_decompress_filter(entry, 1), entry->filename);
		return FAILURE;
	}

	/* now we can safely use proper decompression */
	/* save the new offset location within ufp */
	php_stream_seek(ufp, 0, SEEK_END);
	loc = php_stream_tell(ufp);
	php_stream_filter_append(&ufp->writefilters, filter);
	php_stream_seek(phar_get_entrypfp(entry TSRMLS_CC), phar_get_fp_offset(entry TSRMLS_CC), SEEK_SET);

	if (entry->uncompressed_filesize) {
		if (SUCCESS != phar_stream_copy_to_stream(phar_get_entrypfp(entry TSRMLS_CC), ufp, entry->compressed_filesize, NULL)) {
			spprintf(error, 4096, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", phar->fname, entry->filename);
			php_stream_filter_remove(filter, 1 TSRMLS_CC);
			return FAILURE;
		}
	}

	php_stream_filter_flush(filter, 1);
	php_stream_flush(ufp);
	php_stream_filter_remove(filter, 1 TSRMLS_CC);

	if (php_stream_tell(ufp) - loc != (off_t) entry->uncompressed_filesize) {
		spprintf(error, 4096, "phar error: internal corruption of phar \"%s\" (actual filesize mismatch on file \"%s\")", phar->fname, entry->filename);
		return FAILURE;
	}

	entry->old_flags = entry->flags;

	/* this is now the new location of the file contents within this fp */
	phar_set_fp_type(entry, PHAR_UFP, loc TSRMLS_CC);
	dummy.zero = entry->offset;
	dummy.fp = ufp;
	if (FAILURE == phar_postprocess_file(&dummy, entry->crc32, error, 0 TSRMLS_CC)) {
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

int phar_create_writeable_entry(phar_archive_data *phar, phar_entry_info *entry, char **error TSRMLS_DC) /* {{{ */
{
	if (entry->fp_type == PHAR_MOD) {
		/* already newly created, truncate */
		php_stream_truncate_set_size(entry->fp, 0);

		entry->old_flags = entry->flags;
		entry->is_modified = 1;
		phar->is_modified = 1;
		/* reset file size */
		entry->uncompressed_filesize = 0;
		entry->compressed_filesize = 0;
		entry->crc32 = 0;
		entry->flags = PHAR_ENT_PERM_DEF_FILE;
		entry->fp_type = PHAR_MOD;
		entry->offset = 0;
		return SUCCESS;
	}

	if (error) {
		*error = NULL;
	}

	/* open a new temp file for writing */
	if (entry->link) {
		efree(entry->link);
		entry->link = NULL;
		entry->tar_type = (entry->is_tar ? TAR_FILE : '\0');
	}

	entry->fp = php_stream_fopen_tmpfile();

	if (!entry->fp) {
		if (error) {
			spprintf(error, 0, "phar error: unable to create temporary file");
		}
		return FAILURE;
	}

	entry->old_flags = entry->flags;
	entry->is_modified = 1;
	phar->is_modified = 1;
	/* reset file size */
	entry->uncompressed_filesize = 0;
	entry->compressed_filesize = 0;
	entry->crc32 = 0;
	entry->flags = PHAR_ENT_PERM_DEF_FILE;
	entry->fp_type = PHAR_MOD;
	entry->offset = 0;
	return SUCCESS;
}
/* }}} */

int phar_separate_entry_fp(phar_entry_info *entry, char **error TSRMLS_DC) /* {{{ */
{
	php_stream *fp;
	phar_entry_info *link;

	if (FAILURE == phar_open_entry_fp(entry, error, 1 TSRMLS_CC)) {
		return FAILURE;
	}

	if (entry->fp_type == PHAR_MOD) {
		return SUCCESS;
	}

	fp = php_stream_fopen_tmpfile();
	if (fp == NULL) {
		spprintf(error, 0, "phar error: unable to create temporary file");
		return FAILURE;
	}
	phar_seek_efp(entry, 0, SEEK_SET, 0, 1 TSRMLS_CC);
	link = phar_get_link_source(entry TSRMLS_CC);

	if (!link) {
		link = entry;
	}

	if (SUCCESS != phar_stream_copy_to_stream(phar_get_efp(link, 0 TSRMLS_CC), fp, link->uncompressed_filesize, NULL)) {
		if (error) {
			spprintf(error, 4096, "phar error: cannot separate entry file \"%s\" contents in phar archive \"%s\" for write access", entry->filename, entry->phar->fname);
		}
		return FAILURE;
	}

	if (entry->link) {
		efree(entry->link);
		entry->link = NULL;
		entry->tar_type = (entry->is_tar ? TAR_FILE : '\0');
	}

	entry->offset = 0;
	entry->fp = fp;
	entry->fp_type = PHAR_MOD;
	entry->is_modified = 1;
	return SUCCESS;
}
/* }}} */

/**
 * helper function to open an internal file's fp just-in-time
 */
phar_entry_info * phar_open_jit(phar_archive_data *phar, phar_entry_info *entry, char **error TSRMLS_DC) /* {{{ */
{
	if (error) {
		*error = NULL;
	}
	/* seek to start of internal file and read it */
	if (FAILURE == phar_open_entry_fp(entry, error, 1 TSRMLS_CC)) {
		return NULL;
	}
	if (-1 == phar_seek_efp(entry, 0, SEEK_SET, 0, 1 TSRMLS_CC)) {
		spprintf(error, 4096, "phar error: cannot seek to start of file \"%s\" in phar \"%s\"", entry->filename, phar->fname);
		return NULL;
	}
	return entry;
}
/* }}} */

PHP_PHAR_API int phar_resolve_alias(char *alias, int alias_len, char **filename, int *filename_len TSRMLS_DC) /* {{{ */ {
	phar_archive_data **fd_ptr;
	if (PHAR_GLOBALS->phar_alias_map.arBuckets
			&& SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void**)&fd_ptr)) {
		*filename = (*fd_ptr)->fname;
		*filename_len = (*fd_ptr)->fname_len;
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

int phar_free_alias(phar_archive_data *phar, char *alias, int alias_len TSRMLS_DC) /* {{{ */
{
	if (phar->refcount || phar->is_persistent) {
		return FAILURE;
	}

	/* this archive has no open references, so emit an E_STRICT and remove it */
	if (zend_hash_del(&(PHAR_GLOBALS->phar_fname_map), phar->fname, phar->fname_len) != SUCCESS) {
		return FAILURE;
	}

	/* invalidate phar cache */
	PHAR_G(last_phar) = NULL;
	PHAR_G(last_phar_name) = PHAR_G(last_alias) = NULL;

	return SUCCESS;
}
/* }}} */

/**
 * Looks up a phar archive in the filename map, connecting it to the alias
 * (if any) or returns null
 */
int phar_get_archive(phar_archive_data **archive, char *fname, int fname_len, char *alias, int alias_len, char **error TSRMLS_DC) /* {{{ */
{
	phar_archive_data *fd, **fd_ptr;
	char *my_realpath, *save;
	int save_len;
	ulong fhash, ahash = 0;

	phar_request_initialize(TSRMLS_C);

	if (error) {
		*error = NULL;
	}

	*archive = NULL;

	if (PHAR_G(last_phar) && fname_len == PHAR_G(last_phar_name_len) && !memcmp(fname, PHAR_G(last_phar_name), fname_len)) {
		*archive = PHAR_G(last_phar);
		if (alias && alias_len) {

			if (!PHAR_G(last_phar)->is_temporary_alias && (alias_len != PHAR_G(last_phar)->alias_len || memcmp(PHAR_G(last_phar)->alias, alias, alias_len))) {
				if (error) {
					spprintf(error, 0, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, PHAR_G(last_phar)->fname, fname);
				}
				*archive = NULL;
				return FAILURE;
			}

			if (PHAR_G(last_phar)->alias_len && SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_alias_map), PHAR_G(last_phar)->alias, PHAR_G(last_phar)->alias_len, (void**)&fd_ptr)) {
				zend_hash_del(&(PHAR_GLOBALS->phar_alias_map), PHAR_G(last_phar)->alias, PHAR_G(last_phar)->alias_len);
			}

			zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void*)&(*archive), sizeof(phar_archive_data*), NULL);
			PHAR_G(last_alias) = alias;
			PHAR_G(last_alias_len) = alias_len;
		}

		return SUCCESS;
	}

	if (alias && alias_len && PHAR_G(last_phar) && alias_len == PHAR_G(last_alias_len) && !memcmp(alias, PHAR_G(last_alias), alias_len)) {
		fd = PHAR_G(last_phar);
		fd_ptr = &fd;
		goto alias_success;
	}

	if (alias && alias_len) {
		ahash = zend_inline_hash_func(alias, alias_len);
		if (SUCCESS == zend_hash_quick_find(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, ahash, (void**)&fd_ptr)) {
alias_success:
			if (fname && (fname_len != (*fd_ptr)->fname_len || strncmp(fname, (*fd_ptr)->fname, fname_len))) {
				if (error) {
					spprintf(error, 0, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, (*fd_ptr)->fname, fname);
				}
				if (SUCCESS == phar_free_alias(*fd_ptr, alias, alias_len TSRMLS_CC)) {
					if (error) {
						efree(*error);
						*error = NULL;
					}
				}
				return FAILURE;
			}

			*archive = *fd_ptr;
			fd = *fd_ptr;
			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = alias;
			PHAR_G(last_alias_len) = alias_len;

			return SUCCESS;
		}

		if (PHAR_G(manifest_cached) && SUCCESS == zend_hash_quick_find(&cached_alias, alias, alias_len, ahash, (void **)&fd_ptr)) {
			goto alias_success;
		}
	}

	fhash = zend_inline_hash_func(fname, fname_len);
	my_realpath = NULL;
	save = fname;
	save_len = fname_len;

	if (fname && fname_len) {
		if (SUCCESS == zend_hash_quick_find(&(PHAR_GLOBALS->phar_fname_map), fname, fname_len, fhash, (void**)&fd_ptr)) {
			*archive = *fd_ptr;
			fd = *fd_ptr;

			if (alias && alias_len) {
				if (!fd->is_temporary_alias && (alias_len != fd->alias_len || memcmp(fd->alias, alias, alias_len))) {
					if (error) {
						spprintf(error, 0, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, (*fd_ptr)->fname, fname);
					}
					return FAILURE;
				}

				if (fd->alias_len && SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_alias_map), fd->alias, fd->alias_len, (void**)&fd_ptr)) {
					zend_hash_del(&(PHAR_GLOBALS->phar_alias_map), fd->alias, fd->alias_len);
				}

				zend_hash_quick_add(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, ahash, (void*)&fd, sizeof(phar_archive_data*), NULL);
			}

			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = fd->alias;
			PHAR_G(last_alias_len) = fd->alias_len;

			return SUCCESS;
		}

		if (PHAR_G(manifest_cached) && SUCCESS == zend_hash_quick_find(&cached_phars, fname, fname_len, fhash, (void**)&fd_ptr)) {
			*archive = *fd_ptr;
			fd = *fd_ptr;

			/* this could be problematic - alias should never be different from manifest alias
			   for cached phars */
			if (!fd->is_temporary_alias && alias && alias_len) {
				if (alias_len != fd->alias_len || memcmp(fd->alias, alias, alias_len)) {
					if (error) {
						spprintf(error, 0, "alias \"%s\" is already used for archive \"%s\" cannot be overloaded with \"%s\"", alias, (*fd_ptr)->fname, fname);
					}
					return FAILURE;
				}
			}

			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = fd->alias;
			PHAR_G(last_alias_len) = fd->alias_len;

			return SUCCESS;
		}

		if (SUCCESS == zend_hash_quick_find(&(PHAR_GLOBALS->phar_alias_map), save, save_len, fhash, (void**)&fd_ptr)) {
			fd = *archive = *fd_ptr;

			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = fd->alias;
			PHAR_G(last_alias_len) = fd->alias_len;

			return SUCCESS;
		}

		if (PHAR_G(manifest_cached) && SUCCESS == zend_hash_quick_find(&cached_alias, save, save_len, fhash, (void**)&fd_ptr)) {
			fd = *archive = *fd_ptr;

			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = fd->alias;
			PHAR_G(last_alias_len) = fd->alias_len;

			return SUCCESS;
		}

		/* not found, try converting \ to / */
		my_realpath = expand_filepath(fname, my_realpath TSRMLS_CC);

		if (my_realpath) {
			fname_len = strlen(my_realpath);
			fname = my_realpath;
		} else {
			return FAILURE;
		}
#ifdef PHP_WIN32
		phar_unixify_path_separators(fname, fname_len);
#endif
		fhash = zend_inline_hash_func(fname, fname_len);

		if (SUCCESS == zend_hash_quick_find(&(PHAR_GLOBALS->phar_fname_map), fname, fname_len, fhash, (void**)&fd_ptr)) {
realpath_success:
			*archive = *fd_ptr;
			fd = *fd_ptr;

			if (alias && alias_len) {
				zend_hash_quick_add(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, ahash, (void*)&fd, sizeof(phar_archive_data*), NULL);
			}

			efree(my_realpath);

			PHAR_G(last_phar) = fd;
			PHAR_G(last_phar_name) = fd->fname;
			PHAR_G(last_phar_name_len) = fd->fname_len;
			PHAR_G(last_alias) = fd->alias;
			PHAR_G(last_alias_len) = fd->alias_len;

			return SUCCESS;
		}

		if (PHAR_G(manifest_cached) && SUCCESS == zend_hash_quick_find(&cached_phars, fname, fname_len, fhash, (void**)&fd_ptr)) {
			goto realpath_success;
		}

		efree(my_realpath);
	}

	return FAILURE;
}
/* }}} */

/**
 * Determine which stream compression filter (if any) we need to read this file
 */
char * phar_compress_filter(phar_entry_info * entry, int return_unknown) /* {{{ */
{
	switch (entry->flags & PHAR_ENT_COMPRESSION_MASK) {
	case PHAR_ENT_COMPRESSED_GZ:
		return "zlib.deflate";
	case PHAR_ENT_COMPRESSED_BZ2:
		return "bzip2.compress";
	default:
		return return_unknown ? "unknown" : NULL;
	}
}
/* }}} */

/**
 * Determine which stream decompression filter (if any) we need to read this file
 */
char * phar_decompress_filter(phar_entry_info * entry, int return_unknown) /* {{{ */
{
	php_uint32 flags;

	if (entry->is_modified) {
		flags = entry->old_flags;
	} else {
		flags = entry->flags;
	}

	switch (flags & PHAR_ENT_COMPRESSION_MASK) {
		case PHAR_ENT_COMPRESSED_GZ:
			return "zlib.inflate";
		case PHAR_ENT_COMPRESSED_BZ2:
			return "bzip2.decompress";
		default:
			return return_unknown ? "unknown" : NULL;
	}
}
/* }}} */

/**
 * retrieve information on a file contained within a phar, or null if it ain't there
 */
phar_entry_info *phar_get_entry_info(phar_archive_data *phar, char *path, int path_len, char **error, int security TSRMLS_DC) /* {{{ */
{
	return phar_get_entry_info_dir(phar, path, path_len, 0, error, security TSRMLS_CC);
}
/* }}} */
/**
 * retrieve information on a file or directory contained within a phar, or null if none found
 * allow_dir is 0 for none, 1 for both empty directories in the phar and temp directories, and 2 for only
 * valid pre-existing empty directory entries
 */
phar_entry_info *phar_get_entry_info_dir(phar_archive_data *phar, char *path, int path_len, char dir, char **error, int security TSRMLS_DC) /* {{{ */
{
	const char *pcr_error;
	phar_entry_info *entry;
	int is_dir;

#ifdef PHP_WIN32
	phar_unixify_path_separators(path, path_len);
#endif

	is_dir = (path_len && (path[path_len - 1] == '/')) ? 1 : 0;

	if (error) {
		*error = NULL;
	}

	if (security && path_len >= sizeof(".phar")-1 && !memcmp(path, ".phar", sizeof(".phar")-1)) {
		if (error) {
			spprintf(error, 4096, "phar error: cannot directly access magic \".phar\" directory or files within it");
		}
		return NULL;
	}

	if (!path_len && !dir) {
		if (error) {
			spprintf(error, 4096, "phar error: invalid path \"%s\" must not be empty", path);
		}
		return NULL;
	}

	if (phar_path_check(&path, &path_len, &pcr_error) > pcr_is_ok) {
		if (error) {
			spprintf(error, 4096, "phar error: invalid path \"%s\" contains %s", path, pcr_error);
		}
		return NULL;
	}

	if (!phar->manifest.arBuckets) {
		return NULL;
	}

	if (is_dir) {
		if (!path_len || path_len == 1) {
			return NULL;
		}
		path_len--;
	}

	if (SUCCESS == zend_hash_find(&phar->manifest, path, path_len, (void**)&entry)) {
		if (entry->is_deleted) {
			/* entry is deleted, but has not been flushed to disk yet */
			return NULL;
		}
		if (entry->is_dir && !dir) {
			if (error) {
				spprintf(error, 4096, "phar error: path \"%s\" is a directory", path);
			}
			return NULL;
		}
		if (!entry->is_dir && dir == 2) {
			/* user requested a directory, we must return one */
			if (error) {
				spprintf(error, 4096, "phar error: path \"%s\" exists and is a not a directory", path);
			}
			return NULL;
		}
		return entry;
	}

	if (dir) {
		if (zend_hash_exists(&phar->virtual_dirs, path, path_len)) {
			/* a file or directory exists in a sub-directory of this path */
			entry = (phar_entry_info *) ecalloc(1, sizeof(phar_entry_info));
			/* this next line tells PharFileInfo->__destruct() to efree the filename */
			entry->is_temp_dir = entry->is_dir = 1;
			entry->filename = (char *) estrndup(path, path_len + 1);
			entry->filename_len = path_len;
			entry->phar = phar;
			return entry;
		}
	}

	if (phar->mounted_dirs.arBuckets && zend_hash_num_elements(&phar->mounted_dirs)) {
		phar_zstr key;
		char *str_key;
		ulong unused;
		uint keylen;

		zend_hash_internal_pointer_reset(&phar->mounted_dirs);
		while (FAILURE != zend_hash_has_more_elements(&phar->mounted_dirs)) {
			if (HASH_KEY_NON_EXISTENT == zend_hash_get_current_key_ex(&phar->mounted_dirs, &key, &keylen, &unused, 0, NULL)) {
				break;
			}

			PHAR_STR(key, str_key);

			if ((int)keylen >= path_len || strncmp(str_key, path, keylen)) {
				PHAR_STR_FREE(str_key);
				continue;
			} else {
				char *test;
				int test_len;
				php_stream_statbuf ssb;

				if (SUCCESS != zend_hash_find(&phar->manifest, str_key, keylen, (void **) &entry)) {
					if (error) {
						spprintf(error, 4096, "phar internal error: mounted path \"%s\" could not be retrieved from manifest", str_key);
					}
					PHAR_STR_FREE(str_key);
					return NULL;
				}

				if (!entry->tmp || !entry->is_mounted) {
					if (error) {
						spprintf(error, 4096, "phar internal error: mounted path \"%s\" is not properly initialized as a mounted path", str_key);
					}
					PHAR_STR_FREE(str_key);
					return NULL;
				}
				PHAR_STR_FREE(str_key);

				test_len = spprintf(&test, MAXPATHLEN, "%s%s", entry->tmp, path + keylen);

				if (SUCCESS != php_stream_stat_path(test, &ssb)) {
					efree(test);
					return NULL;
				}

				if (ssb.sb.st_mode & S_IFDIR && !dir) {
					efree(test);
					if (error) {
						spprintf(error, 4096, "phar error: path \"%s\" is a directory", path);
					}
					return NULL;
				}

				if ((ssb.sb.st_mode & S_IFDIR) == 0 && dir) {
					efree(test);
					/* user requested a directory, we must return one */
					if (error) {
						spprintf(error, 4096, "phar error: path \"%s\" exists and is a not a directory", path);
					}
					return NULL;
				}

				/* mount the file just in time */
				if (SUCCESS != phar_mount_entry(phar, test, test_len, path, path_len TSRMLS_CC)) {
					efree(test);
					if (error) {
						spprintf(error, 4096, "phar error: path \"%s\" exists as file \"%s\" and could not be mounted", path, test);
					}
					return NULL;
				}

				efree(test);

				if (SUCCESS != zend_hash_find(&phar->manifest, path, path_len, (void**)&entry)) {
					if (error) {
						spprintf(error, 4096, "phar error: path \"%s\" exists as file \"%s\" and could not be retrieved after being mounted", path, test);
					}
					return NULL;
				}
				return entry;
			}
		}
	}

	return NULL;
}
/* }}} */

static const char hexChars[] = "0123456789ABCDEF";

static int phar_hex_str(const char *digest, size_t digest_len, char **signature TSRMLS_DC) /* {{{ */
{
	int pos = -1;
	size_t len = 0;

	*signature = (char*)safe_pemalloc(digest_len, 2, 1, PHAR_G(persist));

	for (; len < digest_len; ++len) {
		(*signature)[++pos] = hexChars[((const unsigned char *)digest)[len] >> 4];
		(*signature)[++pos] = hexChars[((const unsigned char *)digest)[len] & 0x0F];
	}
	(*signature)[++pos] = '\0';
	return pos;
}
/* }}} */

#ifndef PHAR_HAVE_OPENSSL
static int phar_call_openssl_signverify(int is_sign, php_stream *fp, off_t end, char *key, int key_len, char **signature, int *signature_len TSRMLS_DC) /* {{{ */
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zval *zdata, *zsig, *zkey, *retval_ptr, **zp[3], *openssl;

	MAKE_STD_ZVAL(zdata);
	MAKE_STD_ZVAL(openssl);
	ZVAL_STRINGL(openssl, is_sign ? "openssl_sign" : "openssl_verify", is_sign ? sizeof("openssl_sign")-1 : sizeof("openssl_verify")-1, 1);
	MAKE_STD_ZVAL(zsig);
	ZVAL_STRINGL(zsig, *signature, *signature_len, 1);
	MAKE_STD_ZVAL(zkey);
	ZVAL_STRINGL(zkey, key, key_len, 1);
	zp[0] = &zdata;
	zp[1] = &zsig;
	zp[2] = &zkey;

	php_stream_rewind(fp);
	Z_TYPE_P(zdata) = IS_STRING;
	Z_STRLEN_P(zdata) = end;

	if (end != (off_t) php_stream_copy_to_mem(fp, &(Z_STRVAL_P(zdata)), (size_t) end, 0)) {
		zval_dtor(zdata);
		zval_dtor(zsig);
		zval_dtor(zkey);
		zval_dtor(openssl);
		efree(openssl);
		efree(zdata);
		efree(zkey);
		efree(zsig);
		return FAILURE;
	}

	if (FAILURE == zend_fcall_info_init(openssl, 0, &fci, &fcc, NULL, NULL TSRMLS_CC)) {
		zval_dtor(zdata);
		zval_dtor(zsig);
		zval_dtor(zkey);
		zval_dtor(openssl);
		efree(openssl);
		efree(zdata);
		efree(zkey);
		efree(zsig);
		return FAILURE;
	}

	fci.param_count = 3;
	fci.params = zp;
	Z_ADDREF_P(zdata);
	if (is_sign) {
		Z_SET_ISREF_P(zsig);
	} else {
		Z_ADDREF_P(zsig);
	}
	Z_ADDREF_P(zkey);

	fci.retval_ptr_ptr = &retval_ptr;

	if (FAILURE == zend_call_function(&fci, &fcc TSRMLS_CC)) {
		zval_dtor(zdata);
		zval_dtor(zsig);
		zval_dtor(zkey);
		zval_dtor(openssl);
		efree(openssl);
		efree(zdata);
		efree(zkey);
		efree(zsig);
		return FAILURE;
	}

	zval_dtor(openssl);
	efree(openssl);
	Z_DELREF_P(zdata);

	if (is_sign) {
		Z_UNSET_ISREF_P(zsig);
	} else {
		Z_DELREF_P(zsig);
	}
	Z_DELREF_P(zkey);

	zval_dtor(zdata);
	efree(zdata);
	zval_dtor(zkey);
	efree(zkey);

	switch (Z_TYPE_P(retval_ptr)) {
		default:
		case IS_LONG:
			zval_dtor(zsig);
			efree(zsig);
			if (1 == Z_LVAL_P(retval_ptr)) {
				efree(retval_ptr);
				return SUCCESS;
			}
			efree(retval_ptr);
			return FAILURE;
		case IS_BOOL:
			efree(retval_ptr);
			if (Z_BVAL_P(retval_ptr)) {
				*signature = estrndup(Z_STRVAL_P(zsig), Z_STRLEN_P(zsig));
				*signature_len = Z_STRLEN_P(zsig);
				zval_dtor(zsig);
				efree(zsig);
				return SUCCESS;
			}
			zval_dtor(zsig);
			efree(zsig);
			return FAILURE;
	}
}
/* }}} */
#endif /* #ifndef PHAR_HAVE_OPENSSL */

int phar_verify_signature(php_stream *fp, size_t end_of_phar, php_uint32 sig_type, char *sig, int sig_len, char *fname, char **signature, int *signature_len, char **error TSRMLS_DC) /* {{{ */
{
	int read_size, len;
	off_t read_len;
	unsigned char buf[1024];

	php_stream_rewind(fp);

	switch (sig_type) {
		case PHAR_SIG_OPENSSL: {
#ifdef PHAR_HAVE_OPENSSL
			BIO *in;
			EVP_PKEY *key;
			EVP_MD *mdtype = (EVP_MD *) EVP_sha1();
			EVP_MD_CTX md_ctx;
#else
			int tempsig;
#endif
			php_uint32 pubkey_len;
			char *pubkey = NULL, *pfile;
			php_stream *pfp;
#ifndef PHAR_HAVE_OPENSSL
			if (!zend_hash_exists(&module_registry, "openssl", sizeof("openssl"))) {
				if (error) {
					spprintf(error, 0, "openssl not loaded");
				}
				return FAILURE;
			}
#endif
			/* use __FILE__ . '.pubkey' for public key file */
			spprintf(&pfile, 0, "%s.pubkey", fname);
			pfp = php_stream_open_wrapper(pfile, "rb", 0, NULL);
			efree(pfile);

#if PHP_MAJOR_VERSION > 5
			if (!pfp || !(pubkey_len = php_stream_copy_to_mem(pfp, (void **) &pubkey, PHP_STREAM_COPY_ALL, 0)) || !pubkey) {
#else
			if (!pfp || !(pubkey_len = php_stream_copy_to_mem(pfp, &pubkey, PHP_STREAM_COPY_ALL, 0)) || !pubkey) {
#endif
				if (pfp) {
					php_stream_close(pfp);
				}
				if (error) {
					spprintf(error, 0, "openssl public key could not be read");
				}
				return FAILURE;
			}

			php_stream_close(pfp);
#ifndef PHAR_HAVE_OPENSSL
			tempsig = sig_len;

			if (FAILURE == phar_call_openssl_signverify(0, fp, end_of_phar, pubkey, pubkey_len, &sig, &tempsig TSRMLS_CC)) {
				if (pubkey) {
					efree(pubkey);
				}

				if (error) {
					spprintf(error, 0, "openssl signature could not be verified");
				}

				return FAILURE;
			}

			if (pubkey) {
				efree(pubkey);
			}

			sig_len = tempsig;
#else
			in = BIO_new_mem_buf(pubkey, pubkey_len);

			if (NULL == in) {
				efree(pubkey);
				if (error) {
					spprintf(error, 0, "openssl signature could not be processed");
				}
				return FAILURE;
			}

			key = PEM_read_bio_PUBKEY(in, NULL,NULL, NULL);
			BIO_free(in);
			efree(pubkey);

			if (NULL == key) {
				if (error) {
					spprintf(error, 0, "openssl signature could not be processed");
				}
				return FAILURE;
			}

			EVP_VerifyInit(&md_ctx, mdtype);
			read_len = end_of_phar;

			if (read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (int)read_len;
			}

			php_stream_seek(fp, 0, SEEK_SET);

			while (read_size && (len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				EVP_VerifyUpdate (&md_ctx, buf, len);
				read_len -= (off_t)len;

				if (read_len < read_size) {
					read_size = (int)read_len;
				}
			}

			if (EVP_VerifyFinal(&md_ctx, (unsigned char *)sig, sig_len, key) != 1) {
				/* 1: signature verified, 0: signature does not match, -1: failed signature operation */
				EVP_MD_CTX_cleanup(&md_ctx);

				if (error) {
					spprintf(error, 0, "broken openssl signature");
				}

				return FAILURE;
			}

			EVP_MD_CTX_cleanup(&md_ctx);
#endif

			*signature_len = phar_hex_str((const char*)sig, sig_len, signature TSRMLS_CC);
		}
		break;
#ifdef PHAR_HASH_OK
		case PHAR_SIG_SHA512: {
			unsigned char digest[64];
			PHP_SHA512_CTX context;

			PHP_SHA512Init(&context);
			read_len = end_of_phar;

			if (read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (int)read_len;
			}

			while ((len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				PHP_SHA512Update(&context, buf, len);
				read_len -= (off_t)len;
				if (read_len < read_size) {
					read_size = (int)read_len;
				}
			}

			PHP_SHA512Final(digest, &context);

			if (memcmp(digest, sig, sizeof(digest))) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			*signature_len = phar_hex_str((const char*)digest, sizeof(digest), signature TSRMLS_CC);
			break;
		}
		case PHAR_SIG_SHA256: {
			unsigned char digest[32];
			PHP_SHA256_CTX context;

			PHP_SHA256Init(&context);
			read_len = end_of_phar;

			if (read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (int)read_len;
			}

			while ((len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				PHP_SHA256Update(&context, buf, len);
				read_len -= (off_t)len;
				if (read_len < read_size) {
					read_size = (int)read_len;
				}
			}

			PHP_SHA256Final(digest, &context);

			if (memcmp(digest, sig, sizeof(digest))) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			*signature_len = phar_hex_str((const char*)digest, sizeof(digest), signature TSRMLS_CC);
			break;
		}
#else
		case PHAR_SIG_SHA512:
		case PHAR_SIG_SHA256:
			if (error) {
				spprintf(error, 0, "unsupported signature");
			}
			return FAILURE;
#endif
		case PHAR_SIG_SHA1: {
			unsigned char digest[20];
			PHP_SHA1_CTX  context;

			PHP_SHA1Init(&context);
			read_len = end_of_phar;

			if (read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (int)read_len;
			}

			while ((len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				PHP_SHA1Update(&context, buf, len);
				read_len -= (off_t)len;
				if (read_len < read_size) {
					read_size = (int)read_len;
				}
			}

			PHP_SHA1Final(digest, &context);

			if (memcmp(digest, sig, sizeof(digest))) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			*signature_len = phar_hex_str((const char*)digest, sizeof(digest), signature TSRMLS_CC);
			break;
		}
		case PHAR_SIG_MD5: {
			unsigned char digest[16];
			PHP_MD5_CTX   context;

			PHP_MD5Init(&context);
			read_len = end_of_phar;

			if (read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (int)read_len;
			}

			while ((len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				PHP_MD5Update(&context, buf, len);
				read_len -= (off_t)len;
				if (read_len < read_size) {
					read_size = (int)read_len;
				}
			}

			PHP_MD5Final(digest, &context);

			if (memcmp(digest, sig, sizeof(digest))) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			*signature_len = phar_hex_str((const char*)digest, sizeof(digest), signature TSRMLS_CC);
			break;
		}
		default:
			if (error) {
				spprintf(error, 0, "broken or unsupported signature");
			}
			return FAILURE;
	}
	return SUCCESS;
}
/* }}} */
