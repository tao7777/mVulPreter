htmlParseNameComplex(xmlParserCtxtPtr ctxt) {
    int len = 0, l;
    int c;
    int count = 0;
    const xmlChar *base = ctxt->input->base;

    /*
     * Handler for more complex cases
     */
    GROW;
    c = CUR_CHAR(l);
    if ((c == ' ') || (c == '>') || (c == '/') || /* accelerators */
	(!IS_LETTER(c) && (c != '_') &&
         (c != ':'))) {
	return(NULL);
    }

    while ((c != ' ') && (c != '>') && (c != '/') && /* test bigname.xml */
	   ((IS_LETTER(c)) || (IS_DIGIT(c)) ||
            (c == '.') || (c == '-') ||
	    (c == '_') || (c == ':') ||
	    (IS_COMBINING(c)) ||
	    (IS_EXTENDER(c)))) {
	if (count++ > 100) {
	    count = 0;
	    GROW;
	}
	len += l;
	NEXTL(l);
	c = CUR_CHAR(l);
	if (ctxt->input->base != base) {
	    /*
	     * We changed encoding from an unknown encoding
	     * Input buffer changed location, so we better start again
	     */
	    return(htmlParseNameComplex(ctxt));
 	}
     }
 
    if (ctxt->input->cur - ctxt->input->base < len) {
        /* Sanity check */
	htmlParseErr(ctxt, XML_ERR_INTERNAL_ERROR,
                     "unexpected change of input buffer", NULL, NULL);
        return (NULL);
    }
 
     return(xmlDictLookup(ctxt->dict, ctxt->input->cur - len, len));
 }
