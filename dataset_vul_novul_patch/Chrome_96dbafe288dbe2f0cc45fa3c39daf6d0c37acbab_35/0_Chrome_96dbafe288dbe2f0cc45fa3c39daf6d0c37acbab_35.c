xsltCopyOf(xsltTransformContextPtr ctxt, xmlNodePtr node,
	           xmlNodePtr inst, xsltStylePreCompPtr castedComp) {
#ifdef XSLT_REFACTORED
    xsltStyleItemCopyOfPtr comp = (xsltStyleItemCopyOfPtr) castedComp;
#else
    xsltStylePreCompPtr comp = castedComp;
#endif
     xmlXPathObjectPtr res = NULL;
     xmlNodeSetPtr list = NULL;
     int i;
 
     if ((ctxt == NULL) || (node == NULL) || (inst == NULL))
 	return;
    if ((comp == NULL) || (comp->select == NULL) || (comp->comp == NULL)) {
	xsltTransformError(ctxt, NULL, inst,
	     "xsl:copy-of : compilation failed\n");
	return;
    }

     /*
    * SPEC XSLT 1.0:
    *  "The xsl:copy-of element can be used to insert a result tree
    *  fragment into the result tree, without first converting it to
    *  a string as xsl:value-of does (see [7.6.1 Generating Text with
    *  xsl:value-of]). The required select attribute contains an
    *  expression. When the result of evaluating the expression is a
    *  result tree fragment, the complete fragment is copied into the
    *  result tree. When the result is a node-set, all the nodes in the
    *  set are copied in document order into the result tree; copying
    *  an element node copies the attribute nodes, namespace nodes and
    *  children of the element node as well as the element node itself;
    *  a root node is copied by copying its children. When the result
    *  is neither a node-set nor a result tree fragment, the result is
    *  converted to a string and then inserted into the result tree,
    *  as with xsl:value-of.
    */

#ifdef WITH_XSLT_DEBUG_PROCESS
    XSLT_TRACE(ctxt,XSLT_TRACE_COPY_OF,xsltGenericDebug(xsltGenericDebugContext,
	 "xsltCopyOf: select %s\n", comp->select));
#endif

     /*
     * Evaluate the "select" expression.
     */
    res = xsltPreCompEval(ctxt, node, comp);
 
     if (res != NULL) {
 	if (res->type == XPATH_NODESET) {
	    /*
	    * Node-set
	    * --------
	    */
#ifdef WITH_XSLT_DEBUG_PROCESS
	    XSLT_TRACE(ctxt,XSLT_TRACE_COPY_OF,xsltGenericDebug(xsltGenericDebugContext,
		 "xsltCopyOf: result is a node set\n"));
#endif
	    list = res->nodesetval;
	    if (list != NULL) {
		xmlNodePtr cur;
		/*
		* The list is already sorted in document order by XPath.
		* Append everything in this order under ctxt->insert.
		*/
		for (i = 0;i < list->nodeNr;i++) {
		    cur = list->nodeTab[i];
		    if (cur == NULL)
			continue;
		    if ((cur->type == XML_DOCUMENT_NODE) ||
			(cur->type == XML_HTML_DOCUMENT_NODE))
		    {
			xsltCopyTreeList(ctxt, inst,
			    cur->children, ctxt->insert, 0, 0);
		    } else if (cur->type == XML_ATTRIBUTE_NODE) {
			xsltShallowCopyAttr(ctxt, inst,
			    ctxt->insert, (xmlAttrPtr) cur);
		    } else {
			xsltCopyTreeInternal(ctxt, inst,
			    cur, ctxt->insert, 0, 0);
		    }
		}
	    }
	} else if (res->type == XPATH_XSLT_TREE) {
	    /*
	    * Result tree fragment
	    * --------------------
	    * E.g. via <xsl:variable ...><foo/></xsl:variable>
	    * Note that the root node of such trees is an xmlDocPtr in Libxslt.
	    */
#ifdef WITH_XSLT_DEBUG_PROCESS
	    XSLT_TRACE(ctxt,XSLT_TRACE_COPY_OF,xsltGenericDebug(xsltGenericDebugContext,
		 "xsltCopyOf: result is a result tree fragment\n"));
#endif
	    list = res->nodesetval;
	    if ((list != NULL) && (list->nodeTab != NULL) &&
		(list->nodeTab[0] != NULL) &&
		(IS_XSLT_REAL_NODE(list->nodeTab[0])))
	    {
		xsltCopyTreeList(ctxt, inst,
		    list->nodeTab[0]->children, ctxt->insert, 0, 0);
	    }
	} else {
	    xmlChar *value = NULL;
	    /*
	    * Convert to a string.
	    */
	    value = xmlXPathCastToString(res);
	    if (value == NULL) {
		xsltTransformError(ctxt, NULL, inst,
		    "Internal error in xsltCopyOf(): "
		    "failed to cast an XPath object to string.\n");
		ctxt->state = XSLT_STATE_STOPPED;
	    } else {
		if (value[0] != 0) {
		    /*
		    * Append content as text node.
		    */
		    xsltCopyTextString(ctxt, ctxt->insert, value, 0);
		}
		xmlFree(value);

#ifdef WITH_XSLT_DEBUG_PROCESS
		XSLT_TRACE(ctxt,XSLT_TRACE_COPY_OF,xsltGenericDebug(xsltGenericDebugContext,
		    "xsltCopyOf: result %s\n", res->stringval));
#endif
	    }
	}
    } else {
	ctxt->state = XSLT_STATE_STOPPED;
    }

    if (res != NULL)
	xmlXPathFreeObject(res);
}
