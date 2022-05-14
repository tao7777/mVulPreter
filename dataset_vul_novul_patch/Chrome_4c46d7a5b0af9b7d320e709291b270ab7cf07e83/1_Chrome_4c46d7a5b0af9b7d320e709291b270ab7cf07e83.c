xmlXPtrEvalXPtrPart(xmlXPathParserContextPtr ctxt, xmlChar *name) {
    xmlChar *buffer, *cur;
    int len;
    int level;

    if (name == NULL)
    name = xmlXPathParseName(ctxt);
    if (name == NULL)
	XP_ERROR(XPATH_EXPR_ERROR);

    if (CUR != '(')
	XP_ERROR(XPATH_EXPR_ERROR);
    NEXT;
    level = 1;

    len = xmlStrlen(ctxt->cur);
    len++;
    buffer = (xmlChar *) xmlMallocAtomic(len * sizeof (xmlChar));
    if (buffer == NULL) {
        xmlXPtrErrMemory("allocating buffer");
	return;
    }

    cur = buffer;
    while (CUR != 0) {
	if (CUR == ')') {
	    level--;
	    if (level == 0) {
 		NEXT;
 		break;
 	    }
	    *cur++ = CUR;
 	} else if (CUR == '(') {
 	    level++;
	    *cur++ = CUR;
 	} else if (CUR == '^') {
	    NEXT;
	    if ((CUR == ')') || (CUR == '(') || (CUR == '^')) {
		*cur++ = CUR;
	    } else {
		*cur++ = '^';
		*cur++ = CUR;
 	    }
	} else {
	    *cur++ = CUR;
 	}
 	NEXT;
     }
     *cur = 0;

    if ((level != 0) && (CUR == 0)) {
	xmlFree(buffer);
	XP_ERROR(XPTR_SYNTAX_ERROR);
    }

    if (xmlStrEqual(name, (xmlChar *) "xpointer")) {
	const xmlChar *left = CUR_PTR;

	CUR_PTR = buffer;
	/*
	 * To evaluate an xpointer scheme element (4.3) we need:
	 *   context initialized to the root
	 *   context position initalized to 1
	 *   context size initialized to 1
	 */
	ctxt->context->node = (xmlNodePtr)ctxt->context->doc;
	ctxt->context->proximityPosition = 1;
	ctxt->context->contextSize = 1;
	xmlXPathEvalExpr(ctxt);
	CUR_PTR=left;
    } else if (xmlStrEqual(name, (xmlChar *) "element")) {
	const xmlChar *left = CUR_PTR;
	xmlChar *name2;

	CUR_PTR = buffer;
	if (buffer[0] == '/') {
	    xmlXPathRoot(ctxt);
	    xmlXPtrEvalChildSeq(ctxt, NULL);
	} else {
	    name2 = xmlXPathParseName(ctxt);
	    if (name2 == NULL) {
		CUR_PTR = left;
		xmlFree(buffer);
		XP_ERROR(XPATH_EXPR_ERROR);
	    }
	    xmlXPtrEvalChildSeq(ctxt, name2);
	}
	CUR_PTR = left;
#ifdef XPTR_XMLNS_SCHEME
    } else if (xmlStrEqual(name, (xmlChar *) "xmlns")) {
	const xmlChar *left = CUR_PTR;
	xmlChar *prefix;
	xmlChar *URI;
	xmlURIPtr value;

	CUR_PTR = buffer;
        prefix = xmlXPathParseNCName(ctxt);
	if (prefix == NULL) {
	    xmlFree(buffer);
	    xmlFree(name);
	    XP_ERROR(XPTR_SYNTAX_ERROR);
	}
	SKIP_BLANKS;
	if (CUR != '=') {
	    xmlFree(prefix);
	    xmlFree(buffer);
	    xmlFree(name);
	    XP_ERROR(XPTR_SYNTAX_ERROR);
	}
	NEXT;
	SKIP_BLANKS;
	/* @@ check escaping in the XPointer WD */

	value = xmlParseURI((const char *)ctxt->cur);
	if (value == NULL) {
	    xmlFree(prefix);
	    xmlFree(buffer);
	    xmlFree(name);
	    XP_ERROR(XPTR_SYNTAX_ERROR);
	}
	URI = xmlSaveUri(value);
	xmlFreeURI(value);
	if (URI == NULL) {
	    xmlFree(prefix);
	    xmlFree(buffer);
	    xmlFree(name);
	    XP_ERROR(XPATH_MEMORY_ERROR);
	}
	
	xmlXPathRegisterNs(ctxt->context, prefix, URI);
	CUR_PTR = left;
	xmlFree(URI);
	xmlFree(prefix);
#endif /* XPTR_XMLNS_SCHEME */
    } else {
        xmlXPtrErr(ctxt, XML_XPTR_UNKNOWN_SCHEME,
		   "unsupported scheme '%s'\n", name);
    }
    xmlFree(buffer);
    xmlFree(name);
}
