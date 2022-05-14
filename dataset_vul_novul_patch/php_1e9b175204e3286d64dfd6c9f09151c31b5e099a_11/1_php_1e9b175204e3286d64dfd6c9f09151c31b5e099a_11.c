PHP_METHOD(Phar, offsetUnset)
{
	char *fname, *error;
	size_t fname_len;
	phar_entry_info *entry;
	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->archive->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Write operations disabled by the php.ini setting phar.readonly");
                return;
        }
 
       if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &fname, &fname_len) == FAILURE) {
                return;
        }
 
	if (zend_hash_str_exists(&phar_obj->archive->manifest, fname, (uint) fname_len)) {
		if (NULL != (entry = zend_hash_str_find_ptr(&phar_obj->archive->manifest, fname, (uint) fname_len))) {
			if (entry->is_deleted) {
				/* entry is deleted, but has not been flushed to disk yet */
				return;
			}

			if (phar_obj->archive->is_persistent) {
				if (FAILURE == phar_copy_on_write(&(phar_obj->archive))) {
					zend_throw_exception_ex(phar_ce_PharException, 0, "phar \"%s\" is persistent, unable to copy on write", phar_obj->archive->fname);
					return;
				}
				/* re-populate entry after copy on write */
				entry = zend_hash_str_find_ptr(&phar_obj->archive->manifest, fname, (uint) fname_len);
			}
			entry->is_modified = 0;
			entry->is_deleted = 1;
			/* we need to "flush" the stream to save the newly deleted file on disk */
			phar_flush(phar_obj->archive, 0, 0, 0, &error);

			if (error) {
				zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
				efree(error);
			}

			RETURN_TRUE;
		}
	} else {
		RETURN_FALSE;
	}
}
