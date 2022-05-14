int phar_parse_tarfile(php_stream* fp, char *fname, int fname_len, char *alias, int alias_len, phar_archive_data** pphar, int is_data, php_uint32 compression, char **error TSRMLS_DC) /* {{{ */
{
	char buf[512], *actual_alias = NULL, *p;
	phar_entry_info entry = {0};
	size_t pos = 0, read, totalsize;
	tar_header *hdr;
	php_uint32 sum1, sum2, size, old;
	phar_archive_data *myphar, **actual;
	int last_was_longlink = 0;

	if (error) {
		*error = NULL;
	}

	php_stream_seek(fp, 0, SEEK_END);
	totalsize = php_stream_tell(fp);
	php_stream_seek(fp, 0, SEEK_SET);
	read = php_stream_read(fp, buf, sizeof(buf));

	if (read != sizeof(buf)) {
		if (error) {
			spprintf(error, 4096, "phar error: \"%s\" is not a tar file or is truncated", fname);
		}
		php_stream_close(fp);
		return FAILURE;
	}

	hdr = (tar_header*)buf;
	old = (memcmp(hdr->magic, "ustar", sizeof("ustar")-1) != 0);

	myphar = (phar_archive_data *) pecalloc(1, sizeof(phar_archive_data), PHAR_G(persist));
	myphar->is_persistent = PHAR_G(persist);
	/* estimate number of entries, can't be certain with tar files */
	zend_hash_init(&myphar->manifest, 2 + (totalsize >> 12),
		zend_get_hash_value, destroy_phar_manifest_entry, (zend_bool)myphar->is_persistent);
	zend_hash_init(&myphar->mounted_dirs, 5,
		zend_get_hash_value, NULL, (zend_bool)myphar->is_persistent);
	zend_hash_init(&myphar->virtual_dirs, 4 + (totalsize >> 11),
		zend_get_hash_value, NULL, (zend_bool)myphar->is_persistent);
	myphar->is_tar = 1;
	/* remember whether this entire phar was compressed with gz/bzip2 */
	myphar->flags = compression;

	entry.is_tar = 1;
	entry.is_crc_checked = 1;
	entry.phar = myphar;
	pos += sizeof(buf);

	do {
		phar_entry_info *newentry;

		pos = php_stream_tell(fp);
		hdr = (tar_header*) buf;
		sum1 = phar_tar_number(hdr->checksum, sizeof(hdr->checksum));
		if (sum1 == 0 && phar_tar_checksum(buf, sizeof(buf)) == 0) {
			break;
		}
		memset(hdr->checksum, ' ', sizeof(hdr->checksum));
		sum2 = phar_tar_checksum(buf, old?sizeof(old_tar_header):sizeof(tar_header));

		size = entry.uncompressed_filesize = entry.compressed_filesize =
			phar_tar_number(hdr->size, sizeof(hdr->size));

		if (((!old && hdr->prefix[0] == 0) || old) && strlen(hdr->name) == sizeof(".phar/signature.bin")-1 && !strncmp(hdr->name, ".phar/signature.bin", sizeof(".phar/signature.bin")-1)) {
			off_t curloc;

			if (size > 511) {
				if (error) {
					spprintf(error, 4096, "phar error: tar-based phar \"%s\" has signature that is larger than 511 bytes, cannot process", fname);
				}
bail:
				php_stream_close(fp);
				phar_destroy_phar_data(myphar TSRMLS_CC);
				return FAILURE;
			}
			curloc = php_stream_tell(fp);
			read = php_stream_read(fp, buf, size);
			if (read != size) {
				if (error) {
					spprintf(error, 4096, "phar error: tar-based phar \"%s\" signature cannot be read", fname);
				}
				goto bail;
			}
#ifdef WORDS_BIGENDIAN
# define PHAR_GET_32(buffer) \
	(((((unsigned char*)(buffer))[3]) << 24) \
		| ((((unsigned char*)(buffer))[2]) << 16) \
		| ((((unsigned char*)(buffer))[1]) <<  8) \
		| (((unsigned char*)(buffer))[0]))
#else
# define PHAR_GET_32(buffer) (php_uint32) *(buffer)
#endif
			myphar->sig_flags = PHAR_GET_32(buf);
			if (FAILURE == phar_verify_signature(fp, php_stream_tell(fp) - size - 512, myphar->sig_flags, buf + 8, size - 8, fname, &myphar->signature, &myphar->sig_len, error TSRMLS_CC)) {
				if (error) {
					char *save = *error;
					spprintf(error, 4096, "phar error: tar-based phar \"%s\" signature cannot be verified: %s", fname, save);
					efree(save);
				}
				goto bail;
			}
			php_stream_seek(fp, curloc + 512, SEEK_SET);
			/* signature checked out, let's ensure this is the last file in the phar */
			if (((hdr->typeflag == '\0') || (hdr->typeflag == TAR_FILE)) && size > 0) {
				/* this is not good enough - seek succeeds even on truncated tars */
				php_stream_seek(fp, 512, SEEK_CUR);
				if ((uint)php_stream_tell(fp) > totalsize) {
					if (error) {
						spprintf(error, 4096, "phar error: \"%s\" is a corrupted tar file (truncated)", fname);
					}
					php_stream_close(fp);
					phar_destroy_phar_data(myphar TSRMLS_CC);
					return FAILURE;
				}
			}

			read = php_stream_read(fp, buf, sizeof(buf));

			if (read != sizeof(buf)) {
				if (error) {
					spprintf(error, 4096, "phar error: \"%s\" is a corrupted tar file (truncated)", fname);
				}
				php_stream_close(fp);
				phar_destroy_phar_data(myphar TSRMLS_CC);
				return FAILURE;
			}

			hdr = (tar_header*) buf;
			sum1 = phar_tar_number(hdr->checksum, sizeof(hdr->checksum));

			if (sum1 == 0 && phar_tar_checksum(buf, sizeof(buf)) == 0) {
				break;
			}

			if (error) {
				spprintf(error, 4096, "phar error: \"%s\" has entries after signature, invalid phar", fname);
			}

			goto bail;
		}

		if (!last_was_longlink && hdr->typeflag == 'L') {
			last_was_longlink = 1;
			/* support the ././@LongLink system for storing long filenames */
			entry.filename_len = entry.uncompressed_filesize;

			/* Check for overflow - bug 61065 */
			if (entry.filename_len == UINT_MAX) {
				if (error) {
					spprintf(error, 4096, "phar error: \"%s\" is a corrupted tar file (invalid entry size)", fname);
				}
				php_stream_close(fp);
				phar_destroy_phar_data(myphar TSRMLS_CC);
				return FAILURE;
			}
			entry.filename = pemalloc(entry.filename_len+1, myphar->is_persistent);

			read = php_stream_read(fp, entry.filename, entry.filename_len);
			if (read != entry.filename_len) {
				efree(entry.filename);
				if (error) {
					spprintf(error, 4096, "phar error: \"%s\" is a corrupted tar file (truncated)", fname);
				}
				php_stream_close(fp);
				phar_destroy_phar_data(myphar TSRMLS_CC);
				return FAILURE;
			}
			entry.filename[entry.filename_len] = '\0';

			/* skip blank stuff */
			size = ((size+511)&~511) - size;

			/* this is not good enough - seek succeeds even on truncated tars */
			php_stream_seek(fp, size, SEEK_CUR);
			if ((uint)php_stream_tell(fp) > totalsize) {
				efree(entry.filename);
				if (error) {
					spprintf(error, 4096, "phar error: \"%s\" is a corrupted tar file (truncated)", fname);
				}
				php_stream_close(fp);
				phar_destroy_phar_data(myphar TSRMLS_CC);
				return FAILURE;
                        }
 
                        read = php_stream_read(fp, buf, sizeof(buf));
                        if (read != sizeof(buf)) {
                                efree(entry.filename);
                                if (error) {
					spprintf(error, 4096, "phar error: \"%s\" is a corrupted tar file (truncated)", fname);
				}
				php_stream_close(fp);
				phar_destroy_phar_data(myphar TSRMLS_CC);
				return FAILURE;
			}
			continue;
		} else if (!last_was_longlink && !old && hdr->prefix[0] != 0) {
			char name[256];
			int i, j;

			for (i = 0; i < 155; i++) {
				name[i] = hdr->prefix[i];
				if (name[i] == '\0') {
					break;
				}
			}
			name[i++] = '/';
			for (j = 0; j < 100; j++) {
				name[i+j] = hdr->name[j];
				if (name[i+j] == '\0') {
					break;
				}
			}

			entry.filename_len = i+j;

			if (name[entry.filename_len - 1] == '/') {
				/* some tar programs store directories with trailing slash */
				entry.filename_len--;
			}
			entry.filename = pestrndup(name, entry.filename_len, myphar->is_persistent);
		} else if (!last_was_longlink) {
			int i;

			/* calculate strlen, which can be no longer than 100 */
			for (i = 0; i < 100; i++) {
				if (hdr->name[i] == '\0') {
					break;
				}
			}
			entry.filename_len = i;
			entry.filename = pestrndup(hdr->name, i, myphar->is_persistent);

			if (entry.filename[entry.filename_len - 1] == '/') {
				/* some tar programs store directories with trailing slash */
				entry.filename[entry.filename_len - 1] = '\0';
				entry.filename_len--;
			}
		}
		last_was_longlink = 0;

		phar_add_virtual_dirs(myphar, entry.filename, entry.filename_len TSRMLS_CC);

		if (sum1 != sum2) {
			if (error) {
				spprintf(error, 4096, "phar error: \"%s\" is a corrupted tar file (checksum mismatch of file \"%s\")", fname, entry.filename);
			}
			pefree(entry.filename, myphar->is_persistent);
			php_stream_close(fp);
			phar_destroy_phar_data(myphar TSRMLS_CC);
			return FAILURE;
		}

		entry.tar_type = ((old & (hdr->typeflag == '\0')) ? TAR_FILE : hdr->typeflag);
		entry.offset = entry.offset_abs = pos; /* header_offset unused in tar */
		entry.fp_type = PHAR_FP;
		entry.flags = phar_tar_number(hdr->mode, sizeof(hdr->mode)) & PHAR_ENT_PERM_MASK;
		entry.timestamp = phar_tar_number(hdr->mtime, sizeof(hdr->mtime));
		entry.is_persistent = myphar->is_persistent;
#ifndef S_ISDIR
#define S_ISDIR(mode)	(((mode)&S_IFMT) == S_IFDIR)
#endif
		if (old && entry.tar_type == TAR_FILE && S_ISDIR(entry.flags)) {
			entry.tar_type = TAR_DIR;
		}

		if (entry.tar_type == TAR_DIR) {
			entry.is_dir = 1;
		} else {
			entry.is_dir = 0;
		}

		entry.link = NULL;

		if (entry.tar_type == TAR_LINK) {
			if (!zend_hash_exists(&myphar->manifest, hdr->linkname, strlen(hdr->linkname))) {
				if (error) {
					spprintf(error, 4096, "phar error: \"%s\" is a corrupted tar file - hard link to non-existent file \"%s\"", fname, hdr->linkname);
				}
				pefree(entry.filename, entry.is_persistent);
				php_stream_close(fp);
				phar_destroy_phar_data(myphar TSRMLS_CC);
				return FAILURE;
			}
			entry.link = estrdup(hdr->linkname);
		} else if (entry.tar_type == TAR_SYMLINK) {
			entry.link = estrdup(hdr->linkname);
		}
		phar_set_inode(&entry TSRMLS_CC);
		zend_hash_add(&myphar->manifest, entry.filename, entry.filename_len, (void*)&entry, sizeof(phar_entry_info), (void **) &newentry);

		if (entry.is_persistent) {
			++entry.manifest_pos;
		}

		if (entry.filename_len >= sizeof(".phar/.metadata")-1 && !memcmp(entry.filename, ".phar/.metadata", sizeof(".phar/.metadata")-1)) {
			if (FAILURE == phar_tar_process_metadata(newentry, fp TSRMLS_CC)) {
				if (error) {
					spprintf(error, 4096, "phar error: tar-based phar \"%s\" has invalid metadata in magic file \"%s\"", fname, entry.filename);
				}
				php_stream_close(fp);
				phar_destroy_phar_data(myphar TSRMLS_CC);
				return FAILURE;
			}
		}

		if (!actual_alias && entry.filename_len == sizeof(".phar/alias.txt")-1 && !strncmp(entry.filename, ".phar/alias.txt", sizeof(".phar/alias.txt")-1)) {
			/* found explicit alias */
			if (size > 511) {
				if (error) {
					spprintf(error, 4096, "phar error: tar-based phar \"%s\" has alias that is larger than 511 bytes, cannot process", fname);
				}
				php_stream_close(fp);
				phar_destroy_phar_data(myphar TSRMLS_CC);
				return FAILURE;
			}

			read = php_stream_read(fp, buf, size);

			if (read == size) {
				buf[size] = '\0';
				if (!phar_validate_alias(buf, size)) {
					if (size > 50) {
						buf[50] = '.';
						buf[51] = '.';
						buf[52] = '.';
						buf[53] = '\0';
					}

					if (error) {
						spprintf(error, 4096, "phar error: invalid alias \"%s\" in tar-based phar \"%s\"", buf, fname);
					}

					php_stream_close(fp);
					phar_destroy_phar_data(myphar TSRMLS_CC);
					return FAILURE;
				}

				actual_alias = pestrndup(buf, size, myphar->is_persistent);
				myphar->alias = actual_alias;
				myphar->alias_len = size;
				php_stream_seek(fp, pos, SEEK_SET);
			} else {
				if (error) {
					spprintf(error, 4096, "phar error: Unable to read alias from tar-based phar \"%s\"", fname);
				}

				php_stream_close(fp);
				phar_destroy_phar_data(myphar TSRMLS_CC);
				return FAILURE;
			}
		}

		size = (size+511)&~511;

		if (((hdr->typeflag == '\0') || (hdr->typeflag == TAR_FILE)) && size > 0) {
			/* this is not good enough - seek succeeds even on truncated tars */
			php_stream_seek(fp, size, SEEK_CUR);
			if ((uint)php_stream_tell(fp) > totalsize) {
				if (error) {
					spprintf(error, 4096, "phar error: \"%s\" is a corrupted tar file (truncated)", fname);
				}
				php_stream_close(fp);
				phar_destroy_phar_data(myphar TSRMLS_CC);
				return FAILURE;
			}
		}

		read = php_stream_read(fp, buf, sizeof(buf));

		if (read != sizeof(buf)) {
			if (error) {
				spprintf(error, 4096, "phar error: \"%s\" is a corrupted tar file (truncated)", fname);
			}
			php_stream_close(fp);
			phar_destroy_phar_data(myphar TSRMLS_CC);
			return FAILURE;
		}
	} while (read != 0);

	if (zend_hash_exists(&(myphar->manifest), ".phar/stub.php", sizeof(".phar/stub.php")-1)) {
		myphar->is_data = 0;
	} else {
		myphar->is_data = 1;
	}

	/* ensure signature set */
	if (!myphar->is_data && PHAR_G(require_hash) && !myphar->signature) {
		php_stream_close(fp);
		phar_destroy_phar_data(myphar TSRMLS_CC);
		if (error) {
			spprintf(error, 0, "tar-based phar \"%s\" does not have a signature", fname);
		}
		return FAILURE;
	}

	myphar->fname = pestrndup(fname, fname_len, myphar->is_persistent);
#ifdef PHP_WIN32
	phar_unixify_path_separators(myphar->fname, fname_len);
#endif
	myphar->fname_len = fname_len;
	myphar->fp = fp;
	p = strrchr(myphar->fname, '/');

	if (p) {
		myphar->ext = memchr(p, '.', (myphar->fname + fname_len) - p);
		if (myphar->ext == p) {
			myphar->ext = memchr(p + 1, '.', (myphar->fname + fname_len) - p - 1);
		}
		if (myphar->ext) {
			myphar->ext_len = (myphar->fname + fname_len) - myphar->ext;
		}
	}

	phar_request_initialize(TSRMLS_C);

	if (SUCCESS != zend_hash_add(&(PHAR_GLOBALS->phar_fname_map), myphar->fname, fname_len, (void*)&myphar, sizeof(phar_archive_data*), (void **)&actual)) {
		if (error) {
			spprintf(error, 4096, "phar error: Unable to add tar-based phar \"%s\" to phar registry", fname);
		}
		php_stream_close(fp);
		phar_destroy_phar_data(myphar TSRMLS_CC);
		return FAILURE;
	}

	myphar = *actual;

	if (actual_alias) {
		phar_archive_data **fd_ptr;

		myphar->is_temporary_alias = 0;

		if (SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_alias_map), actual_alias, myphar->alias_len, (void **)&fd_ptr)) {
			if (SUCCESS != phar_free_alias(*fd_ptr, actual_alias, myphar->alias_len TSRMLS_CC)) {
				if (error) {
					spprintf(error, 4096, "phar error: Unable to add tar-based phar \"%s\", alias is already in use", fname);
				}
				zend_hash_del(&(PHAR_GLOBALS->phar_fname_map), myphar->fname, fname_len);
				return FAILURE;
			}
		}

		zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), actual_alias, myphar->alias_len, (void*)&myphar, sizeof(phar_archive_data*), NULL);
	} else {
		phar_archive_data **fd_ptr;

		if (alias_len) {
			if (SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void **)&fd_ptr)) {
				if (SUCCESS != phar_free_alias(*fd_ptr, alias, alias_len TSRMLS_CC)) {
					if (error) {
						spprintf(error, 4096, "phar error: Unable to add tar-based phar \"%s\", alias is already in use", fname);
					}
					zend_hash_del(&(PHAR_GLOBALS->phar_fname_map), myphar->fname, fname_len);
					return FAILURE;
				}
			}
			zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void*)&myphar, sizeof(phar_archive_data*), NULL);
			myphar->alias = pestrndup(alias, alias_len, myphar->is_persistent);
			myphar->alias_len = alias_len;
		} else {
			myphar->alias = pestrndup(myphar->fname, fname_len, myphar->is_persistent);
			myphar->alias_len = fname_len;
		}

		myphar->is_temporary_alias = 1;
	}

	if (pphar) {
		*pphar = myphar;
	}

	return SUCCESS;
}
/* }}} */
