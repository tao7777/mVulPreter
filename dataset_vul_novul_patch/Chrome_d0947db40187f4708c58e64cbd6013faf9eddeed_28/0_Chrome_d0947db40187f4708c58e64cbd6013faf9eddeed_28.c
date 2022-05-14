xmlParseNmtoken(xmlParserCtxtPtr ctxt) {
    xmlChar buf[XML_MAX_NAMELEN + 5];
    int len = 0, l;
    int c;
    int count = 0;

#ifdef DEBUG
    nbParseNmToken++;
 #endif
 
     GROW;
    if (ctxt->instate == XML_PARSER_EOF)
        return(NULL);
     c = CUR_CHAR(l);
 
     while (xmlIsNameChar(ctxt, c)) {
	if (count++ > 100) {
	    count = 0;
	    GROW;
	}
	COPY_BUF(l,buf,len,c);
	NEXTL(l);
	c = CUR_CHAR(l);
	if (len >= XML_MAX_NAMELEN) {
	    /*
	     * Okay someone managed to make a huge token, so he's ready to pay
	     * for the processing speed.
	     */
	    xmlChar *buffer;
	    int max = len * 2;

	    buffer = (xmlChar *) xmlMallocAtomic(max * sizeof(xmlChar));
	    if (buffer == NULL) {
	        xmlErrMemory(ctxt, NULL);
		return(NULL);
	    }
	    memcpy(buffer, buf, len);
	    while (xmlIsNameChar(ctxt, c)) {
 		if (count++ > 100) {
 		    count = 0;
 		    GROW;
                    if (ctxt->instate == XML_PARSER_EOF) {
                        xmlFree(buffer);
                        return(NULL);
                    }
 		}
 		if (len + 10 > max) {
 		    xmlChar *tmp;

		    max *= 2;
		    tmp = (xmlChar *) xmlRealloc(buffer,
			                            max * sizeof(xmlChar));
		    if (tmp == NULL) {
			xmlErrMemory(ctxt, NULL);
			xmlFree(buffer);
			return(NULL);
		    }
		    buffer = tmp;
		}
		COPY_BUF(l,buffer,len,c);
		NEXTL(l);
		c = CUR_CHAR(l);
	    }
	    buffer[len] = 0;
	    return(buffer);
	}
    }
    if (len == 0)
        return(NULL);
    return(xmlStrndup(buf, len));
}
