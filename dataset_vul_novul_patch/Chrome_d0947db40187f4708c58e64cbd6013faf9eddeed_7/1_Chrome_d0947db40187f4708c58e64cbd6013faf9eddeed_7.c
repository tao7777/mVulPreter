xmlParseChunk(xmlParserCtxtPtr ctxt, const char *chunk, int size,
              int terminate) {
    int end_in_lf = 0;
    int remain = 0;

    if (ctxt == NULL)
         return(XML_ERR_INTERNAL_ERROR);
     if ((ctxt->errNo != XML_ERR_OK) && (ctxt->disableSAX == 1))
         return(ctxt->errNo);
     if (ctxt->instate == XML_PARSER_START)
         xmlDetectSAX2(ctxt);
     if ((size > 0) && (chunk != NULL) && (!terminate) &&
        (chunk[size - 1] == '\r')) {
	end_in_lf = 1;
	size--;
    }

xmldecl_done:

    if ((size > 0) && (chunk != NULL) && (ctxt->input != NULL) &&
        (ctxt->input->buf != NULL) && (ctxt->instate != XML_PARSER_EOF))  {
	int base = ctxt->input->base - ctxt->input->buf->buffer->content;
	int cur = ctxt->input->cur - ctxt->input->base;
	int res;

        /*
         * Specific handling if we autodetected an encoding, we should not
         * push more than the first line ... which depend on the encoding
         * And only push the rest once the final encoding was detected
         */
        if ((ctxt->instate == XML_PARSER_START) && (ctxt->input != NULL) &&
            (ctxt->input->buf != NULL) && (ctxt->input->buf->encoder != NULL)) {
            unsigned int len = 45;

            if ((xmlStrcasestr(BAD_CAST ctxt->input->buf->encoder->name,
                               BAD_CAST "UTF-16")) ||
                (xmlStrcasestr(BAD_CAST ctxt->input->buf->encoder->name,
                               BAD_CAST "UTF16")))
                len = 90;
            else if ((xmlStrcasestr(BAD_CAST ctxt->input->buf->encoder->name,
                                    BAD_CAST "UCS-4")) ||
                     (xmlStrcasestr(BAD_CAST ctxt->input->buf->encoder->name,
                                    BAD_CAST "UCS4")))
                len = 180;

            if (ctxt->input->buf->rawconsumed < len)
                len -= ctxt->input->buf->rawconsumed;

            /*
             * Change size for reading the initial declaration only
             * if size is greater than len. Otherwise, memmove in xmlBufferAdd
             * will blindly copy extra bytes from memory.
             */
            if (size > len) {
                remain = size - len;
                size = len;
            } else {
                remain = 0;
            }
        }
	res =xmlParserInputBufferPush(ctxt->input->buf, size, chunk);
	if (res < 0) {
	    ctxt->errNo = XML_PARSER_EOF;
	    ctxt->disableSAX = 1;
	    return (XML_PARSER_EOF);
	}
	ctxt->input->base = ctxt->input->buf->buffer->content + base;
	ctxt->input->cur = ctxt->input->base + cur;
	ctxt->input->end =
	    &ctxt->input->buf->buffer->content[ctxt->input->buf->buffer->use];
#ifdef DEBUG_PUSH
	xmlGenericError(xmlGenericErrorContext, "PP: pushed %d\n", size);
#endif

    } else if (ctxt->instate != XML_PARSER_EOF) {
	if ((ctxt->input != NULL) && ctxt->input->buf != NULL) {
	    xmlParserInputBufferPtr in = ctxt->input->buf;
	    if ((in->encoder != NULL) && (in->buffer != NULL) &&
		    (in->raw != NULL)) {
		int nbchars;

		nbchars = xmlCharEncInFunc(in->encoder, in->buffer, in->raw);
		if (nbchars < 0) {
		    /* TODO 2.6.0 */
		    xmlGenericError(xmlGenericErrorContext,
				    "xmlParseChunk: encoder error\n");
		    return(XML_ERR_INVALID_ENCODING);
		}
	    }
	}
    }
    if (remain != 0)
         xmlParseTryOrFinish(ctxt, 0);
     else
         xmlParseTryOrFinish(ctxt, terminate);
     if ((ctxt->errNo != XML_ERR_OK) && (ctxt->disableSAX == 1))
         return(ctxt->errNo);
 
    if (remain != 0) {
        chunk += size;
        size = remain;
        remain = 0;
        goto xmldecl_done;
    }
    if ((end_in_lf == 1) && (ctxt->input != NULL) &&
        (ctxt->input->buf != NULL)) {
	xmlParserInputBufferPush(ctxt->input->buf, 1, "\r");
    }
    if (terminate) {
	/*
	 * Check for termination
	 */
	int avail = 0;

	if (ctxt->input != NULL) {
	    if (ctxt->input->buf == NULL)
		avail = ctxt->input->length -
			(ctxt->input->cur - ctxt->input->base);
	    else
		avail = ctxt->input->buf->buffer->use -
			(ctxt->input->cur - ctxt->input->base);
	}
			    
	if ((ctxt->instate != XML_PARSER_EOF) &&
	    (ctxt->instate != XML_PARSER_EPILOG)) {
	    xmlFatalErr(ctxt, XML_ERR_DOCUMENT_END, NULL);
	} 
	if ((ctxt->instate == XML_PARSER_EPILOG) && (avail > 0)) {
	    xmlFatalErr(ctxt, XML_ERR_DOCUMENT_END, NULL);
	}
	if (ctxt->instate != XML_PARSER_EOF) {
	    if ((ctxt->sax) && (ctxt->sax->endDocument != NULL))
		ctxt->sax->endDocument(ctxt->userData);
	}
	ctxt->instate = XML_PARSER_EOF;
    }
    return((xmlParserErrors) ctxt->errNo);	      
}
