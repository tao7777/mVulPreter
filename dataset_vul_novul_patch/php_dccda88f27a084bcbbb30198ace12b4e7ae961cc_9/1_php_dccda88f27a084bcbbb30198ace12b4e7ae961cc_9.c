static void php_xml_parser_create_impl(INTERNAL_FUNCTION_PARAMETERS, int ns_support) /* {{{ */
{
	xml_parser *parser;
	int auto_detect = 0;

	char *encoding_param = NULL;
	int encoding_param_len = 0;
 
        char *ns_param = NULL;
        int ns_param_len = 0;
        XML_Char *encoding;
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, (ns_support ? "|ss": "|s"), &encoding_param, &encoding_param_len, &ns_param, &ns_param_len) == FAILURE) {
                RETURN_FALSE;
        }

	if (encoding_param != NULL) {
		/* The supported encoding types are hardcoded here because
		 * we are limited to the encodings supported by expat/xmltok.
		 */
		if (encoding_param_len == 0) {
			encoding = XML(default_encoding);
			auto_detect = 1;
		} else if (strcasecmp(encoding_param, "ISO-8859-1") == 0) {
			encoding = "ISO-8859-1";
		} else if (strcasecmp(encoding_param, "UTF-8") == 0) {
			encoding = "UTF-8";
		} else if (strcasecmp(encoding_param, "US-ASCII") == 0) {
			encoding = "US-ASCII";
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "unsupported source encoding \"%s\"", encoding_param);
			RETURN_FALSE;
		}
	} else {
		encoding = XML(default_encoding);
	}

	if (ns_support && ns_param == NULL){
		ns_param = ":";
	}

	parser = ecalloc(1, sizeof(xml_parser));
	parser->parser = XML_ParserCreate_MM((auto_detect ? NULL : encoding),
                                         &php_xml_mem_hdlrs, ns_param);

	parser->target_encoding = encoding;
	parser->case_folding = 1;
	parser->object = NULL;
	parser->isparsing = 0;

	XML_SetUserData(parser->parser, parser);

	ZEND_REGISTER_RESOURCE(return_value, parser,le_xml_parser);
	parser->index = Z_LVAL_P(return_value);
 }
 /* }}} */
