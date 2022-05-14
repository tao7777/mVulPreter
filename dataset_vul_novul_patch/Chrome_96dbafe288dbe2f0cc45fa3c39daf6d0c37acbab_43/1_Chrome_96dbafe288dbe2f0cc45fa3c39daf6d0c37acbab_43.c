xsltShallowCopyElem(xsltTransformContextPtr ctxt, xmlNodePtr node,
		    xmlNodePtr insert, int isLRE)
{
    xmlNodePtr copy;

    if ((node->type == XML_DTD_NODE) || (insert == NULL))
	return(NULL);
    if ((node->type == XML_TEXT_NODE) ||
	(node->type == XML_CDATA_SECTION_NODE))
	return(xsltCopyText(ctxt, insert, node, 0));

    copy = xmlDocCopyNode(node, insert->doc, 0);
     if (copy != NULL) {
 	copy->doc = ctxt->output;
 	copy = xsltAddChild(insert, copy);
 
 	if (node->type == XML_ELEMENT_NODE) {
 	    /*
	     * Add namespaces as they are needed
	     */
	    if (node->nsDef != NULL) {
		/*
		* TODO: Remove the LRE case in the refactored code
		* gets enabled.
		*/
		if (isLRE)
		    xsltCopyNamespaceList(ctxt, copy, node->nsDef);
		else
		    xsltCopyNamespaceListInternal(copy, node->nsDef);
	    }

	    /*
	    * URGENT TODO: The problem with this is that it does not
	    *  copy over all namespace nodes in scope.
	    *  The damn thing about this is, that we would need to
	    *  use the xmlGetNsList(), for every single node; this is
	    *  also done in xsltCopyTreeInternal(), but only for the top node.
	    */
	    if (node->ns != NULL) {
		if (isLRE) {
		    /*
		    * REVISIT TODO: Since the non-refactored code still does
		    *  ns-aliasing, we need to call xsltGetNamespace() here.
		    *  Remove this when ready.
		    */
		    copy->ns = xsltGetNamespace(ctxt, node, node->ns, copy);
		} else {
		    copy->ns = xsltGetSpecialNamespace(ctxt,
			node, node->ns->href, node->ns->prefix, copy);

		}
	    } else if ((insert->type == XML_ELEMENT_NODE) &&
		       (insert->ns != NULL))
	    {
		/*
		* "Undeclare" the default namespace.
		*/
		xsltGetSpecialNamespace(ctxt, node, NULL, NULL, copy);
	    }
	}
    } else {
	xsltTransformError(ctxt, NULL, node,
		"xsltShallowCopyElem: copy %s failed\n", node->name);
    }
    return(copy);
}
