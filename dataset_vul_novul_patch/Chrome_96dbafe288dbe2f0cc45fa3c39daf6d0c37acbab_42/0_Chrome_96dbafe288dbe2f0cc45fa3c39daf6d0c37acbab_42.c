xsltNumber(xsltTransformContextPtr ctxt, xmlNodePtr node,
	   xmlNodePtr inst, xsltStylePreCompPtr castedComp)
{
#ifdef XSLT_REFACTORED
    xsltStyleItemNumberPtr comp = (xsltStyleItemNumberPtr) castedComp;
 #else
     xsltStylePreCompPtr comp = castedComp;
 #endif
    xmlXPathContextPtr xpctxt;
    xmlNsPtr *oldXPNamespaces;
    int oldXPNsNr;

     if (comp == NULL) {
 	xsltTransformError(ctxt, NULL, inst,
 	     "xsl:number : compilation failed\n");
	return;
    }

    if ((ctxt == NULL) || (node == NULL) || (inst == NULL) || (comp == NULL))
	return;

     comp->numdata.doc = inst->doc;
     comp->numdata.node = inst;
 
    xpctxt = ctxt->xpathCtxt;
    oldXPNsNr = xpctxt->nsNr;
    oldXPNamespaces = xpctxt->namespaces;

#ifdef XSLT_REFACTORED
    if (comp->inScopeNs != NULL) {
        xpctxt->namespaces = comp->inScopeNs->list;
        xpctxt->nsNr = comp->inScopeNs->xpathNumber;
    } else {
        xpctxt->namespaces = NULL;
        xpctxt->nsNr = 0;
    }
#else
    xpctxt->namespaces = comp->nsList;
    xpctxt->nsNr = comp->nsNr;
#endif

     xsltNumberFormat(ctxt, &comp->numdata, node);

    xpctxt->nsNr = oldXPNsNr;
    xpctxt->namespaces = oldXPNamespaces;
 }
