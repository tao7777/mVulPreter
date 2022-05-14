static int spl_array_has_dimension_ex(int check_inherited, zval *object, zval *offset, int check_empty TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	long index;
	zval *rv, *value = NULL, **tmp;

	if (check_inherited && intern->fptr_offset_has) {
		zval *offset_tmp = offset;
		SEPARATE_ARG_IF_REF(offset_tmp);
		zend_call_method_with_1_params(&object, Z_OBJCE_P(object), &intern->fptr_offset_has, "offsetExists", &rv, offset_tmp);
		zval_ptr_dtor(&offset_tmp);

		if (rv && zend_is_true(rv)) {
			zval_ptr_dtor(&rv);
			if (check_empty != 1) {
				return 1;
			} else if (intern->fptr_offset_get) {
				value = spl_array_read_dimension_ex(1, object, offset, BP_VAR_R TSRMLS_CC);
			}
		} else {
			if (rv) {
				zval_ptr_dtor(&rv);
			}
			return 0;
		}
	}

	if (!value) {
 		HashTable *ht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);
 
 		switch(Z_TYPE_P(offset)) {
			case IS_STRING: 
 				if (zend_symtable_find(ht, Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, (void **) &tmp) != FAILURE) {
 					if (check_empty == 2) {
 						return 1;
					}
				} else {
					return 0;
				}
				break;
 
 			case IS_DOUBLE:
 			case IS_RESOURCE:
			case IS_BOOL: 
 			case IS_LONG:
 				if (offset->type == IS_DOUBLE) {
 					index = (long)Z_DVAL_P(offset);
				} else {
					index = Z_LVAL_P(offset);
				}
				if (zend_hash_index_find(ht, index, (void **)&tmp) != FAILURE) {
					if (check_empty == 2) {
						return 1;
					}
				} else {
					return 0;
				}
				break;

			default:
				zend_error(E_WARNING, "Illegal offset type");
				return 0;
		}

		if (check_empty && check_inherited && intern->fptr_offset_get) {
			value = spl_array_read_dimension_ex(1, object, offset, BP_VAR_R TSRMLS_CC);
		} else {
			value = *tmp;
		}
	}

	return check_empty ? zend_is_true(value) : Z_TYPE_P(value) != IS_NULL;
} /* }}} */
