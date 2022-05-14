xmlParseStartTag(xmlParserCtxtPtr ctxt) {
    const xmlChar *name;
    const xmlChar *attname;
    xmlChar *attvalue;
    const xmlChar **atts = ctxt->atts;
    int nbatts = 0;
    int maxatts = ctxt->maxatts;
    int i;

    if (RAW != '<') return(NULL);
    NEXT1;

    name = xmlParseName(ctxt);
    if (name == NULL) {
	xmlFatalErrMsg(ctxt, XML_ERR_NAME_REQUIRED,
	     "xmlParseStartTag: invalid element name\n");
        return(NULL);
    }

    /*
     * Now parse the attributes, it ends up with the ending
     *
     * (S Attribute)* S?
     */
     SKIP_BLANKS;
     GROW;
 
    while (((RAW != '>') && 
 	   ((RAW != '/') || (NXT(1) != '>')) &&
	   (IS_BYTE_CHAR(RAW))) && (ctxt->instate != XML_PARSER_EOF)) {
 	const xmlChar *q = CUR_PTR;
 	unsigned int cons = ctxt->input->consumed;
 
	attname = xmlParseAttribute(ctxt, &attvalue);
        if ((attname != NULL) && (attvalue != NULL)) {
	    /*
	     * [ WFC: Unique Att Spec ]
	     * No attribute name may appear more than once in the same
	     * start-tag or empty-element tag. 
	     */
	    for (i = 0; i < nbatts;i += 2) {
	        if (xmlStrEqual(atts[i], attname)) {
		    xmlErrAttributeDup(ctxt, NULL, attname);
		    xmlFree(attvalue);
		    goto failed;
		}
	    }
	    /*
	     * Add the pair to atts
	     */
	    if (atts == NULL) {
	        maxatts = 22; /* allow for 10 attrs by default */
	        atts = (const xmlChar **)
		       xmlMalloc(maxatts * sizeof(xmlChar *));
		if (atts == NULL) {
		    xmlErrMemory(ctxt, NULL);
		    if (attvalue != NULL)
			xmlFree(attvalue);
		    goto failed;
		}
		ctxt->atts = atts;
		ctxt->maxatts = maxatts;
	    } else if (nbatts + 4 > maxatts) {
	        const xmlChar **n;

	        maxatts *= 2;
	        n = (const xmlChar **) xmlRealloc((void *) atts,
					     maxatts * sizeof(const xmlChar *));
		if (n == NULL) {
		    xmlErrMemory(ctxt, NULL);
		    if (attvalue != NULL)
			xmlFree(attvalue);
		    goto failed;
		}
		atts = n;
		ctxt->atts = atts;
		ctxt->maxatts = maxatts;
	    }
	    atts[nbatts++] = attname;
	    atts[nbatts++] = attvalue;
	    atts[nbatts] = NULL;
	    atts[nbatts + 1] = NULL;
	} else {
	    if (attvalue != NULL)
		xmlFree(attvalue);
	}

failed:     

	GROW
	if ((RAW == '>') || (((RAW == '/') && (NXT(1) == '>'))))
	    break;
	if (!IS_BLANK_CH(RAW)) {
	    xmlFatalErrMsg(ctxt, XML_ERR_SPACE_REQUIRED,
			   "attributes construct error\n");
	}
	SKIP_BLANKS;
        if ((cons == ctxt->input->consumed) && (q == CUR_PTR) &&
            (attname == NULL) && (attvalue == NULL)) {
	    xmlFatalErrMsg(ctxt, XML_ERR_INTERNAL_ERROR,
			   "xmlParseStartTag: problem parsing attributes\n");
	    break;
	}
	SHRINK;
        GROW;
    }

    /*
     * SAX: Start of Element !
     */
    if ((ctxt->sax != NULL) && (ctxt->sax->startElement != NULL) &&
	(!ctxt->disableSAX)) {
	if (nbatts > 0)
	    ctxt->sax->startElement(ctxt->userData, name, atts);
	else
	    ctxt->sax->startElement(ctxt->userData, name, NULL);
    }

    if (atts != NULL) {
        /* Free only the content strings */
        for (i = 1;i < nbatts;i+=2)
	    if (atts[i] != NULL)
	       xmlFree((xmlChar *) atts[i]);
    }
    return(name);
}
