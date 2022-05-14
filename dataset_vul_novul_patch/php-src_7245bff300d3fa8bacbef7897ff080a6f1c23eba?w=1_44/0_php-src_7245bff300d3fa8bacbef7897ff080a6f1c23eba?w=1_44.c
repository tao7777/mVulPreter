 static void spl_filesystem_dir_it_move_forward(zend_object_iterator *iter TSRMLS_DC)
 {
 	spl_filesystem_object *object = spl_filesystem_iterator_to_object((spl_filesystem_iterator *)iter);

 	object->u.dir.index++;
 	spl_filesystem_dir_read(object TSRMLS_CC);
 	if (object->file_name) {
		efree(object->file_name);
		object->file_name = NULL;
	}
}
