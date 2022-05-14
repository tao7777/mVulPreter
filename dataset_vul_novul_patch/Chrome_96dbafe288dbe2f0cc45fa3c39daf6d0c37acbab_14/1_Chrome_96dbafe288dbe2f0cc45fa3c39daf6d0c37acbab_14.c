xsltDocumentFunctionLoadDocument(xmlXPathParserContextPtr ctxt, xmlChar* URI)
{
    xsltTransformContextPtr tctxt;
    xmlURIPtr uri;
    xmlChar *fragment;
    xsltDocumentPtr idoc; /* document info */
    xmlDocPtr doc;
    xmlXPathContextPtr xptrctxt = NULL;
    xmlXPathObjectPtr resObj = NULL;

    tctxt = xsltXPathGetTransformContext(ctxt);
    if (tctxt == NULL) {
	xsltTransformError(NULL, NULL, NULL,
	    "document() : internal error tctxt == NULL\n");
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
    }

    uri = xmlParseURI((const char *) URI);
    if (uri == NULL) {
	xsltTransformError(tctxt, NULL, NULL,
	    "document() : failed to parse URI\n");
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
    }

    /*
     * check for and remove fragment identifier
     */
    fragment = (xmlChar *)uri->fragment;
    if (fragment != NULL) {
        xmlChar *newURI;
	uri->fragment = NULL;
	newURI = xmlSaveUri(uri);
	idoc = xsltLoadDocument(tctxt, newURI);
	xmlFree(newURI);
    } else
	idoc = xsltLoadDocument(tctxt, URI);
    xmlFreeURI(uri);

    if (idoc == NULL) {
	if ((URI == NULL) ||
	    (URI[0] == '#') ||
	    ((tctxt->style->doc != NULL) &&
	    (xmlStrEqual(tctxt->style->doc->URL, URI))))
	{
	    /*
	    * This selects the stylesheet's doc itself.
	    */
	    doc = tctxt->style->doc;
	} else {
	    valuePush(ctxt, xmlXPathNewNodeSet(NULL));

	    if (fragment != NULL)
		xmlFree(fragment);

	    return;
	}
    } else
	doc = idoc->doc;

    if (fragment == NULL) {
	valuePush(ctxt, xmlXPathNewNodeSet((xmlNodePtr) doc));
	return;
    }

    /* use XPointer of HTML location for fragment ID */
#ifdef LIBXML_XPTR_ENABLED
    xptrctxt = xmlXPtrNewContext(doc, NULL, NULL);
    if (xptrctxt == NULL) {
	xsltTransformError(tctxt, NULL, NULL,
	    "document() : internal error xptrctxt == NULL\n");
	goto out_fragment;
    }

     resObj = xmlXPtrEval(fragment, xptrctxt);
     xmlXPathFreeContext(xptrctxt);
 #endif
    xmlFree(fragment);
 
     if (resObj == NULL)
 	goto out_fragment;

    switch (resObj->type) {
	case XPATH_NODESET:
	    break;
	case XPATH_UNDEFINED:
	case XPATH_BOOLEAN:
	case XPATH_NUMBER:
	case XPATH_STRING:
	case XPATH_POINT:
	case XPATH_USERS:
	case XPATH_XSLT_TREE:
	case XPATH_RANGE:
	case XPATH_LOCATIONSET:
	    xsltTransformError(tctxt, NULL, NULL,
		"document() : XPointer does not select a node set: #%s\n",
		fragment);
	goto out_object;
     }
 
     valuePush(ctxt, resObj);
     return;
 
 out_object:
     xmlXPathFreeObject(resObj);
 
 out_fragment:
     valuePush(ctxt, xmlXPathNewNodeSet(NULL));
 }
