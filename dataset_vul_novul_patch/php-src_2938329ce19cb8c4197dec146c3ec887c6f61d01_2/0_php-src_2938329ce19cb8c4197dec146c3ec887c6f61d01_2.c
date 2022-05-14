PHP_FUNCTION(imageaffine)
{
	zval *IM;
	gdImagePtr src;
	gdImagePtr dst;
	gdRect rect;
	gdRectPtr pRect = NULL;
	zval *z_rect = NULL;
	zval *z_affine;
	zval **tmp;
	double affine[6];
	int i, nelems;
	zval **zval_affine_elem = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra|a", &IM, &z_affine, &z_rect) == FAILURE)  {
		return;
	}

	ZEND_FETCH_RESOURCE(src, gdImagePtr, &IM, -1, "Image", le_gd);

	if ((nelems = zend_hash_num_elements(Z_ARRVAL_P(z_affine))) != 6) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Affine array must have six elements");
		RETURN_FALSE;
	}

	for (i = 0; i < nelems; i++) {
		if (zend_hash_index_find(Z_ARRVAL_P(z_affine), i, (void **) &zval_affine_elem) == SUCCESS) {
			switch (Z_TYPE_PP(zval_affine_elem)) {
				case IS_LONG:
					affine[i]  = Z_LVAL_PP(zval_affine_elem);
					break;
				case IS_DOUBLE:
 					affine[i] = Z_DVAL_PP(zval_affine_elem);
 					break;
 				case IS_STRING:
					{
						zval dval;
						dval = **zval_affine_elem;
						zval_copy_ctor(&dval);
						convert_to_double(&dval);
						affine[i] = Z_DVAL(dval);
					}
 					break;
 				default:
 					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid type for element %i", i);
					RETURN_FALSE;
			}
		}
	}
 
 	if (z_rect != NULL) {
 		if (zend_hash_find(HASH_OF(z_rect), "x", sizeof("x"), (void **)&tmp) != FAILURE) {
			if (Z_TYPE_PP(tmp) != IS_LONG) {
				zval lval;
				lval = **tmp;
				zval_copy_ctor(&lval);
				convert_to_long(&lval);
				rect.x = Z_LVAL(lval);
			} else {
				rect.x = Z_LVAL_PP(tmp);
			}
 		} else {
 			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing x position");
 			RETURN_FALSE;
 		}
 
 		if (zend_hash_find(HASH_OF(z_rect), "y", sizeof("x"), (void **)&tmp) != FAILURE) {
			if (Z_TYPE_PP(tmp) != IS_LONG) {
				zval lval;
				lval = **tmp;
				zval_copy_ctor(&lval);
				convert_to_long(&lval);
				rect.y = Z_LVAL(lval);
			} else {
				rect.y = Z_LVAL_PP(tmp);
			}
 		} else {
 			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing y position");
 			RETURN_FALSE;
 		}
 
 		if (zend_hash_find(HASH_OF(z_rect), "width", sizeof("width"), (void **)&tmp) != FAILURE) {
			if (Z_TYPE_PP(tmp) != IS_LONG) {
				zval lval;
				lval = **tmp;
				zval_copy_ctor(&lval);
				convert_to_long(&lval);
				rect.width = Z_LVAL(lval);
			} else {
				rect.width = Z_LVAL_PP(tmp);
			}
 		} else {
 			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing width");
 			RETURN_FALSE;
 		}
 
 		if (zend_hash_find(HASH_OF(z_rect), "height", sizeof("height"), (void **)&tmp) != FAILURE) {
			if (Z_TYPE_PP(tmp) != IS_LONG) {
				zval lval;
				lval = **tmp;
				zval_copy_ctor(&lval);
				convert_to_long(&lval);
				rect.height = Z_LVAL(lval);
			} else {
				rect.height = Z_LVAL_PP(tmp);
			}
 		} else {
 			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing height");
 			RETURN_FALSE;
		}
		pRect = &rect;
	} else {
		rect.x = -1;
		rect.y = -1;
		rect.width = gdImageSX(src);
		rect.height = gdImageSY(src);
		pRect = NULL;
	}


	if (gdTransformAffineGetImage(&dst, src, pRect, affine) != GD_TRUE) {
		RETURN_FALSE;
	}

	if (dst == NULL) {
		RETURN_FALSE;
	} else {
		ZEND_REGISTER_RESOURCE(return_value, dst, le_gd);
	}
}
