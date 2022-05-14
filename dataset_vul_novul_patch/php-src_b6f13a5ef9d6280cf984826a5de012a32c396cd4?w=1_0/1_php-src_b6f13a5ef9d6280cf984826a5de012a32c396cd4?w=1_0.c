PHP_FUNCTION(imageaffinematrixget)
{
	double affine[6];
	long type;
	zval *options = NULL;
	zval **tmp;
	int res = GD_FALSE, i;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|z", &type, &options) == FAILURE)  {
		return;
	}

	switch((gdAffineStandardMatrix)type) {
		case GD_AFFINE_TRANSLATE:
		case GD_AFFINE_SCALE: {
			double x, y;
			if (!options || Z_TYPE_P(options) != IS_ARRAY) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Array expected as options");
				RETURN_FALSE;
			}
			if (zend_hash_find(HASH_OF(options), "x", sizeof("x"), (void **)&tmp) != FAILURE) {
				if (Z_TYPE_PP(tmp) != IS_DOUBLE) {
					zval dval;
					dval = **tmp;
					zval_copy_ctor(&dval);
					convert_to_double(&dval);
					x = Z_DVAL(dval);
				} else {
					x = Z_DVAL_PP(tmp);
				}
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing x position");
				RETURN_FALSE;
			}

			if (zend_hash_find(HASH_OF(options), "y", sizeof("y"), (void **)&tmp) != FAILURE) {
				if (Z_TYPE_PP(tmp) != IS_DOUBLE) {
					zval dval;
					dval = **tmp;
					zval_copy_ctor(&dval);
					convert_to_double(&dval);
					y = Z_DVAL(dval);
				} else {
					y = Z_DVAL_PP(tmp);
				}
			} else {
 				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing y position");
 				RETURN_FALSE;
 			}
 			if (type == GD_AFFINE_TRANSLATE) {
 				res = gdAffineTranslate(affine, x, y);
 			} else {
				res = gdAffineScale(affine, x, y);
			}
			break;
		}

		case GD_AFFINE_ROTATE:
		case GD_AFFINE_SHEAR_HORIZONTAL:
		case GD_AFFINE_SHEAR_VERTICAL: {
			double angle;

			if (!options) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number is expected as option");
				RETURN_FALSE;
			}
			if(Z_TYPE_P(options) != IS_DOUBLE) {
				zval dval;
				dval = *options;
				zval_copy_ctor(&dval);
				convert_to_double(&dval);
				angle = Z_DVAL(dval);
			} else {
				angle = Z_DVAL_P(options);
			}

			if (type == GD_AFFINE_SHEAR_HORIZONTAL) {
				res = gdAffineShearHorizontal(affine, angle);
			} else if (type == GD_AFFINE_SHEAR_VERTICAL) {
				res = gdAffineShearVertical(affine, angle);
			} else {
				res = gdAffineRotate(affine, angle);
			}
			break;
		}

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid type for element %li", type);
			RETURN_FALSE;
	}

	if (res == GD_FALSE) {
		RETURN_FALSE;
	} else {
		array_init(return_value);
		for (i = 0; i < 6; i++) {
			add_index_double(return_value, i, affine[i]);
		}
	}
}
