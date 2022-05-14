static int php_snmp_parse_oid(zval *object, int st, struct objid_query *objid_query, zval **oid, zval **type, zval **value TSRMLS_DC)
{
	char *pptr;
	HashPosition pos_oid, pos_type, pos_value;
	zval **tmp_oid, **tmp_type, **tmp_value;

	if (Z_TYPE_PP(oid) != IS_ARRAY) {
		if (Z_ISREF_PP(oid)) {
			SEPARATE_ZVAL(oid);
		}
		convert_to_string_ex(oid);
	} else if (Z_TYPE_PP(oid) == IS_ARRAY) {
		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(oid), &pos_oid);
	}

	if (st & SNMP_CMD_SET) {
		if (Z_TYPE_PP(type) != IS_ARRAY) {
			if (Z_ISREF_PP(type)) {
				SEPARATE_ZVAL(type);
			}
			convert_to_string_ex(type);
		} else if (Z_TYPE_PP(type) == IS_ARRAY) {
			zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(type), &pos_type);
		}

		if (Z_TYPE_PP(value) != IS_ARRAY) {
			if (Z_ISREF_PP(value)) {
				SEPARATE_ZVAL(value);
			}
			convert_to_string_ex(value);
		} else if (Z_TYPE_PP(value) == IS_ARRAY) {
			zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(value), &pos_value);
		}
	}

	objid_query->count = 0;
	objid_query->array_output = ((st & SNMP_CMD_WALK) ? TRUE : FALSE);
	if (Z_TYPE_PP(oid) == IS_STRING) {
		objid_query->vars = (snmpobjarg *)emalloc(sizeof(snmpobjarg));
		if (objid_query->vars == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "emalloc() failed while parsing oid: %s", strerror(errno));
			efree(objid_query->vars);
			return FALSE;
		}
		objid_query->vars[objid_query->count].oid = Z_STRVAL_PP(oid);
		if (st & SNMP_CMD_SET) {
			if (Z_TYPE_PP(type) == IS_STRING && Z_TYPE_PP(value) == IS_STRING) {
				if (Z_STRLEN_PP(type) != 1) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bogus type '%s', should be single char, got %u", Z_STRVAL_PP(type), Z_STRLEN_PP(type));
					efree(objid_query->vars);
					return FALSE;
				}
				pptr = Z_STRVAL_PP(type);
				objid_query->vars[objid_query->count].type = *pptr;
				objid_query->vars[objid_query->count].value = Z_STRVAL_PP(value);
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Single objid and multiple type or values are not supported");
				efree(objid_query->vars);
				return FALSE;
			}
		}
		objid_query->count++;
	} else if (Z_TYPE_PP(oid) == IS_ARRAY) { /* we got objid array */
		if (zend_hash_num_elements(Z_ARRVAL_PP(oid)) == 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Got empty OID array");
			return FALSE;
		}
		objid_query->vars = (snmpobjarg *)emalloc(sizeof(snmpobjarg) * zend_hash_num_elements(Z_ARRVAL_PP(oid)));
		if (objid_query->vars == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "emalloc() failed while parsing oid array: %s", strerror(errno));
			efree(objid_query->vars);
			return FALSE;
		}
		objid_query->array_output = ( (st & SNMP_CMD_SET) ? FALSE : TRUE );
		for (	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(oid), &pos_oid);
			zend_hash_get_current_data_ex(Z_ARRVAL_PP(oid), (void **) &tmp_oid, &pos_oid) == SUCCESS;
			zend_hash_move_forward_ex(Z_ARRVAL_PP(oid), &pos_oid) ) {

			convert_to_string_ex(tmp_oid);
			objid_query->vars[objid_query->count].oid = Z_STRVAL_PP(tmp_oid);
			if (st & SNMP_CMD_SET) {
				if (Z_TYPE_PP(type) == IS_STRING) {
					pptr = Z_STRVAL_PP(type);
					objid_query->vars[objid_query->count].type = *pptr;
				} else if (Z_TYPE_PP(type) == IS_ARRAY) {
					if (SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_PP(type), (void **) &tmp_type, &pos_type)) {
						convert_to_string_ex(tmp_type);
						if (Z_STRLEN_PP(tmp_type) != 1) {
							php_error_docref(NULL TSRMLS_CC, E_WARNING, "'%s': bogus type '%s', should be single char, got %u", Z_STRVAL_PP(tmp_oid), Z_STRVAL_PP(tmp_type), Z_STRLEN_PP(tmp_type));
							efree(objid_query->vars);
							return FALSE;
						}
						pptr = Z_STRVAL_PP(tmp_type);
						objid_query->vars[objid_query->count].type = *pptr;
						zend_hash_move_forward_ex(Z_ARRVAL_PP(type), &pos_type);
					} else {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "'%s': no type set", Z_STRVAL_PP(tmp_oid));
						efree(objid_query->vars);
						return FALSE;
					}
				}

				if (Z_TYPE_PP(value) == IS_STRING) {
					objid_query->vars[objid_query->count].value = Z_STRVAL_PP(value);
				} else if (Z_TYPE_PP(value) == IS_ARRAY) {
					if (SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_PP(value), (void **) &tmp_value, &pos_value)) {
						convert_to_string_ex(tmp_value);
						objid_query->vars[objid_query->count].value = Z_STRVAL_PP(tmp_value);
						zend_hash_move_forward_ex(Z_ARRVAL_PP(value), &pos_value);
					} else {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "'%s': no value set", Z_STRVAL_PP(tmp_oid));
						efree(objid_query->vars);
						return FALSE;
					}
				}
			}
			objid_query->count++;
		}
	}

	/* now parse all OIDs */
	if (st & SNMP_CMD_WALK) {
		if (objid_query->count > 1) {
			php_snmp_error(object, NULL TSRMLS_CC, PHP_SNMP_ERRNO_OID_PARSING_ERROR, "Multi OID walks are not supported!");
			efree(objid_query->vars);
			return FALSE;
		}
		objid_query->vars[0].name_length = MAX_NAME_LEN;
		if (strlen(objid_query->vars[0].oid)) { /* on a walk, an empty string means top of tree - no error */
			if (!snmp_parse_oid(objid_query->vars[0].oid, objid_query->vars[0].name, &(objid_query->vars[0].name_length))) {
				php_snmp_error(object, NULL TSRMLS_CC, PHP_SNMP_ERRNO_OID_PARSING_ERROR, "Invalid object identifier: %s", objid_query->vars[0].oid);
                                efree(objid_query->vars);
                                return FALSE;
                        }
               } else {
                        memmove((char *)objid_query->vars[0].name, (char *)objid_mib, sizeof(objid_mib));
                        objid_query->vars[0].name_length = sizeof(objid_mib) / sizeof(oid);
                }
	} else {
		for (objid_query->offset = 0; objid_query->offset < objid_query->count; objid_query->offset++) {
			objid_query->vars[objid_query->offset].name_length = MAX_OID_LEN;
			if (!snmp_parse_oid(objid_query->vars[objid_query->offset].oid, objid_query->vars[objid_query->offset].name, &(objid_query->vars[objid_query->offset].name_length))) {
				php_snmp_error(object, NULL TSRMLS_CC, PHP_SNMP_ERRNO_OID_PARSING_ERROR, "Invalid object identifier: %s", objid_query->vars[objid_query->offset].oid);
				efree(objid_query->vars);
				return FALSE;
			}
		}
	}
	objid_query->offset = 0;
	objid_query->step = objid_query->count;
	return (objid_query->count > 0);
}
