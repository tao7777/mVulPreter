xmlXPathCompOpEvalPositionalPredicate(xmlXPathParserContextPtr ctxt,
				      xmlXPathStepOpPtr op,
				      xmlNodeSetPtr set,
				      int contextSize,
				      int minPos,
				      int maxPos,
				      int hasNsNodes)
{
    if (op->ch1 != -1) {
	xmlXPathCompExprPtr comp = ctxt->comp;
	if (comp->steps[op->ch1].op != XPATH_OP_PREDICATE) {
	    /*
	    * TODO: raise an internal error.
	    */
	}
	contextSize = xmlXPathCompOpEvalPredicate(ctxt,
	    &comp->steps[op->ch1], set, contextSize, hasNsNodes);
	CHECK_ERROR0;
	if (contextSize <= 0)
	    return(0);
    }
    /*
    * Check if the node set contains a sufficient number of nodes for
    * the requested range.
    */
    if (contextSize < minPos) {
	xmlXPathNodeSetClear(set, hasNsNodes);
	return(0);
    }
    if (op->ch2 == -1) {
	/*
	* TODO: Can this ever happen?
	*/
	return (contextSize);
    } else {
	xmlDocPtr oldContextDoc;
	int i, pos = 0, newContextSize = 0, contextPos = 0, res;
	xmlXPathStepOpPtr exprOp;
	xmlXPathObjectPtr contextObj = NULL, exprRes = NULL;
	xmlNodePtr oldContextNode, contextNode = NULL;
	xmlXPathContextPtr xpctxt = ctxt->context;

#ifdef LIBXML_XPTR_ENABLED
	    /*
	    * URGENT TODO: Check the following:
	    *  We don't expect location sets if evaluating prediates, right?
	    *  Only filters should expect location sets, right?
	*/
#endif /* LIBXML_XPTR_ENABLED */

	/*
	* Save old context.
	*/
	oldContextNode = xpctxt->node;
	oldContextDoc = xpctxt->doc;
	/*
	* Get the expression of this predicate.
	*/
	exprOp = &ctxt->comp->steps[op->ch2];
	for (i = 0; i < set->nodeNr; i++) {
	    if (set->nodeTab[i] == NULL)
		continue;

	    contextNode = set->nodeTab[i];
	    xpctxt->node = contextNode;
	    xpctxt->contextSize = contextSize;
	    xpctxt->proximityPosition = ++contextPos;

	    /*
	    * Initialize the new set.
	    * Also set the xpath document in case things like
	    * key() evaluation are attempted on the predicate
	    */
	    if ((contextNode->type != XML_NAMESPACE_DECL) &&
		(contextNode->doc != NULL))
		xpctxt->doc = contextNode->doc;
	    /*
	    * Evaluate the predicate expression with 1 context node
	    * at a time; this node is packaged into a node set; this
	    * node set is handed over to the evaluation mechanism.
	    */
	    if (contextObj == NULL)
		contextObj = xmlXPathCacheNewNodeSet(xpctxt, contextNode);
	    else
		xmlXPathNodeSetAddUnique(contextObj->nodesetval,
		    contextNode);

	    valuePush(ctxt, contextObj);
	    res = xmlXPathCompOpEvalToBoolean(ctxt, exprOp, 1);

	    if ((ctxt->error != XPATH_EXPRESSION_OK) || (res == -1)) {
 	        xmlXPathObjectPtr tmp;
 		/* pop the result if any */
 		tmp = valuePop(ctxt);
                if (tmp != contextObj)
                     /*
                      * Free up the result
                      * then pop off contextObj, which will be freed later
                      */
                     xmlXPathReleaseObject(xpctxt, tmp);
                    valuePop(ctxt);
 		goto evaluation_error;
 	    }
 
	    if (res)
		pos++;

	    if (res && (pos >= minPos) && (pos <= maxPos)) {
		/*
		* Fits in the requested range.
		*/
		newContextSize++;
		if (minPos == maxPos) {
		    /*
		    * Only 1 node was requested.
		    */
		    if (contextNode->type == XML_NAMESPACE_DECL) {
			/*
			* As always: take care of those nasty
			* namespace nodes.
			*/
			set->nodeTab[i] = NULL;
		    }
		    xmlXPathNodeSetClear(set, hasNsNodes);
		    set->nodeNr = 1;
		    set->nodeTab[0] = contextNode;
		    goto evaluation_exit;
		}
		if (pos == maxPos) {
		    /*
		    * We are done.
		    */
		    xmlXPathNodeSetClearFromPos(set, i +1, hasNsNodes);
		    goto evaluation_exit;
		}
	    } else {
		/*
		* Remove the entry from the initial node set.
		*/
		set->nodeTab[i] = NULL;
		if (contextNode->type == XML_NAMESPACE_DECL)
		    xmlXPathNodeSetFreeNs((xmlNsPtr) contextNode);
	    }
	    if (exprRes != NULL) {
		xmlXPathReleaseObject(ctxt->context, exprRes);
		exprRes = NULL;
	    }
	    if (ctxt->value == contextObj) {
		/*
		* Don't free the temporary XPath object holding the
		* context node, in order to avoid massive recreation
		* inside this loop.
		*/
		valuePop(ctxt);
		xmlXPathNodeSetClear(contextObj->nodesetval, hasNsNodes);
	    } else {
		/*
		* The object was lost in the evaluation machinery.
		* Can this happen? Maybe in case of internal-errors.
		*/
		contextObj = NULL;
	    }
	}
	goto evaluation_exit;

evaluation_error:
	xmlXPathNodeSetClear(set, hasNsNodes);
	newContextSize = 0;

evaluation_exit:
	if (contextObj != NULL) {
	    if (ctxt->value == contextObj)
		valuePop(ctxt);
	    xmlXPathReleaseObject(xpctxt, contextObj);
	}
	if (exprRes != NULL)
	    xmlXPathReleaseObject(ctxt->context, exprRes);
	/*
	* Reset/invalidate the context.
	*/
	xpctxt->node = oldContextNode;
	xpctxt->doc = oldContextDoc;
	xpctxt->contextSize = -1;
	xpctxt->proximityPosition = -1;
	return(newContextSize);
    }
    return(contextSize);
}
