xsltInitCtxtKey(xsltTransformContextPtr ctxt, xsltDocumentPtr idoc,
	        xsltKeyDefPtr keyDef)
{
    int i, len, k;
    xmlNodeSetPtr matchList = NULL, keylist;
    xmlXPathObjectPtr matchRes = NULL, useRes = NULL;
    xmlChar *str = NULL;
    xsltKeyTablePtr table;
    xmlNodePtr oldInst, cur;
    xmlNodePtr oldContextNode;
    xsltDocumentPtr oldDocInfo;
    int	oldXPPos, oldXPSize;
    xmlDocPtr oldXPDoc;
    int oldXPNsNr;
    xmlNsPtr *oldXPNamespaces;
    xmlXPathContextPtr xpctxt;

#ifdef KEY_INIT_DEBUG
fprintf(stderr, "xsltInitCtxtKey %s : %d\n", keyDef->name, ctxt->keyInitLevel);
#endif

    if ((keyDef->comp == NULL) || (keyDef->usecomp == NULL))
	return(-1);

    /*
     * Detect recursive keys
     */
    if (ctxt->keyInitLevel > ctxt->nbKeys) {
#ifdef WITH_XSLT_DEBUG_KEYS
	XSLT_TRACE(ctxt,XSLT_TRACE_KEYS,
	           xsltGenericDebug(xsltGenericDebugContext,
		       "xsltInitCtxtKey: key definition of %s is recursive\n",
		       keyDef->name));
#endif
	xsltTransformError(ctxt, NULL, keyDef->inst,
	    "Key definition for %s is recursive\n", keyDef->name);
	ctxt->state = XSLT_STATE_STOPPED;
        return(-1);
    }
    ctxt->keyInitLevel++;

    xpctxt = ctxt->xpathCtxt;
    idoc->nbKeysComputed++;
    /*
    * Save context state.
    */
    oldInst = ctxt->inst;
    oldDocInfo = ctxt->document;
    oldContextNode = ctxt->node;

    oldXPDoc = xpctxt->doc;
    oldXPPos = xpctxt->proximityPosition;
    oldXPSize = xpctxt->contextSize;
    oldXPNsNr = xpctxt->nsNr;
    oldXPNamespaces = xpctxt->namespaces;

    /*
    * Set up contexts.
    */
    ctxt->document = idoc;
    ctxt->node = (xmlNodePtr) idoc->doc;
    ctxt->inst = keyDef->inst;

    xpctxt->doc = idoc->doc;
    xpctxt->node = (xmlNodePtr) idoc->doc;
    /* TODO : clarify the use of namespaces in keys evaluation */
    xpctxt->namespaces = keyDef->nsList;
    xpctxt->nsNr = keyDef->nsNr;

    /*
    * Evaluate the 'match' expression of the xsl:key.
    * TODO: The 'match' is a *pattern*.
    */
    matchRes = xmlXPathCompiledEval(keyDef->comp, xpctxt);
    if (matchRes == NULL) {

#ifdef WITH_XSLT_DEBUG_KEYS
	XSLT_TRACE(ctxt,XSLT_TRACE_KEYS,xsltGenericDebug(xsltGenericDebugContext,
	     "xsltInitCtxtKey: %s evaluation failed\n", keyDef->match));
#endif
	xsltTransformError(ctxt, NULL, keyDef->inst,
	    "Failed to evaluate the 'match' expression.\n");
	ctxt->state = XSLT_STATE_STOPPED;
	goto error;
    } else {
	if (matchRes->type == XPATH_NODESET) {
	    matchList = matchRes->nodesetval;

#ifdef WITH_XSLT_DEBUG_KEYS
	    if (matchList != NULL)
		XSLT_TRACE(ctxt,XSLT_TRACE_KEYS,xsltGenericDebug(xsltGenericDebugContext,
		     "xsltInitCtxtKey: %s evaluates to %d nodes\n",
				 keyDef->match, matchList->nodeNr));
#endif
	} else {
	    /*
	    * Is not a node set, but must be.
	    */
#ifdef WITH_XSLT_DEBUG_KEYS
	    XSLT_TRACE(ctxt,XSLT_TRACE_KEYS,xsltGenericDebug(xsltGenericDebugContext,
		 "xsltInitCtxtKey: %s is not a node set\n", keyDef->match));
#endif
	    xsltTransformError(ctxt, NULL, keyDef->inst,
		"The 'match' expression did not evaluate to a node set.\n");
	    ctxt->state = XSLT_STATE_STOPPED;
	    goto error;
	}
    }
    if ((matchList == NULL) || (matchList->nodeNr <= 0))
	goto exit;

    /**
     * Multiple key definitions for the same name are allowed, so
     * we must check if the key is already present for this doc
     */
    table = (xsltKeyTablePtr) idoc->keys;
    while (table != NULL) {
        if (xmlStrEqual(table->name, keyDef->name) &&
	    (((keyDef->nameURI == NULL) && (table->nameURI == NULL)) ||
	     ((keyDef->nameURI != NULL) && (table->nameURI != NULL) &&
	      (xmlStrEqual(table->nameURI, keyDef->nameURI)))))
	    break;
	table = table->next;
    }
    /**
     * If the key was not previously defined, create it now and
     * chain it to the list of keys for the doc
     */
    if (table == NULL) {
        table = xsltNewKeyTable(keyDef->name, keyDef->nameURI);
        if (table == NULL)
	    goto error;
        table->next = idoc->keys;
        idoc->keys = table;
    }

    /*
    * SPEC XSLT 1.0 (XSLT 2.0 does not clarify the context size!)
    * "...the use attribute of the xsl:key element is evaluated with x as
    "  the current node and with a node list containing just x as the
    *  current node list"
    */
    xpctxt->contextSize = 1;
    xpctxt->proximityPosition = 1;

    for (i = 0; i < matchList->nodeNr; i++) {
 	cur = matchList->nodeTab[i];
 	if (! IS_XSLT_REAL_NODE(cur))
 	    continue;
 	xpctxt->node = cur;
 	/*
 	* Process the 'use' of the xsl:key.
	* SPEC XSLT 1.0:
	* "The use attribute is an expression specifying the values of
	*  the key; the expression is evaluated once for each node that
	*  matches the pattern."
	*/
	if (useRes != NULL)
	    xmlXPathFreeObject(useRes);
	useRes = xmlXPathCompiledEval(keyDef->usecomp, xpctxt);
	if (useRes == NULL) {
	    xsltTransformError(ctxt, NULL, keyDef->inst,
		"Failed to evaluate the 'use' expression.\n");
	    ctxt->state = XSLT_STATE_STOPPED;
	    break;
	}
	if (useRes->type == XPATH_NODESET) {
	    if ((useRes->nodesetval != NULL) &&
		(useRes->nodesetval->nodeNr != 0))
	    {
		len = useRes->nodesetval->nodeNr;
		str = xmlXPathCastNodeToString(useRes->nodesetval->nodeTab[0]);
	    } else {
		continue;
	    }
	} else {
	    len = 1;
	    if (useRes->type == XPATH_STRING) {
		/*
		* Consume the string value.
		*/
		str = useRes->stringval;
		useRes->stringval = NULL;
	    } else {
		str = xmlXPathCastToString(useRes);
	    }
	}
	/*
	* Process all strings.
	*/
	k = 0;
	while (1) {
	    if (str == NULL)
		goto next_string;

#ifdef WITH_XSLT_DEBUG_KEYS
	    XSLT_TRACE(ctxt,XSLT_TRACE_KEYS,xsltGenericDebug(xsltGenericDebugContext,
		"xsl:key : node associated to ('%s', '%s')\n", keyDef->name, str));
#endif

	    keylist = xmlHashLookup(table->keys, str);
	    if (keylist == NULL) {
		keylist = xmlXPathNodeSetCreate(cur);
		if (keylist == NULL)
		    goto error;
		xmlHashAddEntry(table->keys, str, keylist);
	    } else {
		/*
		* TODO: How do we know if this function failed?
		*/
		xmlXPathNodeSetAdd(keylist, cur);
	    }
	    switch (cur->type) {
		case XML_ELEMENT_NODE:
		case XML_TEXT_NODE:
		case XML_CDATA_SECTION_NODE:
		case XML_PI_NODE:
		case XML_COMMENT_NODE:
		    cur->psvi = keyDef;
		    break;
		case XML_ATTRIBUTE_NODE:
		    ((xmlAttrPtr) cur)->psvi = keyDef;
		    break;
		case XML_DOCUMENT_NODE:
		case XML_HTML_DOCUMENT_NODE:
		    ((xmlDocPtr) cur)->psvi = keyDef;
		    break;
		default:
		    break;
	    }
	    xmlFree(str);
	    str = NULL;

next_string:
	    k++;
	    if (k >= len)
		break;
	    str = xmlXPathCastNodeToString(useRes->nodesetval->nodeTab[k]);
	}
    }

exit:
error:
    ctxt->keyInitLevel--;
    /*
    * Restore context state.
    */
    xpctxt->doc = oldXPDoc;
    xpctxt->nsNr = oldXPNsNr;
    xpctxt->namespaces = oldXPNamespaces;
    xpctxt->proximityPosition = oldXPPos;
    xpctxt->contextSize = oldXPSize;

    ctxt->node = oldContextNode;
    ctxt->document = oldDocInfo;
    ctxt->inst = oldInst;

    if (str)
	xmlFree(str);
    if (useRes != NULL)
	xmlXPathFreeObject(useRes);
    if (matchRes != NULL)
	xmlXPathFreeObject(matchRes);
    return(0);
}
