static void spl_filesystem_tree_it_rewind(zend_object_iterator *iter TSRMLS_DC)
 {
 	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;
 	spl_filesystem_object   *object   = spl_filesystem_iterator_to_object(iterator);
 	object->u.dir.index = 0;
 	if (object->u.dir.dirp) {
 		php_stream_rewinddir(object->u.dir.dirp);
	}
	do {
		spl_filesystem_dir_read(object TSRMLS_CC);
	} while (spl_filesystem_is_dot(object->u.dir.entry.d_name));
	if (iterator->current) {
		zval_ptr_dtor(&iterator->current);
		iterator->current = NULL;
	}
}
