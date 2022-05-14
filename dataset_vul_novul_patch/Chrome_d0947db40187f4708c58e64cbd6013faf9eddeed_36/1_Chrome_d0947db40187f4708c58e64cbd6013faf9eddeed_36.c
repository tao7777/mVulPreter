xmlParseSystemLiteral(xmlParserCtxtPtr ctxt) {
    xmlChar *buf = NULL;
    int len = 0;
    int size = XML_PARSER_BUFFER_SIZE;
    int cur, l;
    xmlChar stop;
    int state = ctxt->instate;
    int count = 0;

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
    ctxt->instate = XML_PARSER_SYSTEM_LITERAL;
    cur = CUR_CHAR(l);
    while ((IS_CHAR(cur)) && (cur != stop)) { /* checked */
	if (len + 5 >= size) {
	    xmlChar *tmp;

	    size *= 2;
	    tmp = (xmlChar *) xmlRealloc(buf, size * sizeof(xmlChar));
	    if (tmp == NULL) {
	        xmlFree(buf);
		xmlErrMemory(ctxt, NULL);
		ctxt->instate = (xmlParserInputState) state;
		return(NULL);
	    }
	    buf = tmp;
	}
	count++;
 	if (count > 50) {
 	    GROW;
 	    count = 0;
 	}
 	COPY_BUF(l,buf,len,cur);
 	NEXTL(l);
	cur = CUR_CHAR(l);
	if (cur == 0) {
	    GROW;
	    SHRINK;
	    cur = CUR_CHAR(l);
	}
    }
    buf[len] = 0;
    ctxt->instate = (xmlParserInputState) state;
    if (!IS_CHAR(cur)) {
	xmlFatalErr(ctxt, XML_ERR_LITERAL_NOT_FINISHED, NULL);
    } else {
	NEXT;
    }
    return(buf);
}
