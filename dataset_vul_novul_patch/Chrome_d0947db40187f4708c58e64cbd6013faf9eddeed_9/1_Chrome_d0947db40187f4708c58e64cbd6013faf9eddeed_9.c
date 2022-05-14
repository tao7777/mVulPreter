xmlParseCommentComplex(xmlParserCtxtPtr ctxt, xmlChar *buf, int len, int size) {
    int q, ql;
    int r, rl;
    int cur, l;
    int count = 0;
    int inputid;

    inputid = ctxt->input->id;

    if (buf == NULL) {
        len = 0;
	size = XML_PARSER_BUFFER_SIZE;
	buf = (xmlChar *) xmlMallocAtomic(size * sizeof(xmlChar));
	if (buf == NULL) {
	    xmlErrMemory(ctxt, NULL);
	    return;
	}
    }
    GROW;	/* Assure there's enough input data */
    q = CUR_CHAR(ql);
    if (q == 0)
        goto not_terminated;
    if (!IS_CHAR(q)) {
        xmlFatalErrMsgInt(ctxt, XML_ERR_INVALID_CHAR,
                          "xmlParseComment: invalid xmlChar value %d\n",
	                  q);
	xmlFree (buf);
	return;
    }
    NEXTL(ql);
    r = CUR_CHAR(rl);
    if (r == 0)
        goto not_terminated;
    if (!IS_CHAR(r)) {
        xmlFatalErrMsgInt(ctxt, XML_ERR_INVALID_CHAR,
                          "xmlParseComment: invalid xmlChar value %d\n",
	                  q);
	xmlFree (buf);
	return;
    }
    NEXTL(rl);
    cur = CUR_CHAR(l);
    if (cur == 0)
        goto not_terminated;
    while (IS_CHAR(cur) && /* checked */
           ((cur != '>') ||
	    (r != '-') || (q != '-'))) {
	if ((r == '-') && (q == '-')) {
	    xmlFatalErr(ctxt, XML_ERR_HYPHEN_IN_COMMENT, NULL);
	}
	if (len + 5 >= size) {
	    xmlChar *new_buf;
	    size *= 2;
	    new_buf = (xmlChar *) xmlRealloc(buf, size * sizeof(xmlChar));
	    if (new_buf == NULL) {
		xmlFree (buf);
		xmlErrMemory(ctxt, NULL);
		return;
	    }
	    buf = new_buf;
	}
	COPY_BUF(ql,buf,len,q);
	q = r;
	ql = rl;
	r = cur;
	rl = l;

	count++;
 	if (count > 50) {
 	    GROW;
 	    count = 0;
 	}
 	NEXTL(l);
 	cur = CUR_CHAR(l);
	if (cur == 0) {
	    SHRINK;
	    GROW;
	    cur = CUR_CHAR(l);
	}
    }
    buf[len] = 0;
    if (cur == 0) {
	xmlFatalErrMsgStr(ctxt, XML_ERR_COMMENT_NOT_FINISHED,
	                     "Comment not terminated \n<!--%.50s\n", buf);
    } else if (!IS_CHAR(cur)) {
        xmlFatalErrMsgInt(ctxt, XML_ERR_INVALID_CHAR,
                          "xmlParseComment: invalid xmlChar value %d\n",
	                  cur);
    } else {
	if (inputid != ctxt->input->id) {
	    xmlFatalErrMsg(ctxt, XML_ERR_ENTITY_BOUNDARY,
		"Comment doesn't start and stop in the same entity\n");
	}
        NEXT;
	if ((ctxt->sax != NULL) && (ctxt->sax->comment != NULL) &&
	    (!ctxt->disableSAX))
	    ctxt->sax->comment(ctxt->userData, buf);
    }
    xmlFree(buf);
    return;
not_terminated:
    xmlFatalErrMsgStr(ctxt, XML_ERR_COMMENT_NOT_FINISHED,
			 "Comment not terminated\n", NULL);
    xmlFree(buf);
    return;
}
