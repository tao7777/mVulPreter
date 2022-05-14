htmlInitParserCtxt(htmlParserCtxtPtr ctxt)
{
    htmlSAXHandler *sax;

    if (ctxt == NULL) return(-1);
    memset(ctxt, 0, sizeof(htmlParserCtxt));

    ctxt->dict = xmlDictCreate();
    if (ctxt->dict == NULL) {
        htmlErrMemory(NULL, "htmlInitParserCtxt: out of memory\n");
	return(-1);
    }
    sax = (htmlSAXHandler *) xmlMalloc(sizeof(htmlSAXHandler));
    if (sax == NULL) {
        htmlErrMemory(NULL, "htmlInitParserCtxt: out of memory\n");
	return(-1);
    }
    else
        memset(sax, 0, sizeof(htmlSAXHandler));

    /* Allocate the Input stack */
    ctxt->inputTab = (htmlParserInputPtr *)
                      xmlMalloc(5 * sizeof(htmlParserInputPtr));
    if (ctxt->inputTab == NULL) {
        htmlErrMemory(NULL, "htmlInitParserCtxt: out of memory\n");
	ctxt->inputNr = 0;
	ctxt->inputMax = 0;
	ctxt->input = NULL;
	return(-1);
    }
    ctxt->inputNr = 0;
    ctxt->inputMax = 5;
    ctxt->input = NULL;
    ctxt->version = NULL;
    ctxt->encoding = NULL;
    ctxt->standalone = -1;
    ctxt->instate = XML_PARSER_START;

    /* Allocate the Node stack */
    ctxt->nodeTab = (htmlNodePtr *) xmlMalloc(10 * sizeof(htmlNodePtr));
    if (ctxt->nodeTab == NULL) {
        htmlErrMemory(NULL, "htmlInitParserCtxt: out of memory\n");
	ctxt->nodeNr = 0;
	ctxt->nodeMax = 0;
	ctxt->node = NULL;
	ctxt->inputNr = 0;
	ctxt->inputMax = 0;
	ctxt->input = NULL;
	return(-1);
    }
    ctxt->nodeNr = 0;
    ctxt->nodeMax = 10;
    ctxt->node = NULL;

    /* Allocate the Name stack */
    ctxt->nameTab = (const xmlChar **) xmlMalloc(10 * sizeof(xmlChar *));
    if (ctxt->nameTab == NULL) {
        htmlErrMemory(NULL, "htmlInitParserCtxt: out of memory\n");
	ctxt->nameNr = 0;
	ctxt->nameMax = 0;
	ctxt->name = NULL;
	ctxt->nodeNr = 0;
	ctxt->nodeMax = 0;
	ctxt->node = NULL;
	ctxt->inputNr = 0;
	ctxt->inputMax = 0;
	ctxt->input = NULL;
	return(-1);
    }
    ctxt->nameNr = 0;
    ctxt->nameMax = 10;
    ctxt->name = NULL;

    ctxt->nodeInfoTab = NULL;
    ctxt->nodeInfoNr  = 0;
    ctxt->nodeInfoMax = 0;

    if (sax == NULL) ctxt->sax = (xmlSAXHandlerPtr) &htmlDefaultSAXHandler;
    else {
        ctxt->sax = sax;
	memcpy(sax, &htmlDefaultSAXHandler, sizeof(xmlSAXHandlerV1));
    }
    ctxt->userData = ctxt;
    ctxt->myDoc = NULL;
     ctxt->wellFormed = 1;
     ctxt->replaceEntities = 0;
     ctxt->linenumbers = xmlLineNumbersDefaultValue;
     ctxt->html = 1;
     ctxt->vctxt.finishDtd = XML_CTXT_FINISH_DTD_0;
     ctxt->vctxt.userData = ctxt;
    ctxt->vctxt.error = xmlParserValidityError;
    ctxt->vctxt.warning = xmlParserValidityWarning;
    ctxt->record_info = 0;
    ctxt->validate = 0;
    ctxt->nbChars = 0;
    ctxt->checkIndex = 0;
    ctxt->catalogs = NULL;
    xmlInitNodeInfoSeq(&ctxt->node_seq);
    return(0);
}
