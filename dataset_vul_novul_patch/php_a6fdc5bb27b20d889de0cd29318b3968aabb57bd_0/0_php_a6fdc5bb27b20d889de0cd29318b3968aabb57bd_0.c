int phar_zip_flush(phar_archive_data *phar, char *user_stub, long len, int defaultstub, char **error TSRMLS_DC) /* {{{ */
{
	char *pos;
	smart_str main_metadata_str = {0};
        static const char newstub[] = "<?php // zip-based phar archive stub file\n__HALT_COMPILER();";
        char halt_stub[] = "__HALT_COMPILER();";
        char *tmp;

        php_stream *stubfile, *oldfile;
        php_serialize_data_t metadata_hash;
        int free_user_stub, closeoldfile = 0;
	phar_entry_info entry = {0};
	char *temperr = NULL;
	struct _phar_zip_pass pass;
	phar_zip_dir_end eocd;
	php_uint32 cdir_size, cdir_offset;

	pass.error = &temperr;
	entry.flags = PHAR_ENT_PERM_DEF_FILE;
	entry.timestamp = time(NULL);
	entry.is_modified = 1;
	entry.is_zip = 1;
	entry.phar = phar;
	entry.fp_type = PHAR_MOD;

	if (phar->is_persistent) {
		if (error) {
			spprintf(error, 0, "internal error: attempt to flush cached zip-based phar \"%s\"", phar->fname);
		}
		return EOF;
	}

	if (phar->is_data) {
		goto nostub;
	}

	/* set alias */
	if (!phar->is_temporary_alias && phar->alias_len) {
		entry.fp = php_stream_fopen_tmpfile();
		if (entry.fp == NULL) {
			spprintf(error, 0, "phar error: unable to create temporary file");
			return EOF;
		}
		if (phar->alias_len != (int)php_stream_write(entry.fp, phar->alias, phar->alias_len)) {
			if (error) {
				spprintf(error, 0, "unable to set alias in zip-based phar \"%s\"", phar->fname);
			}
			return EOF;
		}

		entry.uncompressed_filesize = entry.compressed_filesize = phar->alias_len;
		entry.filename = estrndup(".phar/alias.txt", sizeof(".phar/alias.txt")-1);
		entry.filename_len = sizeof(".phar/alias.txt")-1;

		if (SUCCESS != zend_hash_update(&phar->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info), NULL)) {
			if (error) {
				spprintf(error, 0, "unable to set alias in zip-based phar \"%s\"", phar->fname);
			}
			return EOF;
		}
	} else {
		zend_hash_del(&phar->manifest, ".phar/alias.txt", sizeof(".phar/alias.txt")-1);
	}

	/* register alias */
	if (phar->alias_len) {
		if (FAILURE == phar_get_archive(&phar, phar->fname, phar->fname_len, phar->alias, phar->alias_len, error TSRMLS_CC)) {
			return EOF;
		}
	}

	/* set stub */
	if (user_stub && !defaultstub) {
		if (len < 0) {
			/* resource passed in */
			if (!(php_stream_from_zval_no_verify(stubfile, (zval **)user_stub))) {
				if (error) {
					spprintf(error, 0, "unable to access resource to copy stub to new zip-based phar \"%s\"", phar->fname);
				}
				return EOF;
			}

			if (len == -1) {
				len = PHP_STREAM_COPY_ALL;
			} else {
				len = -len;
			}

			user_stub = 0;

			if (!(len = php_stream_copy_to_mem(stubfile, &user_stub, len, 0)) || !user_stub) {
				if (error) {
					spprintf(error, 0, "unable to read resource to copy stub to new zip-based phar \"%s\"", phar->fname);
				}
				return EOF;
			}
			free_user_stub = 1;
		} else {
			free_user_stub = 0;
		}

		tmp = estrndup(user_stub, len);
		if ((pos = php_stristr(tmp, halt_stub, len, sizeof(halt_stub) - 1)) == NULL) {
			efree(tmp);
			if (error) {
				spprintf(error, 0, "illegal stub for zip-based phar \"%s\"", phar->fname);
			}
			if (free_user_stub) {
				efree(user_stub);
			}
			return EOF;
		}
		pos = user_stub + (pos - tmp);
		efree(tmp);

		len = pos - user_stub + 18;
		entry.fp = php_stream_fopen_tmpfile();
		if (entry.fp == NULL) {
			spprintf(error, 0, "phar error: unable to create temporary file");
			return EOF;
		}
		entry.uncompressed_filesize = len + 5;

		if ((size_t)len != php_stream_write(entry.fp, user_stub, len)
		||            5 != php_stream_write(entry.fp, " ?>\r\n", 5)) {
			if (error) {
				spprintf(error, 0, "unable to create stub from string in new zip-based phar \"%s\"", phar->fname);
			}
			if (free_user_stub) {
				efree(user_stub);
			}
			php_stream_close(entry.fp);
			return EOF;
		}

		entry.filename = estrndup(".phar/stub.php", sizeof(".phar/stub.php")-1);
		entry.filename_len = sizeof(".phar/stub.php")-1;

		if (SUCCESS != zend_hash_update(&phar->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info), NULL)) {
			if (free_user_stub) {
				efree(user_stub);
			}
			if (error) {
				spprintf(error, 0, "unable to set stub in zip-based phar \"%s\"", phar->fname);
			}
			return EOF;
		}

		if (free_user_stub) {
			efree(user_stub);
		}
	} else {
		/* Either this is a brand new phar (add the stub), or the default stub is required (overwrite the stub) */
		entry.fp = php_stream_fopen_tmpfile();
		if (entry.fp == NULL) {
			spprintf(error, 0, "phar error: unable to create temporary file");
			return EOF;
		}
		if (sizeof(newstub)-1 != php_stream_write(entry.fp, newstub, sizeof(newstub)-1)) {
			php_stream_close(entry.fp);
			if (error) {
				spprintf(error, 0, "unable to %s stub in%szip-based phar \"%s\", failed", user_stub ? "overwrite" : "create", user_stub ? " " : " new ", phar->fname);
			}
			return EOF;
		}

		entry.uncompressed_filesize = entry.compressed_filesize = sizeof(newstub) - 1;
		entry.filename = estrndup(".phar/stub.php", sizeof(".phar/stub.php")-1);
		entry.filename_len = sizeof(".phar/stub.php")-1;

		if (!defaultstub) {
			if (!zend_hash_exists(&phar->manifest, ".phar/stub.php", sizeof(".phar/stub.php")-1)) {
				if (SUCCESS != zend_hash_add(&phar->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info), NULL)) {
					php_stream_close(entry.fp);
					efree(entry.filename);
					if (error) {
						spprintf(error, 0, "unable to create stub in zip-based phar \"%s\"", phar->fname);
					}
					return EOF;
				}
			} else {
				php_stream_close(entry.fp);
				efree(entry.filename);
			}
		} else {
			if (SUCCESS != zend_hash_update(&phar->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info), NULL)) {
				php_stream_close(entry.fp);
				efree(entry.filename);
				if (error) {
					spprintf(error, 0, "unable to overwrite stub in zip-based phar \"%s\"", phar->fname);
				}
				return EOF;
			}
		}
	}
nostub:
	if (phar->fp && !phar->is_brandnew) {
		oldfile = phar->fp;
		closeoldfile = 0;
		php_stream_rewind(oldfile);
	} else {
		oldfile = php_stream_open_wrapper(phar->fname, "rb", 0, NULL);
		closeoldfile = oldfile != NULL;
	}

	/* save modified files to the zip */
	pass.old = oldfile;
	pass.filefp = php_stream_fopen_tmpfile();

	if (!pass.filefp) {
fperror:
		if (closeoldfile) {
			php_stream_close(oldfile);
		}
		if (error) {
			spprintf(error, 4096, "phar zip flush of \"%s\" failed: unable to open temporary file", phar->fname);
		}
		return EOF;
	}

	pass.centralfp = php_stream_fopen_tmpfile();

	if (!pass.centralfp) {
		goto fperror;
	}

	pass.free_fp = pass.free_ufp = 1;
	memset(&eocd, 0, sizeof(eocd));

	strncpy(eocd.signature, "PK\5\6", 4);
	if (!phar->is_data && !phar->sig_flags) {
		phar->sig_flags = PHAR_SIG_SHA1;
	}
	if (phar->sig_flags) {
		PHAR_SET_16(eocd.counthere, zend_hash_num_elements(&phar->manifest) + 1);
		PHAR_SET_16(eocd.count, zend_hash_num_elements(&phar->manifest) + 1);
	} else {
		PHAR_SET_16(eocd.counthere, zend_hash_num_elements(&phar->manifest));
		PHAR_SET_16(eocd.count, zend_hash_num_elements(&phar->manifest));
	}
	zend_hash_apply_with_argument(&phar->manifest, phar_zip_changed_apply, (void *) &pass TSRMLS_CC);

	if (phar->metadata) {
		/* set phar metadata */
		PHP_VAR_SERIALIZE_INIT(metadata_hash);
		php_var_serialize(&main_metadata_str, &phar->metadata, &metadata_hash TSRMLS_CC);
		PHP_VAR_SERIALIZE_DESTROY(metadata_hash);
	}
	if (temperr) {
		if (error) {
			spprintf(error, 4096, "phar zip flush of \"%s\" failed: %s", phar->fname, temperr);
		}
		efree(temperr);
temperror:
		php_stream_close(pass.centralfp);
nocentralerror:
		if (phar->metadata) {
			smart_str_free(&main_metadata_str);
		}
		php_stream_close(pass.filefp);
		if (closeoldfile) {
			php_stream_close(oldfile);
		}
		return EOF;
	}

	if (FAILURE == phar_zip_applysignature(phar, &pass, &main_metadata_str TSRMLS_CC)) {
		goto temperror;
	}

	/* save zip */
	cdir_size = php_stream_tell(pass.centralfp);
	cdir_offset = php_stream_tell(pass.filefp);
	PHAR_SET_32(eocd.cdir_size, cdir_size);
	PHAR_SET_32(eocd.cdir_offset, cdir_offset);
	php_stream_seek(pass.centralfp, 0, SEEK_SET);

	{
		size_t clen;
		int ret = phar_stream_copy_to_stream(pass.centralfp, pass.filefp, PHP_STREAM_COPY_ALL, &clen);
		if (SUCCESS != ret || clen != cdir_size) {
			if (error) {
				spprintf(error, 4096, "phar zip flush of \"%s\" failed: unable to write central-directory", phar->fname);
			}
			goto temperror;
		}
	}

	php_stream_close(pass.centralfp);

	if (phar->metadata) {
		/* set phar metadata */
		PHAR_SET_16(eocd.comment_len, main_metadata_str.len);

		if (sizeof(eocd) != php_stream_write(pass.filefp, (char *)&eocd, sizeof(eocd))) {
			if (error) {
				spprintf(error, 4096, "phar zip flush of \"%s\" failed: unable to write end of central-directory", phar->fname);
			}
			goto nocentralerror;
		}

		if (main_metadata_str.len != php_stream_write(pass.filefp, main_metadata_str.c, main_metadata_str.len)) {
			if (error) {
				spprintf(error, 4096, "phar zip flush of \"%s\" failed: unable to write metadata to zip comment", phar->fname);
			}
			goto nocentralerror;
		}

		smart_str_free(&main_metadata_str);

	} else {
		if (sizeof(eocd) != php_stream_write(pass.filefp, (char *)&eocd, sizeof(eocd))) {
			if (error) {
				spprintf(error, 4096, "phar zip flush of \"%s\" failed: unable to write end of central-directory", phar->fname);
			}
			goto nocentralerror;
		}
	}

	if (phar->fp && pass.free_fp) {
		php_stream_close(phar->fp);
	}

	if (phar->ufp) {
		if (pass.free_ufp) {
			php_stream_close(phar->ufp);
		}
		phar->ufp = NULL;
	}

	/* re-open */
	phar->is_brandnew = 0;

	if (phar->donotflush) {
		/* deferred flush */
		phar->fp = pass.filefp;
	} else {
		phar->fp = php_stream_open_wrapper(phar->fname, "w+b", IGNORE_URL|STREAM_MUST_SEEK|REPORT_ERRORS, NULL);
		if (!phar->fp) {
			if (closeoldfile) {
				php_stream_close(oldfile);
			}
			phar->fp = pass.filefp;
			if (error) {
				spprintf(error, 4096, "unable to open new phar \"%s\" for writing", phar->fname);
			}
			return EOF;
		}
		php_stream_rewind(pass.filefp);
		phar_stream_copy_to_stream(pass.filefp, phar->fp, PHP_STREAM_COPY_ALL, NULL);
		/* we could also reopen the file in "rb" mode but there is no need for that */
		php_stream_close(pass.filefp);
	}

	if (closeoldfile) {
		php_stream_close(oldfile);
	}
	return EOF;
}
/* }}} */
