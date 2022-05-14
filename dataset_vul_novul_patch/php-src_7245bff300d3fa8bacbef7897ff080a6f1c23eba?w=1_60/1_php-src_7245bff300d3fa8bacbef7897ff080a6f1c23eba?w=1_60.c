static void spl_filesystem_tree_it_move_forward(zend_object_iterator *iter TSRMLS_DC)
 {
 	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;
 	spl_filesystem_object   *object   = spl_filesystem_iterator_to_object(iterator);
 	object->u.dir.index++;
 	do {
 		spl_filesystem_dir_read(object TSRMLS_CC);
	} while (spl_filesystem_is_dot(object->u.dir.entry.d_name));
	if (object->file_name) {
		efree(object->file_name);
		object->file_name = NULL;
	}
	if (iterator->current) {
		zval_ptr_dtor(&iterator->current);
		iterator->current = NULL;
	}
}
