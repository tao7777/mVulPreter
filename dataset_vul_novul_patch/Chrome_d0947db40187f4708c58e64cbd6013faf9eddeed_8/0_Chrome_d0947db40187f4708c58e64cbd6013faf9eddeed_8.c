xmlParseComment(xmlParserCtxtPtr ctxt) {
    xmlChar *buf = NULL;
    int size = XML_PARSER_BUFFER_SIZE;
    int len = 0;
    xmlParserInputState state;
    const xmlChar *in;
    int nbchar = 0, ccol;
    int inputid;

    /*
     * Check that there is a comment right here.
     */
    if ((RAW != '<') || (NXT(1) != '!') ||
        (NXT(2) != '-') || (NXT(3) != '-')) return;
    state = ctxt->instate;
    ctxt->instate = XML_PARSER_COMMENT;
    inputid = ctxt->input->id;
    SKIP(4);
    SHRINK;
    GROW;

    /*
     * Accelerated common case where input don't need to be
     * modified before passing it to the handler.
     */
    in = ctxt->input->cur;
    do {
	if (*in == 0xA) {
	    do {
		ctxt->input->line++; ctxt->input->col = 1;
		in++;
	    } while (*in == 0xA);
	}
get_more:
        ccol = ctxt->input->col;
	while (((*in > '-') && (*in <= 0x7F)) ||
	       ((*in >= 0x20) && (*in < '-')) ||
	       (*in == 0x09)) {
		    in++;
		    ccol++;
	}
	ctxt->input->col = ccol;
	if (*in == 0xA) {
	    do {
		ctxt->input->line++; ctxt->input->col = 1;
		in++;
	    } while (*in == 0xA);
	    goto get_more;
	}
	nbchar = in - ctxt->input->cur;
	/*
	 * save current set of data
	 */
	if (nbchar > 0) {
	    if ((ctxt->sax != NULL) &&
		(ctxt->sax->comment != NULL)) {
		if (buf == NULL) {
		    if ((*in == '-') && (in[1] == '-'))
		        size = nbchar + 1;
		    else
		        size = XML_PARSER_BUFFER_SIZE + nbchar;
		    buf = (xmlChar *) xmlMallocAtomic(size * sizeof(xmlChar));
		    if (buf == NULL) {
		        xmlErrMemory(ctxt, NULL);
			ctxt->instate = state;
			return;
		    }
		    len = 0;
		} else if (len + nbchar + 1 >= size) {
		    xmlChar *new_buf;
		    size  += len + nbchar + XML_PARSER_BUFFER_SIZE;
		    new_buf = (xmlChar *) xmlRealloc(buf,
		                                     size * sizeof(xmlChar));
		    if (new_buf == NULL) {
		        xmlFree (buf);
			xmlErrMemory(ctxt, NULL);
			ctxt->instate = state;
			return;
		    }
		    buf = new_buf;
		}
		memcpy(&buf[len], ctxt->input->cur, nbchar);
		len += nbchar;
		buf[len] = 0;
	    }
	}
	ctxt->input->cur = in;
	if (*in == 0xA) {
	    in++;
	    ctxt->input->line++; ctxt->input->col = 1;
	}
	if (*in == 0xD) {
	    in++;
	    if (*in == 0xA) {
		ctxt->input->cur = in;
		in++;
		ctxt->input->line++; ctxt->input->col = 1;
		continue; /* while */
	    }
	    in--;
 	}
 	SHRINK;
 	GROW;
        if (ctxt->instate == XML_PARSER_EOF) {
            xmlFree(buf);
            return;
	}
 	in = ctxt->input->cur;
 	if (*in == '-') {
 	    if (in[1] == '-') {
	        if (in[2] == '>') {
		    if (ctxt->input->id != inputid) {
			xmlFatalErrMsg(ctxt, XML_ERR_ENTITY_BOUNDARY,
			"comment doesn't start and stop in the same entity\n");
		    }
		    SKIP(3);
		    if ((ctxt->sax != NULL) && (ctxt->sax->comment != NULL) &&
		        (!ctxt->disableSAX)) {
			if (buf != NULL)
			    ctxt->sax->comment(ctxt->userData, buf);
			else
			    ctxt->sax->comment(ctxt->userData, BAD_CAST "");
 		    }
 		    if (buf != NULL)
 		        xmlFree(buf);
		    if (ctxt->instate != XML_PARSER_EOF)
			ctxt->instate = state;
 		    return;
 		}
 		if (buf != NULL)
		    xmlFatalErrMsgStr(ctxt, XML_ERR_COMMENT_NOT_FINISHED,
		                      "Comment not terminated \n<!--%.50s\n",
				      buf);
		else
		    xmlFatalErrMsgStr(ctxt, XML_ERR_COMMENT_NOT_FINISHED,
		                      "Comment not terminated \n", NULL);
		in++;
		ctxt->input->col++;
	    }
	    in++;
	    ctxt->input->col++;
	    goto get_more;
	}
    } while (((*in >= 0x20) && (*in <= 0x7F)) || (*in == 0x09));
    xmlParseCommentComplex(ctxt, buf, len, size);
    ctxt->instate = state;
    return;
}
