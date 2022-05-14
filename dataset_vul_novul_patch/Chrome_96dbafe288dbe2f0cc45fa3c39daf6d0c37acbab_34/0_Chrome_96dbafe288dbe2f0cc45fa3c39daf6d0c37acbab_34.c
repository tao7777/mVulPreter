xsltChoose(xsltTransformContextPtr ctxt, xmlNodePtr contextNode,
	   xmlNodePtr inst, xsltStylePreCompPtr comp ATTRIBUTE_UNUSED)
{
    xmlNodePtr cur;

    if ((ctxt == NULL) || (contextNode == NULL) || (inst == NULL))
	return;

    /*
    * TODO: Content model checks should be done only at compilation
    * time.
    */
    cur = inst->children;
    if (cur == NULL) {
	xsltTransformError(ctxt, NULL, inst,
	    "xsl:choose: The instruction has no content.\n");
	return;
    }

#ifdef XSLT_REFACTORED
    /*
    * We don't check the content model during transformation.
    */
#else
    if ((! IS_XSLT_ELEM(cur)) || (! IS_XSLT_NAME(cur, "when"))) {
	xsltTransformError(ctxt, NULL, inst,
	     "xsl:choose: xsl:when expected first\n");
	return;
    }
#endif
 
     {
 	int testRes = 0, res = 0;
 
 #ifdef XSLT_REFACTORED
 	xsltStyleItemWhenPtr wcomp = NULL;
#else
	xsltStylePreCompPtr wcomp = NULL;
#endif

	/*
	* Process xsl:when ---------------------------------------------------
	*/
	while (IS_XSLT_ELEM(cur) && IS_XSLT_NAME(cur, "when")) {
	    wcomp = cur->psvi;

	    if ((wcomp == NULL) || (wcomp->test == NULL) ||
		(wcomp->comp == NULL))
	    {
		xsltTransformError(ctxt, NULL, cur,
		    "Internal error in xsltChoose(): "
		    "The XSLT 'when' instruction was not compiled.\n");
		goto error;
	    }


#ifdef WITH_DEBUGGER
	    if (xslDebugStatus != XSLT_DEBUG_NONE) {
		/*
		* TODO: Isn't comp->templ always NULL for xsl:choose?
		*/
		xslHandleDebugger(cur, contextNode, NULL, ctxt);
	    }
#endif
#ifdef WITH_XSLT_DEBUG_PROCESS
	    XSLT_TRACE(ctxt,XSLT_TRACE_CHOOSE,xsltGenericDebug(xsltGenericDebugContext,
 		"xsltChoose: test %s\n", wcomp->test));
 #endif
 
 #ifdef XSLT_FAST_IF
	    res = xsltPreCompEvalToBoolean(ctxt, contextNode, wcomp);
 
 	    if (res == -1) {
 		ctxt->state = XSLT_STATE_STOPPED;
		goto error;
	    }
	    testRes = (res == 1) ? 1 : 0;
 
 #else /* XSLT_FAST_IF */
 
	    res = xsltPreCompEval(ctxt, cotextNode, wcomp);
 
 	    if (res != NULL) {
 		if (res->type != XPATH_BOOLEAN)
		    res = xmlXPathConvertBoolean(res);
		if (res->type == XPATH_BOOLEAN)
		    testRes = res->boolval;
		else {
#ifdef WITH_XSLT_DEBUG_PROCESS
		    XSLT_TRACE(ctxt,XSLT_TRACE_CHOOSE,xsltGenericDebug(xsltGenericDebugContext,
			"xsltChoose: test didn't evaluate to a boolean\n"));
#endif
		    goto error;
		}
		xmlXPathFreeObject(res);
		res = NULL;
	    } else {
		ctxt->state = XSLT_STATE_STOPPED;
		goto error;
	    }

#endif /* else of XSLT_FAST_IF */

#ifdef WITH_XSLT_DEBUG_PROCESS
	    XSLT_TRACE(ctxt,XSLT_TRACE_CHOOSE,xsltGenericDebug(xsltGenericDebugContext,
		"xsltChoose: test evaluate to %d\n", testRes));
#endif
	    if (testRes)
		goto test_is_true;

	    cur = cur->next;
	}

	/*
	* Process xsl:otherwise ----------------------------------------------
	*/
	if (IS_XSLT_ELEM(cur) && IS_XSLT_NAME(cur, "otherwise")) {

#ifdef WITH_DEBUGGER
	    if (xslDebugStatus != XSLT_DEBUG_NONE)
		xslHandleDebugger(cur, contextNode, NULL, ctxt);
#endif

#ifdef WITH_XSLT_DEBUG_PROCESS
	    XSLT_TRACE(ctxt,XSLT_TRACE_CHOOSE,xsltGenericDebug(xsltGenericDebugContext,
		"evaluating xsl:otherwise\n"));
 #endif
 	    goto test_is_true;
 	}
 	goto exit;
 
 test_is_true:
 
 	goto process_sequence;
     }
 
process_sequence:

    /*
    * Instantiate the sequence constructor.
    */
    xsltApplySequenceConstructor(ctxt, ctxt->node, cur->children,
	NULL);

exit:
error:
    return;
}
