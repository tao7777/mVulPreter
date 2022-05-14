xsltCopyNamespaceList(xsltTransformContextPtr ctxt, xmlNodePtr node,
	              xmlNsPtr cur) {
    xmlNsPtr ret = NULL, tmp;
    xmlNsPtr p = NULL,q;

    if (cur == NULL)
	return(NULL);
    if (cur->type != XML_NAMESPACE_DECL)
	return(NULL);

    /*
     * One can add namespaces only on element nodes
     */
    if ((node != NULL) && (node->type != XML_ELEMENT_NODE))
	node = NULL;

    while (cur != NULL) {
	if (cur->type != XML_NAMESPACE_DECL)
	    break;

	/*
	 * Avoid duplicating namespace declarations in the tree if
	 * a matching declaration is in scope.
	 */
	if (node != NULL) {
	    if ((node->ns != NULL) &&
		(xmlStrEqual(node->ns->prefix, cur->prefix)) &&
	(xmlStrEqual(node->ns->href, cur->href))) {
		cur = cur->next;
		continue;
	    }
	    tmp = xmlSearchNs(node->doc, node, cur->prefix);
	    if ((tmp != NULL) && (xmlStrEqual(tmp->href, cur->href))) {
		cur = cur->next;
		continue;
	    }
	}
#ifdef XSLT_REFACTORED
	/*
	* Namespace exclusion and ns-aliasing is performed at
	* compilation-time in the refactored code.
	*/
	q = xmlNewNs(node, cur->href, cur->prefix);
	if (p == NULL) {
	    ret = p = q;
	} else {
	    p->next = q;
	    p = q;
	}
#else
	/*
	* TODO: Remove this if the refactored code gets enabled.
	*/
	if (!xmlStrEqual(cur->href, XSLT_NAMESPACE)) {
	    const xmlChar *URI;
 	    /* TODO apply cascading */
 	    URI = (const xmlChar *) xmlHashLookup(ctxt->style->nsAliases,
 		                                  cur->href);
	    if (URI == UNDEFINED_DEFAULT_NS) {
		cur = cur->next;
 	        continue;
	    }
 	    if (URI != NULL) {
 		q = xmlNewNs(node, URI, cur->prefix);
 	    } else {
		q = xmlNewNs(node, cur->href, cur->prefix);
	    }
	    if (p == NULL) {
		ret = p = q;
	    } else {
		p->next = q;
		p = q;
	    }
	}
#endif
	cur = cur->next;
    }
    return(ret);
}
