zend_object_iterator *spl_filesystem_dir_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC)
{
	spl_filesystem_iterator *iterator;
	spl_filesystem_object   *dir_object;

	if (by_ref) {
		zend_error(E_ERROR, "An iterator cannot be used with foreach by reference");
	}
	dir_object = (spl_filesystem_object*)zend_object_store_get_object(object TSRMLS_CC);
	iterator   = spl_filesystem_object_to_iterator(dir_object);

	/* initialize iterator if it wasn't gotten before */
	if (iterator->intern.data == NULL) {
		iterator->intern.data = object;
		iterator->intern.funcs = &spl_filesystem_dir_it_funcs;
		/* ->current must be initialized; rewind doesn't set it and valid
		 * doesn't check whether it's set */
 		iterator->current = object;
 	}
 	zval_add_ref(&object);

 	return (zend_object_iterator*)iterator;
 }
