xmlParseNCNameComplex(xmlParserCtxtPtr ctxt) {
    int len = 0, l;
    int c;
    int count = 0;

#ifdef DEBUG
    nbParseNCNameComplex++;
#endif

    /*
     * Handler for more complex cases
     */
    GROW;
    c = CUR_CHAR(l);
    if ((c == ' ') || (c == '>') || (c == '/') || /* accelerators */
	(!xmlIsNameStartChar(ctxt, c) || (c == ':'))) {
	return(NULL);
    }

    while ((c != ' ') && (c != '>') && (c != '/') && /* test bigname.xml */
	   (xmlIsNameChar(ctxt, c) && (c != ':'))) {
 	if (count++ > 100) {
 	    count = 0;
 	    GROW;
            if (ctxt->instate == XML_PARSER_EOF)
                return(NULL);
 	}
 	len += l;
 	NEXTL(l);
	c = CUR_CHAR(l);
    }
    return(xmlDictLookup(ctxt->dict, ctxt->input->cur - len, len));
}
