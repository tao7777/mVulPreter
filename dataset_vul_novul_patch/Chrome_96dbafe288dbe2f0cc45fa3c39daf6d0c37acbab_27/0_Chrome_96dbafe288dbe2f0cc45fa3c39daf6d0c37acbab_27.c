 xsltTestCompMatch(xsltTransformContextPtr ctxt, xsltCompMatchPtr comp,
	          xmlNodePtr matchNode, const xmlChar *mode,
 		  const xmlChar *modeURI) {
     int i;
    xmlNodePtr node = matchNode;
     xsltStepOpPtr step, sel = NULL;
     xsltStepStates states = {0, 0, NULL}; /* // may require backtrack */
 
    if ((comp == NULL) || (node == NULL) || (ctxt == NULL)) {
	xsltTransformError(ctxt, NULL, node,
		"xsltTestCompMatch: null arg\n");
        return(-1);
    }
    if (mode != NULL) {
	if (comp->mode == NULL)
	    return(0);
	/*
	 * both mode strings must be interned on the stylesheet dictionary
	 */
	if (comp->mode != mode)
	    return(0);
    } else {
	if (comp->mode != NULL)
	    return(0);
    }
    if (modeURI != NULL) {
	if (comp->modeURI == NULL)
	    return(0);
	/*
	 * both modeURI strings must be interned on the stylesheet dictionary
	 */
	if (comp->modeURI != modeURI)
	    return(0);
    } else {
	if (comp->modeURI != NULL)
	    return(0);
    }

    i = 0;
restart:
    for (;i < comp->nbStep;i++) {
	step = &comp->steps[i];
	if (step->op != XSLT_OP_PREDICATE)
	    sel = step;
	switch (step->op) {
            case XSLT_OP_END:
		goto found;
            case XSLT_OP_ROOT:
		if ((node->type == XML_DOCUMENT_NODE) ||
#ifdef LIBXML_DOCB_ENABLED
		    (node->type == XML_DOCB_DOCUMENT_NODE) ||
#endif
		    (node->type == XML_HTML_DOCUMENT_NODE))
		    continue;
		if ((node->type == XML_ELEMENT_NODE) && (node->name[0] == ' '))
		    continue;
		goto rollback;
            case XSLT_OP_ELEM:
		if (node->type != XML_ELEMENT_NODE)
		    goto rollback;
		if (step->value == NULL)
		    continue;
		if (step->value[0] != node->name[0])
		    goto rollback;
		if (!xmlStrEqual(step->value, node->name))
		    goto rollback;

		/* Namespace test */
		if (node->ns == NULL) {
		    if (step->value2 != NULL)
			goto rollback;
		} else if (node->ns->href != NULL) {
		    if (step->value2 == NULL)
			goto rollback;
		    if (!xmlStrEqual(step->value2, node->ns->href))
			goto rollback;
		}
		continue;
            case XSLT_OP_ATTR:
		if (node->type != XML_ATTRIBUTE_NODE)
		    goto rollback;
		if (step->value != NULL) {
		    if (step->value[0] != node->name[0])
			goto rollback;
		    if (!xmlStrEqual(step->value, node->name))
			goto rollback;
		}
		/* Namespace test */
		if (node->ns == NULL) {
		    if (step->value2 != NULL)
			goto rollback;
		} else if (step->value2 != NULL) {
		    if (!xmlStrEqual(step->value2, node->ns->href))
			goto rollback;
		}
		continue;
            case XSLT_OP_PARENT:
		if ((node->type == XML_DOCUMENT_NODE) ||
		    (node->type == XML_HTML_DOCUMENT_NODE) ||
#ifdef LIBXML_DOCB_ENABLED
		    (node->type == XML_DOCB_DOCUMENT_NODE) ||
#endif
		    (node->type == XML_NAMESPACE_DECL))
		    goto rollback;
		node = node->parent;
		if (node == NULL)
		    goto rollback;
		if (step->value == NULL)
		    continue;
		if (step->value[0] != node->name[0])
		    goto rollback;
		if (!xmlStrEqual(step->value, node->name))
		    goto rollback;
		/* Namespace test */
		if (node->ns == NULL) {
		    if (step->value2 != NULL)
			goto rollback;
		} else if (node->ns->href != NULL) {
		    if (step->value2 == NULL)
			goto rollback;
		    if (!xmlStrEqual(step->value2, node->ns->href))
			goto rollback;
		}
		continue;
            case XSLT_OP_ANCESTOR:
		/* TODO: implement coalescing of ANCESTOR/NODE ops */
		if (step->value == NULL) {
		    step = &comp->steps[i+1];
		    if (step->op == XSLT_OP_ROOT)
			goto found;
		    /* added NS, ID and KEY as a result of bug 168208 */
		    if ((step->op != XSLT_OP_ELEM) &&
			(step->op != XSLT_OP_ALL) &&
			(step->op != XSLT_OP_NS) &&
			(step->op != XSLT_OP_ID) &&
			(step->op != XSLT_OP_KEY))
			goto rollback;
		}
		if (node == NULL)
		    goto rollback;
		if ((node->type == XML_DOCUMENT_NODE) ||
		    (node->type == XML_HTML_DOCUMENT_NODE) ||
#ifdef LIBXML_DOCB_ENABLED
		    (node->type == XML_DOCB_DOCUMENT_NODE) ||
#endif
		    (node->type == XML_NAMESPACE_DECL))
		    goto rollback;
		node = node->parent;
		if ((step->op != XSLT_OP_ELEM) && step->op != XSLT_OP_ALL) {
		    xsltPatPushState(ctxt, &states, i, node);
		    continue;
		}
		i++;
		if (step->value == NULL) {
		    xsltPatPushState(ctxt, &states, i - 1, node);
		    continue;
		}
		while (node != NULL) {
		    if ((node->type == XML_ELEMENT_NODE) &&
			(step->value[0] == node->name[0]) &&
			(xmlStrEqual(step->value, node->name))) {
			/* Namespace test */
			if (node->ns == NULL) {
			    if (step->value2 == NULL)
				break;
			} else if (node->ns->href != NULL) {
			    if ((step->value2 != NULL) &&
			        (xmlStrEqual(step->value2, node->ns->href)))
				break;
			}
		    }
		    node = node->parent;
		}
		if (node == NULL)
		    goto rollback;
		xsltPatPushState(ctxt, &states, i - 1, node);
		continue;
            case XSLT_OP_ID: {
		/* TODO Handle IDs decently, must be done differently */
		xmlAttrPtr id;

		if (node->type != XML_ELEMENT_NODE)
		    goto rollback;

		id = xmlGetID(node->doc, step->value);
		if ((id == NULL) || (id->parent != node))
		    goto rollback;
		break;
	    }
            case XSLT_OP_KEY: {
		xmlNodeSetPtr list;
		int indx;

		list = xsltGetKey(ctxt, step->value,
			          step->value3, step->value2);
		if (list == NULL)
		    goto rollback;
		for (indx = 0;indx < list->nodeNr;indx++)
		    if (list->nodeTab[indx] == node)
			break;
		if (indx >= list->nodeNr)
		    goto rollback;
		break;
	    }
            case XSLT_OP_NS:
		if (node->type != XML_ELEMENT_NODE)
		    goto rollback;
		if (node->ns == NULL) {
		    if (step->value != NULL)
			goto rollback;
		} else if (node->ns->href != NULL) {
		    if (step->value == NULL)
			goto rollback;
		    if (!xmlStrEqual(step->value, node->ns->href))
			goto rollback;
		}
		break;
            case XSLT_OP_ALL:
		if (node->type != XML_ELEMENT_NODE)
 		    goto rollback;
 		break;
 	    case XSLT_OP_PREDICATE: {
 		/*
		 * When there is cascading XSLT_OP_PREDICATE or a predicate
		 * after an op which hasn't been optimized yet, then use a
 		 * direct computation approach. It's not done directly
 		 * at the beginning of the routine to filter out as much
 		 * as possible this costly computation.
		 */
		if (comp->direct) {
		    if (states.states != NULL) {
 			/* Free the rollback states */
 			xmlFree(states.states);
 		    }
		    return(xsltTestCompMatchDirect(ctxt, comp, matchNode,
 						   comp->nsList, comp->nsNr));
 		}
 
		if (!xsltTestPredicateMatch(ctxt, comp, node, step, sel))
		    goto rollback;
 
 		break;
 	    }
             case XSLT_OP_PI:
 		if (node->type != XML_PI_NODE)
		    goto rollback;
		if (step->value != NULL) {
		    if (!xmlStrEqual(step->value, node->name))
			goto rollback;
		}
		break;
            case XSLT_OP_COMMENT:
		if (node->type != XML_COMMENT_NODE)
		    goto rollback;
		break;
            case XSLT_OP_TEXT:
		if ((node->type != XML_TEXT_NODE) &&
		    (node->type != XML_CDATA_SECTION_NODE))
		    goto rollback;
		break;
            case XSLT_OP_NODE:
		switch (node->type) {
		    case XML_ELEMENT_NODE:
		    case XML_CDATA_SECTION_NODE:
		    case XML_PI_NODE:
		    case XML_COMMENT_NODE:
		    case XML_TEXT_NODE:
			break;
		    default:
			goto rollback;
		}
		break;
	}
    }
found:
    if (states.states != NULL) {
        /* Free the rollback states */
	xmlFree(states.states);
    }
    return(1);
rollback:
    /* got an error try to rollback */
    if (states.states == NULL)
	return(0);
    if (states.nbstates <= 0) {
	xmlFree(states.states);
	return(0);
    }
    states.nbstates--;
    i = states.states[states.nbstates].step;
    node = states.states[states.nbstates].node;
#if 0
    fprintf(stderr, "Pop: %d, %s\n", i, node->name);
#endif
    goto restart;
}
