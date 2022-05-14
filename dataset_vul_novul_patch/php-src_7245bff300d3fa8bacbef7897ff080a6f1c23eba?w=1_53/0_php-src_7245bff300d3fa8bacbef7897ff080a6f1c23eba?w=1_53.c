static zend_object_value spl_filesystem_object_clone(zval *zobject TSRMLS_DC)
{
	zend_object_value new_obj_val;
	zend_object *old_object;
	zend_object *new_object;
	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);
	spl_filesystem_object *intern;
	spl_filesystem_object *source;
	int index, skip_dots;

	old_object = zend_objects_get_address(zobject TSRMLS_CC);
	source = (spl_filesystem_object*)old_object;

	new_obj_val = spl_filesystem_object_new_ex(old_object->ce, &intern TSRMLS_CC);
	new_object = &intern->std;

	intern->flags = source->flags;

	switch (source->type) {
	case SPL_FS_INFO:
		intern->_path_len = source->_path_len;
		intern->_path = estrndup(source->_path, source->_path_len);
		intern->file_name_len = source->file_name_len;
		intern->file_name = estrndup(source->file_name, intern->file_name_len);
		break;
	case SPL_FS_DIR:
		spl_filesystem_dir_open(intern, source->_path TSRMLS_CC);
		/* read until we hit the position in which we were before */
		skip_dots = SPL_HAS_FLAG(source->flags, SPL_FILE_DIR_SKIPDOTS);
		for(index = 0; index < source->u.dir.index; ++index) {
			do {
				spl_filesystem_dir_read(intern TSRMLS_CC);
			} while (skip_dots && spl_filesystem_is_dot(intern->u.dir.entry.d_name));
		}
		intern->u.dir.index = index;
		break;
	case SPL_FS_FILE:
 		php_error_docref(NULL TSRMLS_CC, E_ERROR, "An object of class %s cannot be cloned", old_object->ce->name);
 		break;
 	}

 	intern->file_class = source->file_class;
 	intern->info_class = source->info_class;
 	intern->oth = source->oth;
	intern->oth_handler = source->oth_handler;

	zend_objects_clone_members(new_object, new_obj_val, old_object, handle TSRMLS_CC);

	if (intern->oth_handler && intern->oth_handler->clone) {
		intern->oth_handler->clone(source, intern TSRMLS_CC);
	}

	return new_obj_val;
}
