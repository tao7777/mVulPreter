xmlParsePubidLiteral(xmlParserCtxtPtr ctxt) {
    xmlChar *buf = NULL;
    int len = 0;
    int size = XML_PARSER_BUFFER_SIZE;
    xmlChar cur;
    xmlChar stop;
    int count = 0;
    xmlParserInputState oldstate = ctxt->instate;

    SHRINK;
    if (RAW == '"') {
        NEXT;
	stop = '"';
    } else if (RAW == '\'') {
        NEXT;
	stop = '\'';
    } else {
	xmlFatalErr(ctxt, XML_ERR_LITERAL_NOT_STARTED, NULL);
	return(NULL);
    }
    buf = (xmlChar *) xmlMallocAtomic(size * sizeof(xmlChar));
    if (buf == NULL) {
	xmlErrMemory(ctxt, NULL);
	return(NULL);
    }
    ctxt->instate = XML_PARSER_PUBLIC_LITERAL;
    cur = CUR;
    while ((IS_PUBIDCHAR_CH(cur)) && (cur != stop)) { /* checked */
	if (len + 1 >= size) {
	    xmlChar *tmp;

	    size *= 2;
	    tmp = (xmlChar *) xmlRealloc(buf, size * sizeof(xmlChar));
	    if (tmp == NULL) {
		xmlErrMemory(ctxt, NULL);
		xmlFree(buf);
		return(NULL);
	    }
	    buf = tmp;
	}
	buf[len++] = cur;
	count++;
 	if (count > 50) {
 	    GROW;
 	    count = 0;
            if (ctxt->instate == XML_PARSER_EOF) {
		xmlFree(buf);
		return(NULL);
            }
 	}
 	NEXT;
 	cur = CUR;
	if (cur == 0) {
	    GROW;
	    SHRINK;
	    cur = CUR;
	}
    }
    buf[len] = 0;
    if (cur != stop) {
	xmlFatalErr(ctxt, XML_ERR_LITERAL_NOT_FINISHED, NULL);
    } else {
	NEXT;
    }
    ctxt->instate = oldstate;
    return(buf);
}
