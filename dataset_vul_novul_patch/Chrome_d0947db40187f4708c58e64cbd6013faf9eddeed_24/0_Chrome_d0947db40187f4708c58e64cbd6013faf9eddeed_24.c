 xmlParseMisc(xmlParserCtxtPtr ctxt) {
    while ((ctxt->instate != XML_PARSER_EOF) &&
           (((RAW == '<') && (NXT(1) == '?')) ||
            (CMP4(CUR_PTR, '<', '!', '-', '-')) ||
            IS_BLANK_CH(CUR))) {
         if ((RAW == '<') && (NXT(1) == '?')) {
 	    xmlParsePI(ctxt);
 	} else if (IS_BLANK_CH(CUR)) {
	    NEXT;
	} else
	    xmlParseComment(ctxt);
    }
}
