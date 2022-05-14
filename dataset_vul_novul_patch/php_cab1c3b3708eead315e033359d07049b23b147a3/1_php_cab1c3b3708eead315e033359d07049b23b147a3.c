PHP_FUNCTION(snmp_set_enum_print)
{
	long a1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &a1) == FAILURE) {
		RETURN_FALSE;
	}
 
        netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_PRINT_NUMERIC_ENUM, (int) a1);
        RETURN_TRUE;
} 
