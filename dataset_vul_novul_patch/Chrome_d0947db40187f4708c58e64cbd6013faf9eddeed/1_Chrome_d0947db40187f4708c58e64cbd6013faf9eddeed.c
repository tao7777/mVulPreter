xmlLoadEntityContent(xmlParserCtxtPtr ctxt, xmlEntityPtr entity) {
    xmlParserInputPtr input;
    xmlBufferPtr buf;
    int l, c;
    int count = 0;

    if ((ctxt == NULL) || (entity == NULL) ||
        ((entity->etype != XML_EXTERNAL_PARAMETER_ENTITY) &&
	 (entity->etype != XML_EXTERNAL_GENERAL_PARSED_ENTITY)) ||
	(entity->content != NULL)) {
	xmlFatalErr(ctxt, XML_ERR_INTERNAL_ERROR,
	            "xmlLoadEntityContent parameter error");
        return(-1);
    }

    if (xmlParserDebugEntities)
	xmlGenericError(xmlGenericErrorContext,
		"Reading %s entity content input\n", entity->name);

    buf = xmlBufferCreate();
    if (buf == NULL) {
	xmlFatalErr(ctxt, XML_ERR_INTERNAL_ERROR,
	            "xmlLoadEntityContent parameter error");
        return(-1);
    }

    input = xmlNewEntityInputStream(ctxt, entity);
    if (input == NULL) {
	xmlFatalErr(ctxt, XML_ERR_INTERNAL_ERROR,
	            "xmlLoadEntityContent input error");
	xmlBufferFree(buf);
        return(-1);
    }

    /*
     * Push the entity as the current input, read char by char
     * saving to the buffer until the end of the entity or an error
     */
    if (xmlPushInput(ctxt, input) < 0) {
        xmlBufferFree(buf);
	return(-1);
    }

    GROW;
    c = CUR_CHAR(l);
    while ((ctxt->input == input) && (ctxt->input->cur < ctxt->input->end) &&
           (IS_CHAR(c))) {
        xmlBufferAdd(buf, ctxt->input->cur, l);
 	if (count++ > 100) {
 	    count = 0;
 	    GROW;
 	}
 	NEXTL(l);
 	c = CUR_CHAR(l);
    }

    if ((ctxt->input == input) && (ctxt->input->cur >= ctxt->input->end)) {
        xmlPopInput(ctxt);
    } else if (!IS_CHAR(c)) {
        xmlFatalErrMsgInt(ctxt, XML_ERR_INVALID_CHAR,
                          "xmlLoadEntityContent: invalid char value %d\n",
	                  c);
	xmlBufferFree(buf);
	return(-1);
    }
    entity->content = buf->content;
    buf->content = NULL;
    xmlBufferFree(buf);

    return(0);
}
