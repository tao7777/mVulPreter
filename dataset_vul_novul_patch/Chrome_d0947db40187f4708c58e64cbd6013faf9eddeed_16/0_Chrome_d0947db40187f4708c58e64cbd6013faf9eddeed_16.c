xmlParseElementMixedContentDecl(xmlParserCtxtPtr ctxt, int inputchk) {
    xmlElementContentPtr ret = NULL, cur = NULL, n;
    const xmlChar *elem = NULL;

    GROW;
    if (CMP7(CUR_PTR, '#', 'P', 'C', 'D', 'A', 'T', 'A')) {
	SKIP(7);
	SKIP_BLANKS;
	SHRINK;
	if (RAW == ')') {
	    if ((ctxt->validate) && (ctxt->input->id != inputchk)) {
		xmlValidityError(ctxt, XML_ERR_ENTITY_BOUNDARY,
"Element content declaration doesn't start and stop in the same entity\n",
                                 NULL, NULL);
	    }
	    NEXT;
	    ret = xmlNewDocElementContent(ctxt->myDoc, NULL, XML_ELEMENT_CONTENT_PCDATA);
	    if (ret == NULL)
	        return(NULL);
	    if (RAW == '*') {
		ret->ocur = XML_ELEMENT_CONTENT_MULT;
		NEXT;
	    }
	    return(ret);
	}
	if ((RAW == '(') || (RAW == '|')) {
 	    ret = cur = xmlNewDocElementContent(ctxt->myDoc, NULL, XML_ELEMENT_CONTENT_PCDATA);
 	    if (ret == NULL) return(NULL);
 	}
	while ((RAW == '|') && (ctxt->instate != XML_PARSER_EOF)) {
 	    NEXT;
 	    if (elem == NULL) {
 	        ret = xmlNewDocElementContent(ctxt->myDoc, NULL, XML_ELEMENT_CONTENT_OR);
		if (ret == NULL) return(NULL);
		ret->c1 = cur;
		if (cur != NULL)
		    cur->parent = ret;
		cur = ret;
	    } else {
	        n = xmlNewDocElementContent(ctxt->myDoc, NULL, XML_ELEMENT_CONTENT_OR);
		if (n == NULL) return(NULL);
		n->c1 = xmlNewDocElementContent(ctxt->myDoc, elem, XML_ELEMENT_CONTENT_ELEMENT);
		if (n->c1 != NULL)
		    n->c1->parent = n;
	        cur->c2 = n;
		if (n != NULL)
		    n->parent = cur;
		cur = n;
	    }
	    SKIP_BLANKS;
	    elem = xmlParseName(ctxt);
	    if (elem == NULL) {
		xmlFatalErrMsg(ctxt, XML_ERR_NAME_REQUIRED,
			"xmlParseElementMixedContentDecl : Name expected\n");
		xmlFreeDocElementContent(ctxt->myDoc, cur);
		return(NULL);
	    }
	    SKIP_BLANKS;
	    GROW;
	}
	if ((RAW == ')') && (NXT(1) == '*')) {
	    if (elem != NULL) {
		cur->c2 = xmlNewDocElementContent(ctxt->myDoc, elem,
		                               XML_ELEMENT_CONTENT_ELEMENT);
		if (cur->c2 != NULL)
		    cur->c2->parent = cur;
            }
            if (ret != NULL)
                ret->ocur = XML_ELEMENT_CONTENT_MULT;
	    if ((ctxt->validate) && (ctxt->input->id != inputchk)) {
		xmlValidityError(ctxt, XML_ERR_ENTITY_BOUNDARY,
"Element content declaration doesn't start and stop in the same entity\n",
				 NULL, NULL);
	    }
	    SKIP(2);
	} else {
	    xmlFreeDocElementContent(ctxt->myDoc, ret);
	    xmlFatalErr(ctxt, XML_ERR_MIXED_NOT_STARTED, NULL);
	    return(NULL);
	}

    } else {
	xmlFatalErr(ctxt, XML_ERR_PCDATA_REQUIRED, NULL);
    }
    return(ret);
}
