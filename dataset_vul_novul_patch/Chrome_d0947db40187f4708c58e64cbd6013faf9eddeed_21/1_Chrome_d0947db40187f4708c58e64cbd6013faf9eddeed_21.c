xmlParseExtParsedEnt(xmlParserCtxtPtr ctxt) {
    xmlChar start[4];
    xmlCharEncoding enc;

    if ((ctxt == NULL) || (ctxt->input == NULL))
        return(-1);

    xmlDefaultSAXHandlerInit();

    xmlDetectSAX2(ctxt);

    GROW;

    /*
     * SAX: beginning of the document processing.
     */
    if ((ctxt->sax) && (ctxt->sax->setDocumentLocator))
        ctxt->sax->setDocumentLocator(ctxt->userData, &xmlDefaultSAXLocator);

    /* 
     * Get the 4 first bytes and decode the charset
     * if enc != XML_CHAR_ENCODING_NONE
     * plug some encoding conversion routines.
     */
    if ((ctxt->input->end - ctxt->input->cur) >= 4) {
	start[0] = RAW;
	start[1] = NXT(1);
	start[2] = NXT(2);
	start[3] = NXT(3);
	enc = xmlDetectCharEncoding(start, 4);
	if (enc != XML_CHAR_ENCODING_NONE) {
	    xmlSwitchEncoding(ctxt, enc);
	}
    }


    if (CUR == 0) {
	xmlFatalErr(ctxt, XML_ERR_DOCUMENT_EMPTY, NULL);
    }

    /*
     * Check for the XMLDecl in the Prolog.
     */
    GROW;
    if ((CMP5(CUR_PTR, '<', '?', 'x', 'm', 'l')) && (IS_BLANK_CH(NXT(5)))) {

	/*
	 * Note that we will switch encoding on the fly.
	 */
	xmlParseXMLDecl(ctxt);
	if (ctxt->errNo == XML_ERR_UNSUPPORTED_ENCODING) {
	    /*
	     * The XML REC instructs us to stop parsing right here
	     */
	    return(-1);
	}
	SKIP_BLANKS;
    } else {
	ctxt->version = xmlCharStrdup(XML_DEFAULT_VERSION);
     }
     if ((ctxt->sax) && (ctxt->sax->startDocument) && (!ctxt->disableSAX))
         ctxt->sax->startDocument(ctxt->userData);
 
     /*
      * Doing validity checking on chunk doesn't make sense
     */
    ctxt->instate = XML_PARSER_CONTENT;
    ctxt->validate = 0;
    ctxt->loadsubset = 0;
     ctxt->depth = 0;
 
     xmlParseContent(ctxt);
    
     if ((RAW == '<') && (NXT(1) == '/')) {
 	xmlFatalErr(ctxt, XML_ERR_NOT_WELL_BALANCED, NULL);
    } else if (RAW != 0) {
	xmlFatalErr(ctxt, XML_ERR_EXTRA_CONTENT, NULL);
    }

    /*
     * SAX: end of the document processing.
     */
    if ((ctxt->sax) && (ctxt->sax->endDocument != NULL))
        ctxt->sax->endDocument(ctxt->userData);

    if (! ctxt->wellFormed) return(-1);
    return(0);
}
