xsltIf(xsltTransformContextPtr ctxt, xmlNodePtr contextNode,
	           xmlNodePtr inst, xsltStylePreCompPtr castedComp)
{
    int res = 0;

#ifdef XSLT_REFACTORED
    xsltStyleItemIfPtr comp = (xsltStyleItemIfPtr) castedComp;
#else
    xsltStylePreCompPtr comp = castedComp;
#endif

    if ((ctxt == NULL) || (contextNode == NULL) || (inst == NULL))
	return;
    if ((comp == NULL) || (comp->test == NULL) || (comp->comp == NULL)) {
	xsltTransformError(ctxt, NULL, inst,
	    "Internal error in xsltIf(): "
	    "The XSLT 'if' instruction was not compiled.\n");
	return;
    }

#ifdef WITH_XSLT_DEBUG_PROCESS
    XSLT_TRACE(ctxt,XSLT_TRACE_IF,xsltGenericDebug(xsltGenericDebugContext,
	 "xsltIf: test %s\n", comp->test));
#endif
 
 #ifdef XSLT_FAST_IF
     {
 	xmlDocPtr oldLocalFragmentTop = ctxt->localRVT;
 
	res = xsltPreCompEvalToBoolean(ctxt, contextNode, comp);
 
 	/*
 	* Cleanup fragments created during evaluation of the
 	* "select" expression.
 	*/
 	if (oldLocalFragmentTop != ctxt->localRVT)
 	    xsltReleaseLocalRVTs(ctxt, oldLocalFragmentTop);
     }
 
 #ifdef WITH_XSLT_DEBUG_PROCESS
    XSLT_TRACE(ctxt,XSLT_TRACE_IF,xsltGenericDebug(xsltGenericDebugContext,
	"xsltIf: test evaluate to %d\n", res));
#endif

    if (res == -1) {
	ctxt->state = XSLT_STATE_STOPPED;
	goto error;
    }
    if (res == 1) {
	/*
	* Instantiate the sequence constructor of xsl:if.
	*/
	xsltApplySequenceConstructor(ctxt,
	    contextNode, inst->children, NULL);
    }
 
 #else /* XSLT_FAST_IF */
     {
 	/*
 	* OLD CODE:
 	*/
	xmlXPathObjectPtr xpobj = xsltPreCompEval(ctxt, contextNode, comp);
 	if (xpobj != NULL) {
 	    if (xpobj->type != XPATH_BOOLEAN)
 		xpobj = xmlXPathConvertBoolean(xpobj);
	    if (xpobj->type == XPATH_BOOLEAN) {
		res = xpobj->boolval;

#ifdef WITH_XSLT_DEBUG_PROCESS
		XSLT_TRACE(ctxt,XSLT_TRACE_IF,xsltGenericDebug(xsltGenericDebugContext,
		    "xsltIf: test evaluate to %d\n", res));
#endif
		if (res) {
		    xsltApplySequenceConstructor(ctxt,
			contextNode, inst->children, NULL);
		}
	    } else {

#ifdef WITH_XSLT_DEBUG_PROCESS
		XSLT_TRACE(ctxt, XSLT_TRACE_IF,
		    xsltGenericDebug(xsltGenericDebugContext,
		    "xsltIf: test didn't evaluate to a boolean\n"));
#endif
		ctxt->state = XSLT_STATE_STOPPED;
	    }
	    xmlXPathFreeObject(xpobj);
	} else {
	    ctxt->state = XSLT_STATE_STOPPED;
	}
    }
#endif /* else of XSLT_FAST_IF */

error:
    return;
}
