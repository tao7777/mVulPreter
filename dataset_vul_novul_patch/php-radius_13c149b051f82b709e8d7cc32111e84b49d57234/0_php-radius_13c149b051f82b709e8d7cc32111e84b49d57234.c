 PHP_FUNCTION(radius_get_vendor_attr)
 {
	const void *data, *raw;
 	int len;
 	u_int32_t vendor;
	unsigned char type;
	size_t data_len;
 
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &raw, &len) == FAILURE) {
 		return;
 	}
 
	if (rad_get_vendor_attr(&vendor, &type, &data, &data_len, raw, len) == -1) {
 		RETURN_FALSE;
 	} else {
 
 		array_init(return_value);
		add_assoc_long(return_value, "attr", type);
 		add_assoc_long(return_value, "vendor", vendor);
		add_assoc_stringl(return_value, "data", (char *) data, data_len, 1);
 		return;
 	}
 }
