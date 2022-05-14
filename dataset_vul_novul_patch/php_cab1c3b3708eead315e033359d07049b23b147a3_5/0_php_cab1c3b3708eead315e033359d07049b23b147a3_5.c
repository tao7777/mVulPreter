static void php_snmp_internal(INTERNAL_FUNCTION_PARAMETERS, int st, 
static void php_snmp_internal(INTERNAL_FUNCTION_PARAMETERS, int st,
                                                        struct snmp_session *session,
                                                        struct objid_query *objid_query)
 {
	struct snmp_session *ss;
	struct snmp_pdu *pdu=NULL, *response;
	struct variable_list *vars;
	oid root[MAX_NAME_LEN];
	size_t rootlen = 0;
	int status, count, found;
	char buf[2048];
	char buf2[2048];
	int keepwalking=1;
	char *err;
	zval *snmpval = NULL;
	int snmp_errno;
 
        /* we start with retval=FALSE. If any actual data is acquired, retval will be set to appropriate type */
        RETVAL_FALSE;

        /* reset errno and errstr */
        php_snmp_error(getThis(), NULL TSRMLS_CC, PHP_SNMP_ERRNO_NOERROR, "");
 
	if (st & SNMP_CMD_WALK) { /* remember root OID */
		memmove((char *)root, (char *)(objid_query->vars[0].name), (objid_query->vars[0].name_length) * sizeof(oid));
		rootlen = objid_query->vars[0].name_length;
		objid_query->offset = objid_query->count;
	}

	if ((ss = snmp_open(session)) == NULL) {
		snmp_error(session, NULL, NULL, &err);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not open snmp connection: %s", err);
		free(err);
		RETVAL_FALSE;
		return;
	}

	if ((st & SNMP_CMD_SET) && objid_query->count > objid_query->step) {
		php_snmp_error(getThis(), NULL TSRMLS_CC, PHP_SNMP_ERRNO_MULTIPLE_SET_QUERIES, "Can not fit all OIDs for SET query into one packet, using multiple queries");
	}

	while (keepwalking) {
		keepwalking = 0;
		if (st & SNMP_CMD_WALK) {
			if (session->version == SNMP_VERSION_1) {
				pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
			} else {
				pdu = snmp_pdu_create(SNMP_MSG_GETBULK);
				pdu->non_repeaters = objid_query->non_repeaters;
				pdu->max_repetitions = objid_query->max_repetitions;
			}
			snmp_add_null_var(pdu, objid_query->vars[0].name, objid_query->vars[0].name_length);
		} else {
			if (st & SNMP_CMD_GET) {
				pdu = snmp_pdu_create(SNMP_MSG_GET);
			} else if (st & SNMP_CMD_GETNEXT) {
				pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
			} else if (st & SNMP_CMD_SET) {
				pdu = snmp_pdu_create(SNMP_MSG_SET);
			} else {
				snmp_close(ss);
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unknown SNMP command (internals)");
				RETVAL_FALSE;
				return;
			}
			for (count = 0; objid_query->offset < objid_query->count && count < objid_query->step; objid_query->offset++, count++){
				if (st & SNMP_CMD_SET) {
					if ((snmp_errno = snmp_add_var(pdu, objid_query->vars[objid_query->offset].name, objid_query->vars[objid_query->offset].name_length, objid_query->vars[objid_query->offset].type, objid_query->vars[objid_query->offset].value))) {
						snprint_objid(buf, sizeof(buf), objid_query->vars[objid_query->offset].name, objid_query->vars[objid_query->offset].name_length);
						php_snmp_error(getThis(), NULL TSRMLS_CC, PHP_SNMP_ERRNO_OID_PARSING_ERROR, "Could not add variable: OID='%s' type='%c' value='%s': %s", buf, objid_query->vars[objid_query->offset].type, objid_query->vars[objid_query->offset].value, snmp_api_errstring(snmp_errno));
						snmp_free_pdu(pdu);
						snmp_close(ss);
						RETVAL_FALSE;
						return;
					}
				} else {
					snmp_add_null_var(pdu, objid_query->vars[objid_query->offset].name, objid_query->vars[objid_query->offset].name_length);
				}
			}
			if(pdu->variables == NULL){
				snmp_free_pdu(pdu);
				snmp_close(ss);
				RETVAL_FALSE;
				return;
			}
		}

retry:
		status = snmp_synch_response(ss, pdu, &response);
		if (status == STAT_SUCCESS) {
			if (response->errstat == SNMP_ERR_NOERROR) {
				if (st & SNMP_CMD_SET) {
					if (objid_query->offset < objid_query->count) { /* we have unprocessed OIDs */
						keepwalking = 1;
						continue;
					}
					snmp_free_pdu(response);
					snmp_close(ss);
					RETVAL_TRUE;
					return;
                                }
                                for (vars = response->variables; vars; vars = vars->next_variable) {
                                        /* do not output errors as values */
                                       if (    vars->type == SNMP_ENDOFMIBVIEW ||
                                               vars->type == SNMP_NOSUCHOBJECT ||
                                                vars->type == SNMP_NOSUCHINSTANCE ) {
                                                if ((st & SNMP_CMD_WALK) && Z_TYPE_P(return_value) == IS_ARRAY) {
                                                        break;
						}
						snprint_objid(buf, sizeof(buf), vars->name, vars->name_length);
						snprint_value(buf2, sizeof(buf2), vars->name, vars->name_length, vars);
                                                php_snmp_error(getThis(), NULL TSRMLS_CC, PHP_SNMP_ERRNO_ERROR_IN_REPLY, "Error in packet at '%s': %s", buf, buf2);
                                                continue;
                                        }

                                       if ((st & SNMP_CMD_WALK) &&
                                                (vars->name_length < rootlen || memcmp(root, vars->name, rootlen * sizeof(oid)))) { /* not part of this subtree */
                                                if (Z_TYPE_P(return_value) == IS_ARRAY) { /* some records are fetched already, shut down further lookup */
                                                        keepwalking = 0;
						} else {
							/* first fetched OID is out of subtree, fallback to GET query */
							st |= SNMP_CMD_GET;
							st ^= SNMP_CMD_WALK;
							objid_query->offset = 0;
							keepwalking = 1;
						}
						break;
					}

					MAKE_STD_ZVAL(snmpval);
					php_snmp_getvalue(vars, snmpval TSRMLS_CC, objid_query->valueretrieval);

					if (objid_query->array_output) {
						if (Z_TYPE_P(return_value) == IS_BOOL) {
							array_init(return_value);
						}
						if (st & SNMP_NUMERIC_KEYS) {
							add_next_index_zval(return_value, snmpval);
						} else if (st & SNMP_ORIGINAL_NAMES_AS_KEYS && st & SNMP_CMD_GET) {
							found = 0;
							for (count = 0; count < objid_query->count; count++) {
								if (objid_query->vars[count].name_length == vars->name_length && snmp_oid_compare(objid_query->vars[count].name, objid_query->vars[count].name_length, vars->name, vars->name_length) == 0) {
									found = 1;
									objid_query->vars[count].name_length = 0; /* mark this name as used */
									break;
								}
							}
							if (found) {
								add_assoc_zval(return_value, objid_query->vars[count].oid, snmpval);
							} else {
								snprint_objid(buf2, sizeof(buf2), vars->name, vars->name_length);
								php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not find original OID name for '%s'", buf2);
							}
						} else if (st & SNMP_USE_SUFFIX_AS_KEYS && st & SNMP_CMD_WALK) {
							snprint_objid(buf2, sizeof(buf2), vars->name, vars->name_length);
							if (rootlen <= vars->name_length && snmp_oid_compare(root, rootlen, vars->name, rootlen) == 0) {
								buf2[0] = '\0';
								count = rootlen;
								while(count < vars->name_length){
									sprintf(buf, "%lu.", vars->name[count]);
									strcat(buf2, buf);
									count++;
								}
								buf2[strlen(buf2) - 1] = '\0'; /* remove trailing '.' */
							}
							add_assoc_zval(return_value, buf2, snmpval);
						} else {
							snprint_objid(buf2, sizeof(buf2), vars->name, vars->name_length);
							add_assoc_zval(return_value, buf2, snmpval);
						}
					} else {
						*return_value = *snmpval;
						zval_copy_ctor(return_value);
						zval_ptr_dtor(&snmpval);
						break;
					}

					/* OID increase check */
					if (st & SNMP_CMD_WALK) {
						if (objid_query->oid_increasing_check == TRUE && snmp_oid_compare(objid_query->vars[0].name, objid_query->vars[0].name_length, vars->name, vars->name_length) >= 0) {
							snprint_objid(buf2, sizeof(buf2), vars->name, vars->name_length);
							php_snmp_error(getThis(), NULL TSRMLS_CC, PHP_SNMP_ERRNO_OID_NOT_INCREASING, "Error: OID not increasing: %s", buf2);
							keepwalking = 0;
						} else {
							memmove((char *)(objid_query->vars[0].name), (char *)vars->name, vars->name_length * sizeof(oid));
							objid_query->vars[0].name_length = vars->name_length;
							keepwalking = 1;
						}
					}
				}
				if (objid_query->offset < objid_query->count) { /* we have unprocessed OIDs */
					keepwalking = 1;
				}
			} else {
				if (st & SNMP_CMD_WALK && response->errstat == SNMP_ERR_TOOBIG && objid_query->max_repetitions > 1) { /* Answer will not fit into single packet */
					objid_query->max_repetitions /= 2;
					snmp_free_pdu(response);
					keepwalking = 1;
					continue;
				}
				if (!(st & SNMP_CMD_WALK) || response->errstat != SNMP_ERR_NOSUCHNAME || Z_TYPE_P(return_value) == IS_BOOL) {
					for (	count=1, vars = response->variables;
						vars && count != response->errindex;
						vars = vars->next_variable, count++);

					if (st & (SNMP_CMD_GET | SNMP_CMD_GETNEXT) && response->errstat == SNMP_ERR_TOOBIG && objid_query->step > 1) { /* Answer will not fit into single packet */
						objid_query->offset = ((objid_query->offset > objid_query->step) ? (objid_query->offset - objid_query->step) : 0 );
						objid_query->step /= 2;
						snmp_free_pdu(response);
						keepwalking = 1;
						continue;
					}
					if (vars) {
						snprint_objid(buf, sizeof(buf), vars->name, vars->name_length);
						php_snmp_error(getThis(), NULL TSRMLS_CC, PHP_SNMP_ERRNO_ERROR_IN_REPLY, "Error in packet at '%s': %s", buf, snmp_errstring(response->errstat));
					} else {
						php_snmp_error(getThis(), NULL TSRMLS_CC, PHP_SNMP_ERRNO_ERROR_IN_REPLY, "Error in packet at %u object_id: %s", response->errindex, snmp_errstring(response->errstat));
					}
					if (st & (SNMP_CMD_GET | SNMP_CMD_GETNEXT)) { /* cut out bogus OID and retry */
						if ((pdu = snmp_fix_pdu(response, ((st & SNMP_CMD_GET) ? SNMP_MSG_GET : SNMP_MSG_GETNEXT) )) != NULL) {
							snmp_free_pdu(response);
							goto retry;
						}
					}
					snmp_free_pdu(response);
					snmp_close(ss);
					if (objid_query->array_output) {
						zval_dtor(return_value);
					}
					RETVAL_FALSE;
					return;
				}
			}
		} else if (status == STAT_TIMEOUT) {
			php_snmp_error(getThis(), NULL TSRMLS_CC, PHP_SNMP_ERRNO_TIMEOUT, "No response from %s", session->peername);
			if (objid_query->array_output) {
				zval_dtor(return_value);
			}
			snmp_close(ss);
			RETVAL_FALSE;
			return;
		} else {    /* status == STAT_ERROR */
			snmp_error(ss, NULL, NULL, &err);
			php_snmp_error(getThis(), NULL TSRMLS_CC, PHP_SNMP_ERRNO_GENERIC, "Fatal error: %s", err);
			free(err);
			if (objid_query->array_output) {
				zval_dtor(return_value);
			}
			snmp_close(ss);
			RETVAL_FALSE;
			return;
		}
		if (response) {
			snmp_free_pdu(response);
		}
	} /* keepwalking */
	snmp_close(ss);
}
