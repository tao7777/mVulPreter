xmlParsePI(xmlParserCtxtPtr ctxt) {
    xmlChar *buf = NULL;
    int len = 0;
    int size = XML_PARSER_BUFFER_SIZE;
    int cur, l;
    const xmlChar *target;
    xmlParserInputState state;
    int count = 0;

    if ((RAW == '<') && (NXT(1) == '?')) {
	xmlParserInputPtr input = ctxt->input;
	state = ctxt->instate;
        ctxt->instate = XML_PARSER_PI;
	/*
	 * this is a Processing Instruction.
	 */
	SKIP(2);
	SHRINK;

	/*
	 * Parse the target name and check for special support like
	 * namespace.
	 */
        target = xmlParsePITarget(ctxt);
	if (target != NULL) {
	    if ((RAW == '?') && (NXT(1) == '>')) {
		if (input != ctxt->input) {
		    xmlFatalErrMsg(ctxt, XML_ERR_ENTITY_BOUNDARY,
	    "PI declaration doesn't start and stop in the same entity\n");
		}
		SKIP(2);

		/*
		 * SAX: PI detected.
		 */
		if ((ctxt->sax) && (!ctxt->disableSAX) &&
 		    (ctxt->sax->processingInstruction != NULL))
 		    ctxt->sax->processingInstruction(ctxt->userData,
 		                                     target, NULL);
		if (ctxt->instate != XML_PARSER_EOF)
		    ctxt->instate = state;
 		return;
 	    }
 	    buf = (xmlChar *) xmlMallocAtomic(size * sizeof(xmlChar));
	    if (buf == NULL) {
		xmlErrMemory(ctxt, NULL);
		ctxt->instate = state;
		return;
	    }
	    cur = CUR;
	    if (!IS_BLANK(cur)) {
		xmlFatalErrMsgStr(ctxt, XML_ERR_SPACE_REQUIRED,
			  "ParsePI: PI %s space expected\n", target);
	    }
            SKIP_BLANKS;
	    cur = CUR_CHAR(l);
	    while (IS_CHAR(cur) && /* checked */
		   ((cur != '?') || (NXT(1) != '>'))) {
		if (len + 5 >= size) {
		    xmlChar *tmp;

		    size *= 2;
		    tmp = (xmlChar *) xmlRealloc(buf, size * sizeof(xmlChar));
		    if (tmp == NULL) {
			xmlErrMemory(ctxt, NULL);
			xmlFree(buf);
			ctxt->instate = state;
			return;
		    }
		    buf = tmp;
		}
		count++;
		if (count > 50) {
		    GROW;
		    count = 0;
		}
		COPY_BUF(l,buf,len,cur);
		NEXTL(l);
		cur = CUR_CHAR(l);
		if (cur == 0) {
		    SHRINK;
		    GROW;
		    cur = CUR_CHAR(l);
		}
	    }
	    buf[len] = 0;
	    if (cur != '?') {
		xmlFatalErrMsgStr(ctxt, XML_ERR_PI_NOT_FINISHED,
		      "ParsePI: PI %s never end ...\n", target);
	    } else {
		if (input != ctxt->input) {
		    xmlFatalErrMsg(ctxt, XML_ERR_SPACE_REQUIRED,
	    "PI declaration doesn't start and stop in the same entity\n");
		}
		SKIP(2);

#ifdef LIBXML_CATALOG_ENABLED
		if (((state == XML_PARSER_MISC) ||
	             (state == XML_PARSER_START)) &&
		    (xmlStrEqual(target, XML_CATALOG_PI))) {
		    xmlCatalogAllow allow = xmlCatalogGetDefaults();
		    if ((allow == XML_CATA_ALLOW_DOCUMENT) ||
			(allow == XML_CATA_ALLOW_ALL))
			xmlParseCatalogPI(ctxt, buf);
		}
#endif


		/*
		 * SAX: PI detected.
		 */
		if ((ctxt->sax) && (!ctxt->disableSAX) &&
		    (ctxt->sax->processingInstruction != NULL))
		    ctxt->sax->processingInstruction(ctxt->userData,
		                                     target, buf);
	    }
	    xmlFree(buf);
 	} else {
 	    xmlFatalErr(ctxt, XML_ERR_PI_NOT_STARTED, NULL);
 	}
	if (ctxt->instate != XML_PARSER_EOF)
	    ctxt->instate = state;
     }
 }
