xmlPushInput(xmlParserCtxtPtr ctxt, xmlParserInputPtr input) {
    int ret;
    if (input == NULL) return(-1);

    if (xmlParserDebugEntities) {
	if ((ctxt->input != NULL) && (ctxt->input->filename))
	    xmlGenericError(xmlGenericErrorContext,
		    "%s(%d): ", ctxt->input->filename,
		    ctxt->input->line);
 	xmlGenericError(xmlGenericErrorContext,
 		"Pushing input %d : %.30s\n", ctxt->inputNr+1, input->cur);
     }
    if (((ctxt->inputNr > 40) && ((ctxt->options & XML_PARSE_HUGE) == 0)) ||
        (ctxt->inputNr > 1024)) {
        xmlFatalErr(ctxt, XML_ERR_ENTITY_LOOP, NULL);
        while (ctxt->inputNr > 1)
            xmlFreeInputStream(inputPop(ctxt));
	return(-1);
    }
     ret = inputPush(ctxt, input);
     if (ctxt->instate == XML_PARSER_EOF)
         return(-1);
    GROW;
    return(ret);
}
