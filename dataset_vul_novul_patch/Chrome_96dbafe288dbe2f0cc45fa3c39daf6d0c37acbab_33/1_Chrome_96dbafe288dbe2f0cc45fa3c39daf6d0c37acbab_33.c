xsltApplyTemplates(xsltTransformContextPtr ctxt, xmlNodePtr node,
	           xmlNodePtr inst, xsltStylePreCompPtr castedComp)
{
#ifdef XSLT_REFACTORED
    xsltStyleItemApplyTemplatesPtr comp =
	(xsltStyleItemApplyTemplatesPtr) castedComp;
#else
    xsltStylePreCompPtr comp = castedComp;
#endif
    int i;
     xmlNodePtr cur, delNode = NULL, oldContextNode;
     xmlNodeSetPtr list = NULL, oldList;
     xsltStackElemPtr withParams = NULL;
    int oldXPProximityPosition, oldXPContextSize, oldXPNsNr;
     const xmlChar *oldMode, *oldModeURI;
     xmlDocPtr oldXPDoc;
     xsltDocumentPtr oldDocInfo;
     xmlXPathContextPtr xpctxt;
    xmlNsPtr *oldXPNamespaces;
 
     if (comp == NULL) {
 	xsltTransformError(ctxt, NULL, inst,
	     "xsl:apply-templates : compilation failed\n");
	return;
    }
    if ((ctxt == NULL) || (node == NULL) || (inst == NULL) || (comp == NULL))
	return;

#ifdef WITH_XSLT_DEBUG_PROCESS
    if ((node != NULL) && (node->name != NULL))
	XSLT_TRACE(ctxt,XSLT_TRACE_APPLY_TEMPLATES,xsltGenericDebug(xsltGenericDebugContext,
	     "xsltApplyTemplates: node: '%s'\n", node->name));
#endif

    xpctxt = ctxt->xpathCtxt;
    /*
    * Save context states.
    */
    oldContextNode = ctxt->node;
    oldMode = ctxt->mode;
    oldModeURI = ctxt->modeURI;
    oldDocInfo = ctxt->document;
    oldList = ctxt->nodeList;

    /*
     * The xpath context size and proximity position, as
     * well as the xpath and context documents, may be changed
     * so we save their initial state and will restore on exit
     */
     oldXPContextSize = xpctxt->contextSize;
     oldXPProximityPosition = xpctxt->proximityPosition;
     oldXPDoc = xpctxt->doc;
    oldXPNsNr = xpctxt->nsNr;
    oldXPNamespaces = xpctxt->namespaces;
 
     /*
     * Set up contexts.
    */
    ctxt->mode = comp->mode;
    ctxt->modeURI = comp->modeURI;

    if (comp->select != NULL) {
	xmlXPathObjectPtr res = NULL;

	if (comp->comp == NULL) {
	    xsltTransformError(ctxt, NULL, inst,
		 "xsl:apply-templates : compilation failed\n");
	    goto error;
	}
#ifdef WITH_XSLT_DEBUG_PROCESS
	XSLT_TRACE(ctxt,XSLT_TRACE_APPLY_TEMPLATES,xsltGenericDebug(xsltGenericDebugContext,
 	     "xsltApplyTemplates: select %s\n", comp->select));
 #endif
 
	/*
	* Set up XPath.
	*/
	xpctxt->node = node; /* Set the "context node" */
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
	res = xmlXPathCompiledEval(comp->comp, xpctxt);
 
	xpctxt->contextSize = oldXPContextSize;
	xpctxt->proximityPosition = oldXPProximityPosition;
 	if (res != NULL) {
 	    if (res->type == XPATH_NODESET) {
 		list = res->nodesetval; /* consume the node set */
		res->nodesetval = NULL;
	    } else {
		xsltTransformError(ctxt, NULL, inst,
		    "The 'select' expression did not evaluate to a "
		    "node set.\n");
		ctxt->state = XSLT_STATE_STOPPED;
		xmlXPathFreeObject(res);
		goto error;
	    }
	    xmlXPathFreeObject(res);
	    /*
	    * Note: An xsl:apply-templates with a 'select' attribute,
	    * can change the current source doc.
	    */
	} else {
	    xsltTransformError(ctxt, NULL, inst,
		"Failed to evaluate the 'select' expression.\n");
	    ctxt->state = XSLT_STATE_STOPPED;
	    goto error;
	}
	if (list == NULL) {
#ifdef WITH_XSLT_DEBUG_PROCESS
	    XSLT_TRACE(ctxt,XSLT_TRACE_APPLY_TEMPLATES,xsltGenericDebug(xsltGenericDebugContext,
		"xsltApplyTemplates: select didn't evaluate to a node list\n"));
#endif
	    goto exit;
	}
	/*
	*
	* NOTE: Previously a document info (xsltDocument) was
	* created and attached to the Result Tree Fragment.
	* But such a document info is created on demand in
	* xsltKeyFunction() (functions.c), so we need to create
	* it here beforehand.
	* In order to take care of potential keys we need to
	* do some extra work for the case when a Result Tree Fragment
	* is converted into a nodeset (e.g. exslt:node-set()) :
	* We attach a "pseudo-doc" (xsltDocument) to _private.
	* This xsltDocument, together with the keyset, will be freed
	* when the Result Tree Fragment is freed.
	*
	*/
#if 0
	if ((ctxt->nbKeys > 0) &&
	    (list->nodeNr != 0) &&
	    (list->nodeTab[0]->doc != NULL) &&
	    XSLT_IS_RES_TREE_FRAG(list->nodeTab[0]->doc))
	{
	    /*
	    * NOTE that it's also OK if @effectiveDocInfo will be
	    * set to NULL.
	    */
	    isRTF = 1;
	    effectiveDocInfo = list->nodeTab[0]->doc->_private;
	}
#endif
    } else {
	/*
	 * Build an XPath node set with the children
	 */
	list = xmlXPathNodeSetCreate(NULL);
	if (list == NULL)
	    goto error;
	if (node->type != XML_NAMESPACE_DECL)
	    cur = node->children;
	else
	    cur = NULL;
	while (cur != NULL) {
	    switch (cur->type) {
		case XML_TEXT_NODE:
		    if ((IS_BLANK_NODE(cur)) &&
			(cur->parent != NULL) &&
			(cur->parent->type == XML_ELEMENT_NODE) &&
			(ctxt->style->stripSpaces != NULL)) {
			const xmlChar *val;

			if (cur->parent->ns != NULL) {
			    val = (const xmlChar *)
				  xmlHashLookup2(ctxt->style->stripSpaces,
						 cur->parent->name,
						 cur->parent->ns->href);
			    if (val == NULL) {
				val = (const xmlChar *)
				  xmlHashLookup2(ctxt->style->stripSpaces,
						 BAD_CAST "*",
						 cur->parent->ns->href);
			    }
			} else {
			    val = (const xmlChar *)
				  xmlHashLookup2(ctxt->style->stripSpaces,
						 cur->parent->name, NULL);
			}
			if ((val != NULL) &&
			    (xmlStrEqual(val, (xmlChar *) "strip"))) {
			    delNode = cur;
			    break;
			}
		    }
		    /* no break on purpose */
		case XML_ELEMENT_NODE:
		case XML_DOCUMENT_NODE:
		case XML_HTML_DOCUMENT_NODE:
		case XML_CDATA_SECTION_NODE:
		case XML_PI_NODE:
		case XML_COMMENT_NODE:
		    xmlXPathNodeSetAddUnique(list, cur);
		    break;
		case XML_DTD_NODE:
		    /* Unlink the DTD, it's still reachable
		     * using doc->intSubset */
		    if (cur->next != NULL)
			cur->next->prev = cur->prev;
		    if (cur->prev != NULL)
			cur->prev->next = cur->next;
		    break;
		case XML_NAMESPACE_DECL:
		    break;
		default:
#ifdef WITH_XSLT_DEBUG_PROCESS
		    XSLT_TRACE(ctxt,XSLT_TRACE_APPLY_TEMPLATES,xsltGenericDebug(xsltGenericDebugContext,
		     "xsltApplyTemplates: skipping cur type %d\n",
				     cur->type));
#endif
		    delNode = cur;
	    }
	    cur = cur->next;
	    if (delNode != NULL) {
#ifdef WITH_XSLT_DEBUG_PROCESS
		XSLT_TRACE(ctxt,XSLT_TRACE_APPLY_TEMPLATES,xsltGenericDebug(xsltGenericDebugContext,
		     "xsltApplyTemplates: removing ignorable blank cur\n"));
#endif
		xmlUnlinkNode(delNode);
		xmlFreeNode(delNode);
		delNode = NULL;
	    }
	}
    }

#ifdef WITH_XSLT_DEBUG_PROCESS
    if (list != NULL)
    XSLT_TRACE(ctxt,XSLT_TRACE_APPLY_TEMPLATES,xsltGenericDebug(xsltGenericDebugContext,
	"xsltApplyTemplates: list of %d nodes\n", list->nodeNr));
#endif

    if ((list == NULL) || (list->nodeNr == 0))
	goto exit;

    /*
    * Set the context's node set and size; this is also needed for
    * for xsltDoSortFunction().
    */
    ctxt->nodeList = list;
    /*
    * Process xsl:with-param and xsl:sort instructions.
    * (The code became so verbose just to avoid the
    *  xmlNodePtr sorts[XSLT_MAX_SORT] if there's no xsl:sort)
    * BUG TODO: We are not using namespaced potentially defined on the
    * xsl:sort or xsl:with-param elements; XPath expression might fail.
    */
    if (inst->children) {
	xsltStackElemPtr param;

	cur = inst->children;
	while (cur) {

#ifdef WITH_DEBUGGER
	    if (ctxt->debugStatus != XSLT_DEBUG_NONE)
		xslHandleDebugger(cur, node, NULL, ctxt);
#endif
	    if (ctxt->state == XSLT_STATE_STOPPED)
		break;
	    if (cur->type == XML_TEXT_NODE) {
		cur = cur->next;
		continue;
	    }
	    if (! IS_XSLT_ELEM(cur))
		break;
	    if (IS_XSLT_NAME(cur, "with-param")) {
		param = xsltParseStylesheetCallerParam(ctxt, cur);
		if (param != NULL) {
		    param->next = withParams;
		    withParams = param;
		}
	    }
	    if (IS_XSLT_NAME(cur, "sort")) {
		xsltTemplatePtr oldCurTempRule =
		    ctxt->currentTemplateRule;
		int nbsorts = 0;
		xmlNodePtr sorts[XSLT_MAX_SORT];

		sorts[nbsorts++] = cur;

		while (cur) {

#ifdef WITH_DEBUGGER
		    if (ctxt->debugStatus != XSLT_DEBUG_NONE)
			xslHandleDebugger(cur, node, NULL, ctxt);
#endif
		    if (ctxt->state == XSLT_STATE_STOPPED)
			break;

		    if (cur->type == XML_TEXT_NODE) {
			cur = cur->next;
			continue;
		    }

		    if (! IS_XSLT_ELEM(cur))
			break;
		    if (IS_XSLT_NAME(cur, "with-param")) {
			param = xsltParseStylesheetCallerParam(ctxt, cur);
			if (param != NULL) {
			    param->next = withParams;
			    withParams = param;
			}
		    }
		    if (IS_XSLT_NAME(cur, "sort")) {
			if (nbsorts >= XSLT_MAX_SORT) {
			    xsltTransformError(ctxt, NULL, cur,
				"The number (%d) of xsl:sort instructions exceeds the "
				"maximum allowed by this processor's settings.\n",
				nbsorts);
			    ctxt->state = XSLT_STATE_STOPPED;
			    break;
			} else {
			    sorts[nbsorts++] = cur;
			}
		    }
		    cur = cur->next;
		}
		/*
		* The "current template rule" is cleared for xsl:sort.
		*/
		ctxt->currentTemplateRule = NULL;
		/*
		* Sort.
		*/
		xsltDoSortFunction(ctxt, sorts, nbsorts);
		ctxt->currentTemplateRule = oldCurTempRule;
		break;
	    }
	    cur = cur->next;
	}
    }
    xpctxt->contextSize = list->nodeNr;
    /*
    * Apply templates for all selected source nodes.
    */
    for (i = 0; i < list->nodeNr; i++) {
	cur = list->nodeTab[i];
	/*
	* The node becomes the "current node".
	*/
	ctxt->node = cur;
	/*
	* An xsl:apply-templates can change the current context doc.
	* OPTIMIZE TODO: Get rid of the need to set the context doc.
	*/
	if ((cur->type != XML_NAMESPACE_DECL) && (cur->doc != NULL))
	    xpctxt->doc = cur->doc;

	xpctxt->proximityPosition = i + 1;
	/*
	* Find and apply a template for this node.
	*/
	xsltProcessOneNode(ctxt, cur, withParams);
    }

exit:
error:
    /*
    * Free the parameter list.
    */
    if (withParams != NULL)
	xsltFreeStackElemList(withParams);
    if (list != NULL)
	xmlXPathFreeNodeSet(list);
     /*
     * Restore context states.
     */
    xpctxt->nsNr = oldXPNsNr;
    xpctxt->namespaces = oldXPNamespaces;
     xpctxt->doc = oldXPDoc;
     xpctxt->contextSize = oldXPContextSize;
     xpctxt->proximityPosition = oldXPProximityPosition;

    ctxt->document = oldDocInfo;
    ctxt->nodeList = oldList;
    ctxt->node = oldContextNode;
    ctxt->mode = oldMode;
    ctxt->modeURI = oldModeURI;
}
