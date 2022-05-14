static void php_snmp(INTERNAL_FUNCTION_PARAMETERS, int st, int version)
{
	zval **oid, **value, **type;
	char *a1, *a2, *a3, *a4, *a5, *a6, *a7;
	int a1_len, a2_len, a3_len, a4_len, a5_len, a6_len, a7_len;
	zend_bool use_orignames = 0, suffix_keys = 0;
	long timeout = SNMP_DEFAULT_TIMEOUT;
	long retries = SNMP_DEFAULT_RETRIES;
	int argc = ZEND_NUM_ARGS();
	struct objid_query objid_query;
	php_snmp_session *session;
        int session_less_mode = (getThis() == NULL);
        php_snmp_object *snmp_object;
        php_snmp_object glob_snmp_object;
        objid_query.max_repetitions = -1;
        objid_query.non_repeaters = 0;
        objid_query.valueretrieval = SNMP_G(valueretrieval);
	objid_query.oid_increasing_check = TRUE;

	if (session_less_mode) {
		if (version == SNMP_VERSION_3) {
			if (st & SNMP_CMD_SET) {
				if (zend_parse_parameters(argc TSRMLS_CC, "sssssssZZZ|ll", &a1, &a1_len, &a2, &a2_len, &a3, &a3_len,
					&a4, &a4_len, &a5, &a5_len, &a6, &a6_len, &a7, &a7_len, &oid, &type, &value, &timeout, &retries) == FAILURE) {
					RETURN_FALSE;
				}
			} else {
				/* SNMP_CMD_GET
				 * SNMP_CMD_GETNEXT
				 * SNMP_CMD_WALK
				 */
				if (zend_parse_parameters(argc TSRMLS_CC, "sssssssZ|ll", &a1, &a1_len, &a2, &a2_len, &a3, &a3_len,
					&a4, &a4_len, &a5, &a5_len, &a6, &a6_len, &a7, &a7_len, &oid, &timeout, &retries) == FAILURE) {
					RETURN_FALSE;
				}
			}
		} else {
			if (st & SNMP_CMD_SET) {
				if (zend_parse_parameters(argc TSRMLS_CC, "ssZZZ|ll", &a1, &a1_len, &a2, &a2_len, &oid, &type, &value, &timeout, &retries) == FAILURE) {
					RETURN_FALSE;
				}
			} else {
				/* SNMP_CMD_GET
				 * SNMP_CMD_GETNEXT
				 * SNMP_CMD_WALK
				 */
				if (zend_parse_parameters(argc TSRMLS_CC, "ssZ|ll", &a1, &a1_len, &a2, &a2_len, &oid, &timeout, &retries) == FAILURE) {
					RETURN_FALSE;
				}
			}
		}
	} else {
		if (st & SNMP_CMD_SET) {
			if (zend_parse_parameters(argc TSRMLS_CC, "ZZZ", &oid, &type, &value) == FAILURE) {
				RETURN_FALSE;
			}
		} else if (st & SNMP_CMD_WALK) {
			if (zend_parse_parameters(argc TSRMLS_CC, "Z|bll", &oid, &suffix_keys, &(objid_query.max_repetitions), &(objid_query.non_repeaters)) == FAILURE) {
				RETURN_FALSE;
			}
			if (suffix_keys) {
				st |= SNMP_USE_SUFFIX_AS_KEYS;
			}
		} else if (st & SNMP_CMD_GET) {
			if (zend_parse_parameters(argc TSRMLS_CC, "Z|b", &oid, &use_orignames) == FAILURE) {
				RETURN_FALSE;
			}
			if (use_orignames) {
				st |= SNMP_ORIGINAL_NAMES_AS_KEYS;
			}
		} else {
			/* SNMP_CMD_GETNEXT
			 */
			if (zend_parse_parameters(argc TSRMLS_CC, "Z", &oid) == FAILURE) {
				RETURN_FALSE;
			}
		}
	}

	if (!php_snmp_parse_oid(getThis(), st, &objid_query, oid, type, value TSRMLS_CC)) {
		RETURN_FALSE;
	}

	if (session_less_mode) {
		if (netsnmp_session_init(&session, version, a1, a2, timeout, retries TSRMLS_CC)) {
			efree(objid_query.vars);
			netsnmp_session_free(&session);
			RETURN_FALSE;
		}
		if (version == SNMP_VERSION_3 && netsnmp_session_set_security(session, a3, a4, a5, a6, a7, NULL, NULL TSRMLS_CC)) {
			efree(objid_query.vars);
			netsnmp_session_free(&session);
			/* Warning message sent already, just bail out */
			RETURN_FALSE;
		}
	} else {
		zval *object = getThis();
		snmp_object = (php_snmp_object *)zend_object_store_get_object(object TSRMLS_CC);
		session = snmp_object->session;
		if (!session) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid or uninitialized SNMP object");
			efree(objid_query.vars);
			RETURN_FALSE;
		}

		if (snmp_object->max_oids > 0) {
			objid_query.step = snmp_object->max_oids;
			if (objid_query.max_repetitions < 0) { /* unspecified in function call, use session-wise */
				objid_query.max_repetitions = snmp_object->max_oids;
			}
		}
		objid_query.oid_increasing_check = snmp_object->oid_increasing_check;
		objid_query.valueretrieval = snmp_object->valueretrieval;
		glob_snmp_object.enum_print = netsnmp_ds_get_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_PRINT_NUMERIC_ENUM);
		netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_PRINT_NUMERIC_ENUM, snmp_object->enum_print);
		glob_snmp_object.quick_print = netsnmp_ds_get_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_QUICK_PRINT);
		netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_QUICK_PRINT, snmp_object->quick_print);
		glob_snmp_object.oid_output_format = netsnmp_ds_get_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT);
		netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT, snmp_object->oid_output_format);
	}

	if (objid_query.max_repetitions < 0) {
		objid_query.max_repetitions = 20; /* provide correct default value */
        }
 
        php_snmp_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU, st, session, &objid_query);
        efree(objid_query.vars);
 
        if (session_less_mode) {
		netsnmp_session_free(&session);
	} else {
		netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_PRINT_NUMERIC_ENUM, glob_snmp_object.enum_print);
		netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_QUICK_PRINT, glob_snmp_object.quick_print);
		netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT, glob_snmp_object.oid_output_format);
	}
 }
