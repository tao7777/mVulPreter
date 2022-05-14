xmlParseNameAndCompare(xmlParserCtxtPtr ctxt, xmlChar const *other) {
    register const xmlChar *cmp = other;
    register const xmlChar *in;
     const xmlChar *ret;
 
     GROW;
 
     in = ctxt->input->cur;
     while (*in != 0 && *in == *cmp) {
	++in;
	++cmp;
	ctxt->input->col++;
    }
    if (*cmp == 0 && (*in == '>' || IS_BLANK_CH (*in))) {
	/* success */
	ctxt->input->cur = in;
	return (const xmlChar*) 1;
    }
    /* failure (or end of input buffer), check with full function */
    ret = xmlParseName (ctxt);
    /* strings coming from the dictionnary direct compare possible */
    if (ret == other) {
	return (const xmlChar*) 1;
    }
    return ret;
}
