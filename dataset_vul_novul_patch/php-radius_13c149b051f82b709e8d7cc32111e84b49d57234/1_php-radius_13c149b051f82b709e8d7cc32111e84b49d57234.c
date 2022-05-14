 PHP_FUNCTION(radius_get_vendor_attr)
 {
	int res;
	const void *data;
 	int len;
 	u_int32_t vendor;
 
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &data, &len) == FAILURE) {
 		return;
 	}
 
	res = rad_get_vendor_attr(&vendor, &data, (size_t *) &len);
	if (res == -1) {
 		RETURN_FALSE;
 	} else {
 
 		array_init(return_value);
		add_assoc_long(return_value, "attr", res);
 		add_assoc_long(return_value, "vendor", vendor);
		add_assoc_stringl(return_value, "data", (char *) data, len, 1);
 		return;
 	}
 }
