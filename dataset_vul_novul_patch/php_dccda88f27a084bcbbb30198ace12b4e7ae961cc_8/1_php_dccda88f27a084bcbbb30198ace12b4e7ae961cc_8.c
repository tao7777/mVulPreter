static zval *_xml_xmlchar_zval(const XML_Char *s, int len, const XML_Char *encoding)
 {
        zval *ret;
        MAKE_STD_ZVAL(ret);
        if (s == NULL) {
                ZVAL_FALSE(ret);
                return ret;
	}
	if (len == 0) {
		len = _xml_xmlcharlen(s);
	}
	Z_TYPE_P(ret) = IS_STRING;
	Z_STRVAL_P(ret) = xml_utf8_decode(s, len, &Z_STRLEN_P(ret), encoding);
	return ret;
}
