xmlParseExternalSubset(xmlParserCtxtPtr ctxt, const xmlChar *ExternalID,
                       const xmlChar *SystemID) {
    xmlDetectSAX2(ctxt);
    GROW;

    if ((ctxt->encoding == (const xmlChar *)XML_CHAR_ENCODING_NONE) &&
        (ctxt->input->end - ctxt->input->cur >= 4)) {
        xmlChar start[4];
	xmlCharEncoding enc;

	start[0] = RAW;
	start[1] = NXT(1);
	start[2] = NXT(2);
	start[3] = NXT(3);
	enc = xmlDetectCharEncoding(start, 4);
	if (enc != XML_CHAR_ENCODING_NONE)
	    xmlSwitchEncoding(ctxt, enc);
    }

    if (CMP5(CUR_PTR, '<', '?', 'x', 'm', 'l')) {
	xmlParseTextDecl(ctxt);
	if (ctxt->errNo == XML_ERR_UNSUPPORTED_ENCODING) {
	    /*
	     * The XML REC instructs us to stop parsing right here
	     */
	    ctxt->instate = XML_PARSER_EOF;
	    return;
	}
    }
    if (ctxt->myDoc == NULL) {
        ctxt->myDoc = xmlNewDoc(BAD_CAST "1.0");
	if (ctxt->myDoc == NULL) {
	    xmlErrMemory(ctxt, "New Doc failed");
	    return;
	}
	ctxt->myDoc->properties = XML_DOC_INTERNAL;
    }
    if ((ctxt->myDoc != NULL) && (ctxt->myDoc->intSubset == NULL))
        xmlCreateIntSubset(ctxt->myDoc, NULL, ExternalID, SystemID);

    ctxt->instate = XML_PARSER_DTD;
    ctxt->external = 1;
    while (((RAW == '<') && (NXT(1) == '?')) ||
           ((RAW == '<') && (NXT(1) == '!')) ||
	   (RAW == '%') || IS_BLANK_CH(CUR)) {
	const xmlChar *check = CUR_PTR;
	unsigned int cons = ctxt->input->consumed;

	GROW;
        if ((RAW == '<') && (NXT(1) == '!') && (NXT(2) == '[')) {
	    xmlParseConditionalSections(ctxt);
	} else if (IS_BLANK_CH(CUR)) {
	    NEXT;
	} else if (RAW == '%') {
            xmlParsePEReference(ctxt);
	} else
	    xmlParseMarkupDecl(ctxt);

	/*
	 * Pop-up of finished entities.
	 */
	while ((RAW == 0) && (ctxt->inputNr > 1))
	    xmlPopInput(ctxt);

	if ((CUR_PTR == check) && (cons == ctxt->input->consumed)) {
	    xmlFatalErr(ctxt, XML_ERR_EXT_SUBSET_NOT_FINISHED, NULL);
 	    break;
 	}
     }
     if (RAW != 0) {
 	xmlFatalErr(ctxt, XML_ERR_EXT_SUBSET_NOT_FINISHED, NULL);
     }

}
