PHP_FUNCTION(imagecrop)
{
	zval *IM;
	gdImagePtr im;
	gdImagePtr im_crop;
	gdRect rect;
	zval *z_rect;
	zval **tmp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &IM, &z_rect) == FAILURE)  {
		return;
	}

 	ZEND_FETCH_RESOURCE(im, gdImagePtr, &IM, -1, "Image", le_gd);
 
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

	im_crop = gdImageCrop(im, &rect);

	if (im_crop == NULL) {
		RETURN_FALSE;
	} else {
		ZEND_REGISTER_RESOURCE(return_value, im_crop, le_gd);
	}
}
