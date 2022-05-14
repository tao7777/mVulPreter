xmlStopParser(xmlParserCtxtPtr ctxt) {
     if (ctxt == NULL)
         return;
     ctxt->instate = XML_PARSER_EOF;
    ctxt->errNo = XML_ERR_USER_STOP;
     ctxt->disableSAX = 1;
     if (ctxt->input != NULL) {
 	ctxt->input->cur = BAD_CAST"";
	ctxt->input->base = ctxt->input->cur;
    }
}
