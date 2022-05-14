static void php_snmp_getvalue(struct variable_list *vars, zval *snmpval TSRMLS_DC, int valueretrieval)
{
	zval *val;
	char sbuf[512];
	char *buf = &(sbuf[0]);
        char *dbuf = (char *)NULL;
        int buflen = sizeof(sbuf) - 1;
        int val_len = vars->val_len;
        /* use emalloc() for large values, use static array otherwize */
 
        /* There is no way to know the size of buffer snprint_value() needs in order to print a value there.
	 * So we are forced to probe it
	 */
	while ((valueretrieval & SNMP_VALUE_PLAIN) == 0) {
		*buf = '\0';
		if (snprint_value(buf, buflen, vars->name, vars->name_length, vars) == -1) {
			if (val_len > 512*1024) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "snprint_value() asks for a buffer more than 512k, Net-SNMP bug?");
				break;
			}
			 /* buffer is not long enough to hold full output, double it */
			val_len *= 2;
		} else {
			break;
		}

		if (buf == dbuf) {
			dbuf = (char *)erealloc(dbuf, val_len + 1);
		} else {
			dbuf = (char *)emalloc(val_len + 1);
		}

		if (!dbuf) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "emalloc() failed: %s, fallback to static buffer", strerror(errno));
			buf = &(sbuf[0]);
			buflen = sizeof(sbuf) - 1;
			break;
		}

		buf = dbuf;
		buflen = val_len;
	}

	if((valueretrieval & SNMP_VALUE_PLAIN) && val_len > buflen){
		if ((dbuf = (char *)emalloc(val_len + 1))) {
			buf = dbuf;
			buflen = val_len;
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "emalloc() failed: %s, fallback to static buffer", strerror(errno));
		}
	}

	MAKE_STD_ZVAL(val);

	if (valueretrieval & SNMP_VALUE_PLAIN) {
		*buf = 0;
		switch (vars->type) {
		case ASN_BIT_STR:		/* 0x03, asn1.h */
			ZVAL_STRINGL(val, (char *)vars->val.bitstring, vars->val_len, 1);
			break;

		case ASN_OCTET_STR:		/* 0x04, asn1.h */
		case ASN_OPAQUE:		/* 0x44, snmp_impl.h */
			ZVAL_STRINGL(val, (char *)vars->val.string, vars->val_len, 1);
			break;

		case ASN_NULL:			/* 0x05, asn1.h */
			ZVAL_NULL(val);
			break;

		case ASN_OBJECT_ID:		/* 0x06, asn1.h */
			snprint_objid(buf, buflen, vars->val.objid, vars->val_len / sizeof(oid));
			ZVAL_STRING(val, buf, 1);
			break;

		case ASN_IPADDRESS:		/* 0x40, snmp_impl.h */
			snprintf(buf, buflen, "%d.%d.%d.%d",
				 (vars->val.string)[0], (vars->val.string)[1],
				 (vars->val.string)[2], (vars->val.string)[3]);
			buf[buflen]=0;
			ZVAL_STRING(val, buf, 1);
			break;

		case ASN_COUNTER:		/* 0x41, snmp_impl.h */
		case ASN_GAUGE:			/* 0x42, snmp_impl.h */
		/* ASN_UNSIGNED is the same as ASN_GAUGE */
		case ASN_TIMETICKS:		/* 0x43, snmp_impl.h */
		case ASN_UINTEGER:		/* 0x47, snmp_impl.h */
			snprintf(buf, buflen, "%lu", *vars->val.integer);
			buf[buflen]=0;
			ZVAL_STRING(val, buf, 1);
			break;

		case ASN_INTEGER:		/* 0x02, asn1.h */
			snprintf(buf, buflen, "%ld", *vars->val.integer);
			buf[buflen]=0;
			ZVAL_STRING(val, buf, 1);
			break;

#if defined(NETSNMP_WITH_OPAQUE_SPECIAL_TYPES) || defined(OPAQUE_SPECIAL_TYPES)
		case ASN_OPAQUE_FLOAT:		/* 0x78, asn1.h */
			snprintf(buf, buflen, "%f", *vars->val.floatVal);
			ZVAL_STRING(val, buf, 1);
			break;

		case ASN_OPAQUE_DOUBLE:		/* 0x79, asn1.h */
			snprintf(buf, buflen, "%Lf", *vars->val.doubleVal);
			ZVAL_STRING(val, buf, 1);
			break;

		case ASN_OPAQUE_I64:		/* 0x80, asn1.h */
			printI64(buf, vars->val.counter64);
			ZVAL_STRING(val, buf, 1);
			break;

		case ASN_OPAQUE_U64:		/* 0x81, asn1.h */
#endif
		case ASN_COUNTER64:		/* 0x46, snmp_impl.h */
			printU64(buf, vars->val.counter64);
			ZVAL_STRING(val, buf, 1);
			break;

		default:
			ZVAL_STRING(val, "Unknown value type", 1);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown value type: %u", vars->type);
			break;
		}
	} else /* use Net-SNMP value translation */ {
		/* we have desired string in buffer, just use it */
		ZVAL_STRING(val, buf, 1);
	}

	if (valueretrieval & SNMP_VALUE_OBJECT) {
		object_init(snmpval);
		add_property_long(snmpval, "type", vars->type);
		add_property_zval(snmpval, "value", val);
	} else  {
		*snmpval = *val;
		zval_copy_ctor(snmpval);
	}
	zval_ptr_dtor(&val);

	if(dbuf){ /* malloc was used to store value */
		efree(dbuf);
	}
}
