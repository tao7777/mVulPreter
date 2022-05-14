PHP_METHOD(Phar, getStub)
{
	size_t len;
	char *buf;
	php_stream *fp;
	php_stream_filter *filter = NULL;
	phar_entry_info *stub;

        phar_entry_info *stub;
 
        PHAR_ARCHIVE_OBJECT();

        if (zend_parse_parameters_none() == FAILURE) {
                return;
        }

		if (SUCCESS == zend_hash_find(&(phar_obj->arc.archive->manifest), ".phar/stub.php", sizeof(".phar/stub.php")-1, (void **)&stub)) {
			if (phar_obj->arc.archive->fp && !phar_obj->arc.archive->is_brandnew && !(stub->flags & PHAR_ENT_COMPRESSION_MASK)) {
				fp = phar_obj->arc.archive->fp;
			} else {
				if (!(fp = php_stream_open_wrapper(phar_obj->arc.archive->fname, "rb", 0, NULL))) {
					zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "phar error: unable to open phar \"%s\"", phar_obj->arc.archive->fname);
					return;
				}
				if (stub->flags & PHAR_ENT_COMPRESSION_MASK) {
					char *filter_name;

					if ((filter_name = phar_decompress_filter(stub, 0)) != NULL) {
						filter = php_stream_filter_create(filter_name, NULL, php_stream_is_persistent(fp) TSRMLS_CC);
					} else {
						filter = NULL;
					}
					if (!filter) {
						zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "phar error: unable to read stub of phar \"%s\" (cannot create %s filter)", phar_obj->arc.archive->fname, phar_decompress_filter(stub, 1));
						return;
					}
					php_stream_filter_append(&fp->readfilters, filter);
				}
			}

			if (!fp)  {
				zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC,
					"Unable to read stub");
				return;
			}

			php_stream_seek(fp, stub->offset_abs, SEEK_SET);
			len = stub->uncompressed_filesize;
			goto carry_on;
		} else {
			RETURN_STRINGL("", 0, 1);
		}
	}
