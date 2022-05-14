SPL_METHOD(SplFileObject, getCsvControl)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
 	char delimiter[2], enclosure[2];
 
 	array_init(return_value);
 	delimiter[0] = intern->u.file.delimiter;
 	delimiter[1] = '\0';
 	enclosure[0] = intern->u.file.enclosure;
	enclosure[1] = '\0';

	add_next_index_string(return_value, delimiter, 1);
	add_next_index_string(return_value, enclosure, 1);
}
