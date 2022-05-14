 static void spl_filesystem_dir_it_rewind(zend_object_iterator *iter TSRMLS_DC)
 {
 	spl_filesystem_object *object = spl_filesystem_iterator_to_object((spl_filesystem_iterator *)iter);

 	object->u.dir.index = 0;
 	if (object->u.dir.dirp) {
 		php_stream_rewinddir(object->u.dir.dirp);
	}
	spl_filesystem_dir_read(object TSRMLS_CC);
}
