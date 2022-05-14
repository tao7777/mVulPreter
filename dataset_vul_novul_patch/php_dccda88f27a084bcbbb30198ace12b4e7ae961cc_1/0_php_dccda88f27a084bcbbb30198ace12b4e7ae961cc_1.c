PHP_FUNCTION(xml_parse_into_struct)
{
	xml_parser *parser;
	zval *pind, **xdata, **info = NULL;
	char *data;
	int data_len, ret;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsZ|Z", &pind, &data, &data_len, &xdata, &info) == FAILURE) {
                return;
        }

       if (info) {
                zval_dtor(*info);
                array_init(*info);
        }

	ZEND_FETCH_RESOURCE(parser,xml_parser *, &pind, -1, "XML Parser", le_xml_parser);

	zval_dtor(*xdata);
        array_init(*xdata);
 
        parser->data = *xdata;

        if (info) {
                parser->info = *info;
        }

        parser->level = 0;
        parser->ltags = safe_emalloc(XML_MAXLEVEL, sizeof(char *), 0);
 
	XML_SetDefaultHandler(parser->parser, _xml_defaultHandler);
	XML_SetElementHandler(parser->parser, _xml_startElementHandler, _xml_endElementHandler);
	XML_SetCharacterDataHandler(parser->parser, _xml_characterDataHandler);

	parser->isparsing = 1;
	ret = XML_Parse(parser->parser, data, data_len, 1);
	parser->isparsing = 0;

	RETVAL_LONG(ret);
 }
