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
