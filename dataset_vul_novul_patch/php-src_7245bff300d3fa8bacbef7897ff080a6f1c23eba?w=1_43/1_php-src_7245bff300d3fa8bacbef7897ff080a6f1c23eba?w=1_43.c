 static void spl_filesystem_dir_it_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC)
 {
 	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;
 	*data = &iterator->current;
 }
