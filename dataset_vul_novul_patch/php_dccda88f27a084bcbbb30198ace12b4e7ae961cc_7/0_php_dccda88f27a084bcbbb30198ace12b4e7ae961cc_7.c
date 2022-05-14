void _xml_unparsedEntityDeclHandler(void *userData, 
void _xml_unparsedEntityDeclHandler(void *userData,
                                                                                const XML_Char *entityName,
                                                                                 const XML_Char *base,
                                                                                 const XML_Char *systemId,
                                                                                 const XML_Char *publicId,
										 const XML_Char *notationName)
{
	xml_parser *parser = (xml_parser *)userData;

	if (parser && parser->unparsedEntityDeclHandler) {
		zval *retval, *args[6];

		args[0] = _xml_resource_zval(parser->index);
		args[1] = _xml_xmlchar_zval(entityName, 0, parser->target_encoding);
		args[2] = _xml_xmlchar_zval(base, 0, parser->target_encoding);
		args[3] = _xml_xmlchar_zval(systemId, 0, parser->target_encoding);
		args[4] = _xml_xmlchar_zval(publicId, 0, parser->target_encoding);
		args[5] = _xml_xmlchar_zval(notationName, 0, parser->target_encoding);
		if ((retval = xml_call_handler(parser, parser->unparsedEntityDeclHandler, parser->unparsedEntityDeclPtr, 6, args))) {
			zval_ptr_dtor(&retval);
		}
	}
}
