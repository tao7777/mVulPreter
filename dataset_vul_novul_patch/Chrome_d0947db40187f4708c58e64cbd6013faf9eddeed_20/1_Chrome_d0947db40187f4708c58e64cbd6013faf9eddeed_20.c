xmlParseEntityValue(xmlParserCtxtPtr ctxt, xmlChar **orig) {
    xmlChar *buf = NULL;
    int len = 0;
    int size = XML_PARSER_BUFFER_SIZE;
    int c, l;
    xmlChar stop;
    xmlChar *ret = NULL;
    const xmlChar *cur = NULL;
    xmlParserInputPtr input;

    if (RAW == '"') stop = '"';
    else if (RAW == '\'') stop = '\'';
    else {
	xmlFatalErr(ctxt, XML_ERR_ENTITY_NOT_STARTED, NULL);
	return(NULL);
    }
    buf = (xmlChar *) xmlMallocAtomic(size * sizeof(xmlChar));
    if (buf == NULL) {
	xmlErrMemory(ctxt, NULL);
	return(NULL);
    }

    /*
     * The content of the entity definition is copied in a buffer.
     */

     ctxt->instate = XML_PARSER_ENTITY_VALUE;
     input = ctxt->input;
     GROW;
     NEXT;
     c = CUR_CHAR(l);
     /*
     * NOTE: 4.4.5 Included in Literal
     * When a parameter entity reference appears in a literal entity
     * value, ... a single or double quote character in the replacement
     * text is always treated as a normal data character and will not
     * terminate the literal. 
      * In practice it means we stop the loop only when back at parsing
      * the initial entity and the quote is found
      */
    while ((IS_CHAR(c)) && ((c != stop) || /* checked */
	   (ctxt->input != input))) {
 	if (len + 5 >= size) {
 	    xmlChar *tmp;
 
	    size *= 2;
	    tmp = (xmlChar *) xmlRealloc(buf, size * sizeof(xmlChar));
	    if (tmp == NULL) {
		xmlErrMemory(ctxt, NULL);
		xmlFree(buf);
		return(NULL);
	    }
	    buf = tmp;
	}
	COPY_BUF(l,buf,len,c);
	NEXTL(l);
	/*
	 * Pop-up of finished entities.
	 */
	while ((RAW == 0) && (ctxt->inputNr > 1)) /* non input consuming */
	    xmlPopInput(ctxt);

	GROW;
	c = CUR_CHAR(l);
	if (c == 0) {
	    GROW;
	    c = CUR_CHAR(l);
 	}
     }
     buf[len] = 0;
 
     /*
      * Raise problem w.r.t. '&' and '%' being used in non-entities
     * reference constructs. Note Charref will be handled in
     * xmlStringDecodeEntities()
     */
    cur = buf;
    while (*cur != 0) { /* non input consuming */
	if ((*cur == '%') || ((*cur == '&') && (cur[1] != '#'))) {
	    xmlChar *name;
	    xmlChar tmp = *cur;

	    cur++;
	    name = xmlParseStringName(ctxt, &cur);
            if ((name == NULL) || (*cur != ';')) {
		xmlFatalErrMsgInt(ctxt, XML_ERR_ENTITY_CHAR_ERROR,
	    "EntityValue: '%c' forbidden except for entities references\n",
	                          tmp);
	    }
	    if ((tmp == '%') && (ctxt->inSubset == 1) &&
		(ctxt->inputNr == 1)) {
		xmlFatalErr(ctxt, XML_ERR_ENTITY_PE_INTERNAL, NULL);
	    }
	    if (name != NULL)
		xmlFree(name);
	    if (*cur == 0)
	        break;
	}
	cur++;
    }

    /*
     * Then PEReference entities are substituted.
     */
    if (c != stop) {
	xmlFatalErr(ctxt, XML_ERR_ENTITY_NOT_FINISHED, NULL);
	xmlFree(buf);
    } else {
	NEXT;
	/*
	 * NOTE: 4.4.7 Bypassed
	 * When a general entity reference appears in the EntityValue in
	 * an entity declaration, it is bypassed and left as is.
	 * so XML_SUBSTITUTE_REF is not set here.
 	 */
 	ret = xmlStringDecodeEntities(ctxt, buf, XML_SUBSTITUTE_PEREF,
 				      0, 0, 0);
	if (orig != NULL) 
 	    *orig = buf;
 	else
 	    xmlFree(buf);
     }
     return(ret);
 }
