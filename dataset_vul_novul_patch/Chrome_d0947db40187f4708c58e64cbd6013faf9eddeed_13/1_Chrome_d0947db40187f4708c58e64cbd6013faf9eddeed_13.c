xmlParseElement(xmlParserCtxtPtr ctxt) {
    const xmlChar *name;
    const xmlChar *prefix = NULL;
    const xmlChar *URI = NULL;
    xmlParserNodeInfo node_info;
    int line, tlen;
    xmlNodePtr ret;
    int nsNr = ctxt->nsNr;

    if (((unsigned int) ctxt->nameNr > xmlParserMaxDepth) &&
        ((ctxt->options & XML_PARSE_HUGE) == 0)) {
	xmlFatalErrMsgInt(ctxt, XML_ERR_INTERNAL_ERROR,
		 "Excessive depth in document: %d use XML_PARSE_HUGE option\n",
			  xmlParserMaxDepth);
	ctxt->instate = XML_PARSER_EOF;
	return;
    }

    /* Capture start position */
    if (ctxt->record_info) {
        node_info.begin_pos = ctxt->input->consumed +
                          (CUR_PTR - ctxt->input->base);
	node_info.begin_line = ctxt->input->line;
    }

    if (ctxt->spaceNr == 0)
	spacePush(ctxt, -1);
    else if (*ctxt->space == -2)
	spacePush(ctxt, -1);
    else
	spacePush(ctxt, *ctxt->space);

    line = ctxt->input->line;
#ifdef LIBXML_SAX1_ENABLED
    if (ctxt->sax2)
#endif /* LIBXML_SAX1_ENABLED */
        name = xmlParseStartTag2(ctxt, &prefix, &URI, &tlen);
#ifdef LIBXML_SAX1_ENABLED
    else
	name = xmlParseStartTag(ctxt);
#endif /* LIBXML_SAX1_ENABLED */
    if (ctxt->instate == XML_PARSER_EOF)
	return;
    if (name == NULL) {
	spacePop(ctxt);
        return;
    }
    namePush(ctxt, name);
    ret = ctxt->node;

#ifdef LIBXML_VALID_ENABLED
    /*
     * [ VC: Root Element Type ]
     * The Name in the document type declaration must match the element
     * type of the root element. 
     */
    if (ctxt->validate && ctxt->wellFormed && ctxt->myDoc &&
        ctxt->node && (ctxt->node == ctxt->myDoc->children))
        ctxt->valid &= xmlValidateRoot(&ctxt->vctxt, ctxt->myDoc);
#endif /* LIBXML_VALID_ENABLED */

    /*
     * Check for an Empty Element.
     */
    if ((RAW == '/') && (NXT(1) == '>')) {
        SKIP(2);
	if (ctxt->sax2) {
	    if ((ctxt->sax != NULL) && (ctxt->sax->endElementNs != NULL) &&
		(!ctxt->disableSAX))
		ctxt->sax->endElementNs(ctxt->userData, name, prefix, URI);
#ifdef LIBXML_SAX1_ENABLED
	} else {
	    if ((ctxt->sax != NULL) && (ctxt->sax->endElement != NULL) &&
		(!ctxt->disableSAX))
		ctxt->sax->endElement(ctxt->userData, name);
#endif /* LIBXML_SAX1_ENABLED */
	}
	namePop(ctxt);
	spacePop(ctxt);
	if (nsNr != ctxt->nsNr)
	    nsPop(ctxt, ctxt->nsNr - nsNr);
	if ( ret != NULL && ctxt->record_info ) {
	   node_info.end_pos = ctxt->input->consumed +
			      (CUR_PTR - ctxt->input->base);
	   node_info.end_line = ctxt->input->line;
	   node_info.node = ret;
	   xmlParserAddNodeInfo(ctxt, &node_info);
	}
	return;
    }
    if (RAW == '>') {
        NEXT1;
    } else {
        xmlFatalErrMsgStrIntStr(ctxt, XML_ERR_GT_REQUIRED,
		     "Couldn't find end of Start Tag %s line %d\n",
		                name, line, NULL);

	/*
	 * end of parsing of this node.
	 */
	nodePop(ctxt);
	namePop(ctxt);
	spacePop(ctxt);
	if (nsNr != ctxt->nsNr)
	    nsPop(ctxt, ctxt->nsNr - nsNr);

	/*
	 * Capture end position and add node
	 */
	if ( ret != NULL && ctxt->record_info ) {
	   node_info.end_pos = ctxt->input->consumed +
			      (CUR_PTR - ctxt->input->base);
	   node_info.end_line = ctxt->input->line;
	   node_info.node = ret;
	   xmlParserAddNodeInfo(ctxt, &node_info);
	}
	return;
    }

    /*
      * Parse the content of the element:
      */
     xmlParseContent(ctxt);
     if (!IS_BYTE_CHAR(RAW)) {
         xmlFatalErrMsgStrIntStr(ctxt, XML_ERR_TAG_NOT_FINISHED,
 	 "Premature end of data in tag %s line %d\n",
		                name, line, NULL);

	/*
	 * end of parsing of this node.
	 */
	nodePop(ctxt);
	namePop(ctxt);
	spacePop(ctxt);
	if (nsNr != ctxt->nsNr)
	    nsPop(ctxt, ctxt->nsNr - nsNr);
	return;
    }

    /*
     * parse the end of tag: '</' should be here.
     */
    if (ctxt->sax2) {
	xmlParseEndTag2(ctxt, prefix, URI, line, ctxt->nsNr - nsNr, tlen);
	namePop(ctxt);
    }
#ifdef LIBXML_SAX1_ENABLED
      else
	xmlParseEndTag1(ctxt, line);
#endif /* LIBXML_SAX1_ENABLED */

    /*
     * Capture end position and add node
     */
    if ( ret != NULL && ctxt->record_info ) {
       node_info.end_pos = ctxt->input->consumed +
                          (CUR_PTR - ctxt->input->base);
       node_info.end_line = ctxt->input->line;
       node_info.node = ret;
       xmlParserAddNodeInfo(ctxt, &node_info);
    }
}
