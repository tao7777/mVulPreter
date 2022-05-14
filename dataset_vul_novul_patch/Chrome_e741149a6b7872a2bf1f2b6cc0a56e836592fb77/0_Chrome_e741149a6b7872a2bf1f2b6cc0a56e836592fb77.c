 xsltGenerateIdFunction(xmlXPathParserContextPtr ctxt, int nargs){
     xmlNodePtr cur = NULL;
    xmlXPathObjectPtr obj = NULL;
     long val;
     xmlChar str[30];
     xmlDocPtr doc;
 
     if (nargs == 0) {
 	cur = ctxt->context->node;
     } else if (nargs == 1) {
 	xmlNodeSetPtr nodelist;
 	int i, ret;
 
	if ((ctxt->value == NULL) || (ctxt->value->type != XPATH_NODESET)) {
	    ctxt->error = XPATH_INVALID_TYPE;
	    xsltTransformError(xsltXPathGetTransformContext(ctxt), NULL, NULL,
		"generate-id() : invalid arg expecting a node-set\n");
	    return;
	}
	obj = valuePop(ctxt);
	nodelist = obj->nodesetval;
	if ((nodelist == NULL) || (nodelist->nodeNr <= 0)) {
	    xmlXPathFreeObject(obj);
	    valuePush(ctxt, xmlXPathNewCString(""));
	    return;
	}
	cur = nodelist->nodeTab[0];
	for (i = 1;i < nodelist->nodeNr;i++) {
	    ret = xmlXPathCmpNodes(cur, nodelist->nodeTab[i]);
 	    if (ret == -1)
 	        cur = nodelist->nodeTab[i];
 	}
     } else {
 	xsltTransformError(xsltXPathGetTransformContext(ctxt), NULL, NULL,
 		"generate-id() : invalid number of args %d\n", nargs);
	ctxt->error = XPATH_INVALID_ARITY;
	return;
    }
    /*
     * Okay this is ugly but should work, use the NodePtr address
     * to forge the ID
     */
    if (cur->type != XML_NAMESPACE_DECL)
        doc = cur->doc;
    else {
        xmlNsPtr ns = (xmlNsPtr) cur;

        if (ns->context != NULL)
            doc = ns->context;
        else
            doc = ctxt->context->doc;
 
     }
 
    if (obj)
        xmlXPathFreeObject(obj);

     val = (long)((char *)cur - (char *)doc);
     if (val >= 0) {
       sprintf((char *)str, "idp%ld", val);
    } else {
      sprintf((char *)str, "idm%ld", -val);
    }
    valuePush(ctxt, xmlXPathNewString(str));
}
