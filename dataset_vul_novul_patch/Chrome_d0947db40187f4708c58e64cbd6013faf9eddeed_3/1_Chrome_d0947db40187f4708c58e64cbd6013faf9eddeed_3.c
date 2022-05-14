xmlParseCDSect(xmlParserCtxtPtr ctxt) {
    xmlChar *buf = NULL;
    int len = 0;
    int size = XML_PARSER_BUFFER_SIZE;
    int r, rl;
    int	s, sl;
    int cur, l;
    int count = 0;

    /* Check 2.6.0 was NXT(0) not RAW */
    if (CMP9(CUR_PTR, '<', '!', '[', 'C', 'D', 'A', 'T', 'A', '[')) {
	SKIP(9);
    } else
        return;

    ctxt->instate = XML_PARSER_CDATA_SECTION;
    r = CUR_CHAR(rl);
    if (!IS_CHAR(r)) {
	xmlFatalErr(ctxt, XML_ERR_CDATA_NOT_FINISHED, NULL);
	ctxt->instate = XML_PARSER_CONTENT;
        return;
    }
    NEXTL(rl);
    s = CUR_CHAR(sl);
    if (!IS_CHAR(s)) {
	xmlFatalErr(ctxt, XML_ERR_CDATA_NOT_FINISHED, NULL);
	ctxt->instate = XML_PARSER_CONTENT;
        return;
    }
    NEXTL(sl);
    cur = CUR_CHAR(l);
    buf = (xmlChar *) xmlMallocAtomic(size * sizeof(xmlChar));
    if (buf == NULL) {
	xmlErrMemory(ctxt, NULL);
	return;
    }
    while (IS_CHAR(cur) &&
           ((r != ']') || (s != ']') || (cur != '>'))) {
	if (len + 5 >= size) {
	    xmlChar *tmp;

	    size *= 2;
	    tmp = (xmlChar *) xmlRealloc(buf, size * sizeof(xmlChar));
	    if (tmp == NULL) {
	        xmlFree(buf);
		xmlErrMemory(ctxt, NULL);
		return;
	    }
	    buf = tmp;
	}
	COPY_BUF(rl,buf,len,r);
	r = s;
	rl = sl;
	s = cur;
	sl = l;
 	count++;
 	if (count > 50) {
 	    GROW;
 	    count = 0;
 	}
 	NEXTL(l);
	cur = CUR_CHAR(l);
    }
    buf[len] = 0;
    ctxt->instate = XML_PARSER_CONTENT;
    if (cur != '>') {
	xmlFatalErrMsgStr(ctxt, XML_ERR_CDATA_NOT_FINISHED,
	                     "CData section not finished\n%.50s\n", buf);
	xmlFree(buf);
        return;
    }
    NEXTL(l);

    /*
     * OK the buffer is to be consumed as cdata.
     */
    if ((ctxt->sax != NULL) && (!ctxt->disableSAX)) {
	if (ctxt->sax->cdataBlock != NULL)
	    ctxt->sax->cdataBlock(ctxt->userData, buf, len);
	else if (ctxt->sax->characters != NULL)
	    ctxt->sax->characters(ctxt->userData, buf, len);
    }
    xmlFree(buf);
}
