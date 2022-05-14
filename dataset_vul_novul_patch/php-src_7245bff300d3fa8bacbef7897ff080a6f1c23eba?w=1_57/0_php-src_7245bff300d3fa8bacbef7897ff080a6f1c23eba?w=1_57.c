static HashTable* spl_filesystem_object_get_debug_info(zval *obj, int *is_temp TSRMLS_DC) /* {{{ */
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(obj TSRMLS_CC);
	HashTable *rv;
	zval *tmp, zrv;
	char *pnstr, *path;
	int  pnlen, path_len;
	char stmp[2];

	*is_temp = 1;

	if (!intern->std.properties) {
		rebuild_object_properties(&intern->std);
	}

	ALLOC_HASHTABLE(rv);
	ZEND_INIT_SYMTABLE_EX(rv, zend_hash_num_elements(intern->std.properties) + 3, 0);

	INIT_PZVAL(&zrv);
	Z_ARRVAL(zrv) = rv;

	zend_hash_copy(rv, intern->std.properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	pnstr = spl_gen_private_prop_name(spl_ce_SplFileInfo, "pathName", sizeof("pathName")-1, &pnlen TSRMLS_CC);
	path = spl_filesystem_object_get_pathname(intern, &path_len TSRMLS_CC);
	add_assoc_stringl_ex(&zrv, pnstr, pnlen+1, path, path_len, 1);
	efree(pnstr);

 	if (intern->file_name) {
 		pnstr = spl_gen_private_prop_name(spl_ce_SplFileInfo, "fileName", sizeof("fileName")-1, &pnlen TSRMLS_CC);
 		spl_filesystem_object_get_path(intern, &path_len TSRMLS_CC);

 		if (path_len && path_len < intern->file_name_len) {
 			add_assoc_stringl_ex(&zrv, pnstr, pnlen+1, intern->file_name + path_len + 1, intern->file_name_len - (path_len + 1), 1);
 		} else {
			add_assoc_stringl_ex(&zrv, pnstr, pnlen+1, intern->file_name, intern->file_name_len, 1);
		}
		efree(pnstr);
	}
	if (intern->type == SPL_FS_DIR) {
#ifdef HAVE_GLOB
		pnstr = spl_gen_private_prop_name(spl_ce_DirectoryIterator, "glob", sizeof("glob")-1, &pnlen TSRMLS_CC);
		if (php_stream_is(intern->u.dir.dirp ,&php_glob_stream_ops)) {
			add_assoc_stringl_ex(&zrv, pnstr, pnlen+1, intern->_path, intern->_path_len, 1);
		} else {
			add_assoc_bool_ex(&zrv, pnstr, pnlen+1, 0);
		}
		efree(pnstr);
#endif
		pnstr = spl_gen_private_prop_name(spl_ce_RecursiveDirectoryIterator, "subPathName", sizeof("subPathName")-1, &pnlen TSRMLS_CC);
		if (intern->u.dir.sub_path) {
			add_assoc_stringl_ex(&zrv, pnstr, pnlen+1, intern->u.dir.sub_path, intern->u.dir.sub_path_len, 1);
		} else {
			add_assoc_stringl_ex(&zrv, pnstr, pnlen+1, "", 0, 1);
		}
		efree(pnstr);
	}
	if (intern->type == SPL_FS_FILE) {
		pnstr = spl_gen_private_prop_name(spl_ce_SplFileObject, "openMode", sizeof("openMode")-1, &pnlen TSRMLS_CC);
		add_assoc_stringl_ex(&zrv, pnstr, pnlen+1, intern->u.file.open_mode, intern->u.file.open_mode_len, 1);
		efree(pnstr);
		stmp[1] = '\0';
		stmp[0] = intern->u.file.delimiter;
		pnstr = spl_gen_private_prop_name(spl_ce_SplFileObject, "delimiter", sizeof("delimiter")-1, &pnlen TSRMLS_CC);
		add_assoc_stringl_ex(&zrv, pnstr, pnlen+1, stmp, 1, 1);
		efree(pnstr);
		stmp[0] = intern->u.file.enclosure;
		pnstr = spl_gen_private_prop_name(spl_ce_SplFileObject, "enclosure", sizeof("enclosure")-1, &pnlen TSRMLS_CC);
		add_assoc_stringl_ex(&zrv, pnstr, pnlen+1, stmp, 1, 1);
		efree(pnstr);
	}

	return rv;
}
/* }}} */
