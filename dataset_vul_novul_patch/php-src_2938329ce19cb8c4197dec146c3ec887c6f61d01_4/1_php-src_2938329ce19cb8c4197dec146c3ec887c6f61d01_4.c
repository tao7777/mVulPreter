PHP_FUNCTION(imageaffinematrixconcat)
{
	double m1[6];
	double m2[6];
	double mr[6];

	zval **tmp;
	zval *z_m1;
	zval *z_m2;
	int i, nelems;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "aa", &z_m1, &z_m2) == FAILURE)  {
		return;
	}

	if (((nelems = zend_hash_num_elements(Z_ARRVAL_P(z_m1))) != 6) || (nelems = zend_hash_num_elements(Z_ARRVAL_P(z_m2))) != 6) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Affine arrays must have six elements");
		RETURN_FALSE;
	}

	for (i = 0; i < 6; i++) {
		if (zend_hash_index_find(Z_ARRVAL_P(z_m1), i, (void **) &tmp) == SUCCESS) {
			switch (Z_TYPE_PP(tmp)) {
				case IS_LONG:
					m1[i]  = Z_LVAL_PP(tmp);
					break;
				case IS_DOUBLE:
 					m1[i] = Z_DVAL_PP(tmp);
 					break;
 				case IS_STRING:
					convert_to_double_ex(tmp);
					m1[i] = Z_DVAL_PP(tmp);
 					break;
 				default:
 					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid type for element %i", i);
					RETURN_FALSE;
			}
		}
		if (zend_hash_index_find(Z_ARRVAL_P(z_m2), i, (void **) &tmp) == SUCCESS) {
			switch (Z_TYPE_PP(tmp)) {
				case IS_LONG:
					m2[i]  = Z_LVAL_PP(tmp);
					break;
				case IS_DOUBLE:
 					m2[i] = Z_DVAL_PP(tmp);
 					break;
 				case IS_STRING:
					convert_to_double_ex(tmp);
					m2[i] = Z_DVAL_PP(tmp);
 					break;
 				default:
 					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid type for element %i", i);
					RETURN_FALSE;
			}
		}
	}

	if (gdAffineConcat (mr, m1, m2) != GD_TRUE) {
		RETURN_FALSE;
	}

	array_init(return_value);
	for (i = 0; i < 6; i++) {
		add_index_double(return_value, i, mr[i]);
	}
}
