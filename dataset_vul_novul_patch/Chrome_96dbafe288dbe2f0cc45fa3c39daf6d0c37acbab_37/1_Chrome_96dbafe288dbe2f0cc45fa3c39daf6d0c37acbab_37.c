xsltCopyTreeInternal(xsltTransformContextPtr ctxt,
		     xmlNodePtr invocNode,
		     xmlNodePtr node,
		     xmlNodePtr insert, int isLRE, int topElemVisited)
{
    xmlNodePtr copy;

    if (node == NULL)
	return(NULL);
    switch (node->type) {
        case XML_ELEMENT_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_PI_NODE:
        case XML_COMMENT_NODE:
        case XML_DOCUMENT_NODE:
        case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
        case XML_DOCB_DOCUMENT_NODE:
#endif
	    break;
        case XML_TEXT_NODE: {
	    int noenc = (node->name == xmlStringTextNoenc);
	    return(xsltCopyTextString(ctxt, insert, node->content, noenc));
	    }
        case XML_CDATA_SECTION_NODE:
	    return(xsltCopyTextString(ctxt, insert, node->content, 0));
        case XML_ATTRIBUTE_NODE:
	    return((xmlNodePtr)
		xsltShallowCopyAttr(ctxt, invocNode, insert, (xmlAttrPtr) node));
        case XML_NAMESPACE_DECL:
	    return((xmlNodePtr) xsltShallowCopyNsNode(ctxt, invocNode,
		insert, (xmlNsPtr) node));

        case XML_DOCUMENT_TYPE_NODE:
        case XML_DOCUMENT_FRAG_NODE:
        case XML_NOTATION_NODE:
        case XML_DTD_NODE:
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
            return(NULL);
    }
    if (XSLT_IS_RES_TREE_FRAG(node)) {
	if (node->children != NULL)
	    copy = xsltCopyTreeList(ctxt, invocNode,
		node->children, insert, 0, 0);
	else
	    copy = NULL;
	return(copy);
    }
    copy = xmlDocCopyNode(node, insert->doc, 0);
     if (copy != NULL) {
 	copy->doc = ctxt->output;
 	copy = xsltAddChild(insert, copy);
 	/*
 	 * The node may have been coalesced into another text node.
 	 */
	if (insert->last != copy)
	    return(insert->last);
	copy->next = NULL;

	if (node->type == XML_ELEMENT_NODE) {
	    /*
	    * Copy in-scope namespace nodes.
	    *
	    * REVISIT: Since we try to reuse existing in-scope ns-decls by
	    *  using xmlSearchNsByHref(), this will eventually change
	    *  the prefix of an original ns-binding; thus it might
	    *  break QNames in element/attribute content.
	    * OPTIMIZE TODO: If we had a xmlNsPtr * on the transformation
	    *  context, plus a ns-lookup function, which writes directly
	    *  to a given list, then we wouldn't need to create/free the
	    *  nsList every time.
	    */
	    if ((topElemVisited == 0) &&
		(node->parent != NULL) &&
		(node->parent->type != XML_DOCUMENT_NODE) &&
		(node->parent->type != XML_HTML_DOCUMENT_NODE))
	    {
		xmlNsPtr *nsList, *curns, ns;

		/*
		* If this is a top-most element in a tree to be
		* copied, then we need to ensure that all in-scope
		* namespaces are copied over. For nodes deeper in the
		* tree, it is sufficient to reconcile only the ns-decls
		* (node->nsDef entries).
		*/

		nsList = xmlGetNsList(node->doc, node);
		if (nsList != NULL) {
		    curns = nsList;
		    do {
			/*
			* Search by prefix first in order to break as less
			* QNames in element/attribute content as possible.
			*/
			ns = xmlSearchNs(insert->doc, insert,
			    (*curns)->prefix);

			if ((ns == NULL) ||
			    (! xmlStrEqual(ns->href, (*curns)->href)))
			{
			    ns = NULL;
			    /*
			    * Search by namespace name.
			    * REVISIT TODO: Currently disabled.
			    */
#if 0
			    ns = xmlSearchNsByHref(insert->doc,
				insert, (*curns)->href);
#endif
			}
			if (ns == NULL) {
			    /*
			    * Declare a new namespace on the copied element.
			    */
			    ns = xmlNewNs(copy, (*curns)->href,
				(*curns)->prefix);
			    /* TODO: Handle errors */
			}
			if (node->ns == *curns) {
			    /*
			    * If this was the original's namespace then set
			    * the generated counterpart on the copy.
			    */
			    copy->ns = ns;
			}
			curns++;
		    } while (*curns != NULL);
		    xmlFree(nsList);
		}
	    } else if (node->nsDef != NULL) {
		/*
		* Copy over all namespace declaration attributes.
		*/
		if (node->nsDef != NULL) {
		    if (isLRE)
			xsltCopyNamespaceList(ctxt, copy, node->nsDef);
		    else
			xsltCopyNamespaceListInternal(copy, node->nsDef);
		}
	    }
	    /*
	    * Set the namespace.
	    */
	    if (node->ns != NULL) {
		if (copy->ns == NULL) {
		    /*
		    * This will map copy->ns to one of the newly created
		    * in-scope ns-decls, OR create a new ns-decl on @copy.
		    */
		    copy->ns = xsltGetSpecialNamespace(ctxt, invocNode,
			node->ns->href, node->ns->prefix, copy);
		}
	    } else if ((insert->type == XML_ELEMENT_NODE) &&
		(insert->ns != NULL))
	    {
		/*
		* "Undeclare" the default namespace on @copy with xmlns="".
		*/
		xsltGetSpecialNamespace(ctxt, invocNode, NULL, NULL, copy);
	    }
	    /*
	    * Copy attribute nodes.
	    */
	    if (node->properties != NULL) {
		xsltCopyAttrListNoOverwrite(ctxt, invocNode,
		    copy, node->properties);
	    }
	    if (topElemVisited == 0)
		topElemVisited = 1;
	}
	/*
	* Copy the subtree.
	*/
	if (node->children != NULL) {
	    xsltCopyTreeList(ctxt, invocNode,
		node->children, copy, isLRE, topElemVisited);
	}
    } else {
	xsltTransformError(ctxt, NULL, invocNode,
	    "xsltCopyTreeInternal: Copying of '%s' failed.\n", node->name);
    }
    return(copy);
}
