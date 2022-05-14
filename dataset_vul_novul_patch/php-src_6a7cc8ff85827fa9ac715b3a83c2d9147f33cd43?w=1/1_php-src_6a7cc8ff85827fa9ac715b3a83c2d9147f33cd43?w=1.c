ZEND_API void zend_object_store_ctor_failed(zval *zobject TSRMLS_DC)
 {
 	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);
 	zend_object_store_bucket *obj_bucket = &EG(objects_store).object_buckets[handle];
	obj_bucket->bucket.obj.handlers = Z_OBJ_HT_P(zobject);;
 	obj_bucket->destructor_called = 1;
 }
