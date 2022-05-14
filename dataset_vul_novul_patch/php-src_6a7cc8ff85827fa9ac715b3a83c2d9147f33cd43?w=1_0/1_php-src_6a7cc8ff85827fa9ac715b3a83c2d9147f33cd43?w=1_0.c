ZEND_API void zend_objects_store_del_ref_by_handle_ex(zend_object_handle handle, const zend_object_handlers *handlers TSRMLS_DC) /* {{{ */
{
	struct _store_object *obj;
	int failure = 0;

	if (!EG(objects_store).object_buckets) {
		return;
	}

	obj = &EG(objects_store).object_buckets[handle].bucket.obj;

	/*	Make sure we hold a reference count during the destructor call
		otherwise, when the destructor ends the storage might be freed
		when the refcount reaches 0 a second time
	 */
	if (EG(objects_store).object_buckets[handle].valid) {
		if (obj->refcount == 1) {
			if (!EG(objects_store).object_buckets[handle].destructor_called) {
				EG(objects_store).object_buckets[handle].destructor_called = 1;

				if (obj->dtor) {
					if (handlers && !obj->handlers) {
						obj->handlers = handlers;
					}
					zend_try {
						obj->dtor(obj->object, handle TSRMLS_CC);
					} zend_catch {
						failure = 1;
 					} zend_end_try();
 				}
 			}
 			/* re-read the object from the object store as the store might have been reallocated in the dtor */
 			obj = &EG(objects_store).object_buckets[handle].bucket.obj;
 
			if (obj->refcount == 1) {
				GC_REMOVE_ZOBJ_FROM_BUFFER(obj);
				if (obj->free_storage) {
					zend_try {
						obj->free_storage(obj->object TSRMLS_CC);
					} zend_catch {
						failure = 1;
					} zend_end_try();
				}
				ZEND_OBJECTS_STORE_ADD_TO_FREE_LIST();
			}
		}
	}

	obj->refcount--;

#if ZEND_DEBUG_OBJECTS
	if (obj->refcount == 0) {
		fprintf(stderr, "Deallocated object id #%d\n", handle);
	} else {
		fprintf(stderr, "Decreased refcount of object id #%d\n", handle);
	}
#endif
	if (failure) {
		zend_bailout();
	}
}
/* }}} */
