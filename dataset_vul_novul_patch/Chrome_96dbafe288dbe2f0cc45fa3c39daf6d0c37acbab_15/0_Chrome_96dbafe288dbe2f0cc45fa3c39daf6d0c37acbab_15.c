xsltGenerateIdFunction(xmlXPathParserContextPtr ctxt, int nargs){
    static char base_address;
    xmlNodePtr cur = NULL;
     xmlXPathObjectPtr obj = NULL;
     long val;
     xmlChar str[30];
 
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
 
     if (obj)
         xmlXPathFreeObject(obj);
 
     val = (long)((char *)cur - (char *)&base_address);
     if (val >= 0) {
      snprintf((char *)str, sizeof(str), "idp%ld", val);
     } else {
      snprintf((char *)str, sizeof(str), "idm%ld", -val);
     }
     valuePush(ctxt, xmlXPathNewString(str));
 }
