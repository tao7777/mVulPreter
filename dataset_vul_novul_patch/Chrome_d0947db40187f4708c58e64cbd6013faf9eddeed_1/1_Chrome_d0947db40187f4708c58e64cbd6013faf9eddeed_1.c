xmlParseAttValueInternal(xmlParserCtxtPtr ctxt, int *len, int *alloc,
                         int normalize)
{
    xmlChar limit = 0;
    const xmlChar *in = NULL, *start, *end, *last;
    xmlChar *ret = NULL;

    GROW;
    in = (xmlChar *) CUR_PTR;
    if (*in != '"' && *in != '\'') {
        xmlFatalErr(ctxt, XML_ERR_ATTRIBUTE_NOT_STARTED, NULL);
        return (NULL);
    }
    ctxt->instate = XML_PARSER_ATTRIBUTE_VALUE;

    /*
     * try to handle in this routine the most common case where no
     * allocation of a new string is required and where content is
     * pure ASCII.
     */
    limit = *in++;
    end = ctxt->input->end;
    start = in;
    if (in >= end) {
        const xmlChar *oldbase = ctxt->input->base;
	GROW;
	if (oldbase != ctxt->input->base) {
	    long delta = ctxt->input->base - oldbase;
	    start = start + delta;
	    in = in + delta;
	}
	end = ctxt->input->end;
    }
    if (normalize) {
        /*
	 * Skip any leading spaces
	 */
	while ((in < end) && (*in != limit) && 
	       ((*in == 0x20) || (*in == 0x9) ||
	        (*in == 0xA) || (*in == 0xD))) {
	    in++;
	    start = in;
 	    if (in >= end) {
 		const xmlChar *oldbase = ctxt->input->base;
 		GROW;
 		if (oldbase != ctxt->input->base) {
 		    long delta = ctxt->input->base - oldbase;
 		    start = start + delta;
		    in = in + delta;
		}
		end = ctxt->input->end;
	    }
	}
	while ((in < end) && (*in != limit) && (*in >= 0x20) &&
	       (*in <= 0x7f) && (*in != '&') && (*in != '<')) {
	    if ((*in++ == 0x20) && (*in == 0x20)) break;
 	    if (in >= end) {
 		const xmlChar *oldbase = ctxt->input->base;
 		GROW;
 		if (oldbase != ctxt->input->base) {
 		    long delta = ctxt->input->base - oldbase;
 		    start = start + delta;
		    in = in + delta;
		}
		end = ctxt->input->end;
	    }
	}
	last = in;
	/*
	 * skip the trailing blanks
	 */
	while ((last[-1] == 0x20) && (last > start)) last--;
	while ((in < end) && (*in != limit) && 
	       ((*in == 0x20) || (*in == 0x9) ||
	        (*in == 0xA) || (*in == 0xD))) {
	    in++;
 	    if (in >= end) {
 		const xmlChar *oldbase = ctxt->input->base;
 		GROW;
 		if (oldbase != ctxt->input->base) {
 		    long delta = ctxt->input->base - oldbase;
 		    start = start + delta;
		    in = in + delta;
		    last = last + delta;
		}
		end = ctxt->input->end;
	    }
	}
	if (*in != limit) goto need_complex;
    } else {
	while ((in < end) && (*in != limit) && (*in >= 0x20) &&
	       (*in <= 0x7f) && (*in != '&') && (*in != '<')) {
	    in++;
 	    if (in >= end) {
 		const xmlChar *oldbase = ctxt->input->base;
 		GROW;
 		if (oldbase != ctxt->input->base) {
 		    long delta = ctxt->input->base - oldbase;
 		    start = start + delta;
		    in = in + delta;
		}
		end = ctxt->input->end;
	    }
	}
	last = in;
	if (*in != limit) goto need_complex;
    }
    in++;
    if (len != NULL) {
        *len = last - start;
        ret = (xmlChar *) start;
    } else {
        if (alloc) *alloc = 1;
        ret = xmlStrndup(start, last - start);
    }
    CUR_PTR = in;
    if (alloc) *alloc = 0;
    return ret;
need_complex:
    if (alloc) *alloc = 1;
    return xmlParseAttValueComplex(ctxt, len, normalize);
}
