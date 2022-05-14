xsltValueOf(xsltTransformContextPtr ctxt, xmlNodePtr node,
	           xmlNodePtr inst, xsltStylePreCompPtr castedComp)
{
#ifdef XSLT_REFACTORED
    xsltStyleItemValueOfPtr comp = (xsltStyleItemValueOfPtr) castedComp;
#else
    xsltStylePreCompPtr comp = castedComp;
 #endif
     xmlXPathObjectPtr res = NULL;
     xmlChar *value = NULL;
    xmlDocPtr oldXPContextDoc;
    xmlNsPtr *oldXPNamespaces;
    xmlNodePtr oldXPContextNode;
    int oldXPProximityPosition, oldXPContextSize, oldXPNsNr;
    xmlXPathContextPtr xpctxt;
 
     if ((ctxt == NULL) || (node == NULL) || (inst == NULL))
 	return;

    if ((comp == NULL) || (comp->select == NULL) || (comp->comp == NULL)) {
	xsltTransformError(ctxt, NULL, inst,
	    "Internal error in xsltValueOf(): "
	    "The XSLT 'value-of' instruction was not compiled.\n");
	return;
    }

#ifdef WITH_XSLT_DEBUG_PROCESS
    XSLT_TRACE(ctxt,XSLT_TRACE_VALUE_OF,xsltGenericDebug(xsltGenericDebugContext,
 	 "xsltValueOf: select %s\n", comp->select));
 #endif
 
    xpctxt = ctxt->xpathCtxt;
    oldXPContextDoc = xpctxt->doc;
    oldXPContextNode = xpctxt->node;
    oldXPProximityPosition = xpctxt->proximityPosition;
    oldXPContextSize = xpctxt->contextSize;
    oldXPNsNr = xpctxt->nsNr;
    oldXPNamespaces = xpctxt->namespaces;
    xpctxt->node = node;
    if (comp != NULL) {
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
    } else {
	xpctxt->namespaces = NULL;
	xpctxt->nsNr = 0;
    }
    res = xmlXPathCompiledEval(comp->comp, xpctxt);
    xpctxt->doc = oldXPContextDoc;
    xpctxt->node = oldXPContextNode;
    xpctxt->contextSize = oldXPContextSize;
    xpctxt->proximityPosition = oldXPProximityPosition;
    xpctxt->nsNr = oldXPNsNr;
    xpctxt->namespaces = oldXPNamespaces;
 
     /*
     * Cast the XPath object to string.
    */
    if (res != NULL) {
	value = xmlXPathCastToString(res);
	if (value == NULL) {
	    xsltTransformError(ctxt, NULL, inst,
		"Internal error in xsltValueOf(): "
		"failed to cast an XPath object to string.\n");
	    ctxt->state = XSLT_STATE_STOPPED;
	    goto error;
	}
	if (value[0] != 0) {
	    xsltCopyTextString(ctxt, ctxt->insert, value, comp->noescape);
	}
    } else {
	xsltTransformError(ctxt, NULL, inst,
	    "XPath evaluation returned no result.\n");
	ctxt->state = XSLT_STATE_STOPPED;
	goto error;
    }

#ifdef WITH_XSLT_DEBUG_PROCESS
    if (value) {
	XSLT_TRACE(ctxt,XSLT_TRACE_VALUE_OF,xsltGenericDebug(xsltGenericDebugContext,
	     "xsltValueOf: result '%s'\n", value));
    }
#endif

error:
    if (value != NULL)
	xmlFree(value);
    if (res != NULL)
	xmlXPathFreeObject(res);
}
