xsltElement(xsltTransformContextPtr ctxt, xmlNodePtr node,
	    xmlNodePtr inst, xsltStylePreCompPtr castedComp) {
#ifdef XSLT_REFACTORED
    xsltStyleItemElementPtr comp = (xsltStyleItemElementPtr) castedComp;
#else
    xsltStylePreCompPtr comp = castedComp;
#endif
    xmlChar *prop = NULL;
    const xmlChar *name, *prefix = NULL, *nsName = NULL;
    xmlNodePtr copy;
    xmlNodePtr oldInsert;

    if (ctxt->insert == NULL)
	return;

    /*
    * A comp->has_name == 0 indicates that we need to skip this instruction,
    * since it was evaluated to be invalid already during compilation.
    */
    if (!comp->has_name)
        return;

    /*
     * stack and saves
     */
    oldInsert = ctxt->insert;

    if (comp->name == NULL) {
	/* TODO: fix attr acquisition wrt to the XSLT namespace */
        prop = xsltEvalAttrValueTemplate(ctxt, inst,
	    (const xmlChar *) "name", XSLT_NAMESPACE);
        if (prop == NULL) {
            xsltTransformError(ctxt, NULL, inst,
		"xsl:element: The attribute 'name' is missing.\n");
            goto error;
        }
	if (xmlValidateQName(prop, 0)) {
	    xsltTransformError(ctxt, NULL, inst,
		"xsl:element: The effective name '%s' is not a "
		"valid QName.\n", prop);
	    /* we fall through to catch any further errors, if possible */
	}
	name = xsltSplitQName(ctxt->dict, prop, &prefix);
	xmlFree(prop);
    } else {
	/*
	* The "name" value was static.
	*/
#ifdef XSLT_REFACTORED
	prefix = comp->nsPrefix;
	name = comp->name;
#else
	name = xsltSplitQName(ctxt->dict, comp->name, &prefix);
#endif
    }

    /*
     * Create the new element
     */
    if (ctxt->output->dict == ctxt->dict) {
	copy = xmlNewDocNodeEatName(ctxt->output, NULL, (xmlChar *)name, NULL);
    } else {
	copy = xmlNewDocNode(ctxt->output, NULL, (xmlChar *)name, NULL);
    }
    if (copy == NULL) {
	xsltTransformError(ctxt, NULL, inst,
	    "xsl:element : creation of %s failed\n", name);
 	return;
     }
     copy = xsltAddChild(ctxt->insert, copy);
    if (copy == NULL) {
        xsltTransformError(ctxt, NULL, inst,
            "xsl:element : xsltAddChild failed\n");
        return;
    }
 
     /*
     * Namespace
    * ---------
    */
    if (comp->has_ns) {
	if (comp->ns != NULL) {
	    /*
	    * No AVT; just plain text for the namespace name.
	    */
	    if (comp->ns[0] != 0)
		nsName = comp->ns;
	} else {
	    xmlChar *tmpNsName;
	    /*
	    * Eval the AVT.
	    */
	    /* TODO: check attr acquisition wrt to the XSLT namespace */
	    tmpNsName = xsltEvalAttrValueTemplate(ctxt, inst,
		(const xmlChar *) "namespace", XSLT_NAMESPACE);
	    /*
	    * SPEC XSLT 1.0:
	    *  "If the string is empty, then the expanded-name of the
	    *  attribute has a null namespace URI."
	    */
	    if ((tmpNsName != NULL) && (tmpNsName[0] != 0))
		nsName = xmlDictLookup(ctxt->dict, BAD_CAST tmpNsName, -1);
	    xmlFree(tmpNsName);
	}

        if (xmlStrEqual(nsName, BAD_CAST "http://www.w3.org/2000/xmlns/")) {
            xsltTransformError(ctxt, NULL, inst,
                "xsl:attribute: Namespace http://www.w3.org/2000/xmlns/ "
                "forbidden.\n");
            goto error;
        }
        if (xmlStrEqual(nsName, XML_XML_NAMESPACE)) {
            prefix = BAD_CAST "xml";
        } else if (xmlStrEqual(prefix, BAD_CAST "xml")) {
            prefix = NULL;
        }
    } else {
	xmlNsPtr ns;
	/*
	* SPEC XSLT 1.0:
	*  "If the namespace attribute is not present, then the QName is
	*  expanded into an expanded-name using the namespace declarations
	*  in effect for the xsl:element element, including any default
	*  namespace declaration.
	*/
	ns = xmlSearchNs(inst->doc, inst, prefix);
	if (ns == NULL) {
	    /*
	    * TODO: Check this in the compilation layer in case it's a
	    * static value.
	    */
            if (prefix != NULL) {
                xsltTransformError(ctxt, NULL, inst,
                    "xsl:element: The QName '%s:%s' has no "
                    "namespace binding in scope in the stylesheet; "
                    "this is an error, since the namespace was not "
                    "specified by the instruction itself.\n", prefix, name);
            }
	} else
	    nsName = ns->href;
    }
    /*
    * Find/create a matching ns-decl in the result tree.
    */
    if (nsName != NULL) {
	if (xmlStrEqual(prefix, BAD_CAST "xmlns")) {
            /* Don't use a prefix of "xmlns" */
	    xmlChar *pref = xmlStrdup(BAD_CAST "ns_1");

	    copy->ns = xsltGetSpecialNamespace(ctxt, inst, nsName, pref, copy);

	    xmlFree(pref);
	} else {
	    copy->ns = xsltGetSpecialNamespace(ctxt, inst, nsName, prefix,
		copy);
	}
    } else if ((copy->parent != NULL) &&
	(copy->parent->type == XML_ELEMENT_NODE) &&
	(copy->parent->ns != NULL))
    {
	/*
	* "Undeclare" the default namespace.
	*/
	xsltGetSpecialNamespace(ctxt, inst, NULL, NULL, copy);
    }

    ctxt->insert = copy;

    if (comp->has_use) {
	if (comp->use != NULL) {
	    xsltApplyAttributeSet(ctxt, node, inst, comp->use);
	} else {
	    xmlChar *attrSets = NULL;
	    /*
	    * BUG TODO: use-attribute-sets is not a value template.
	    *  use-attribute-sets = qnames
	    */
	    attrSets = xsltEvalAttrValueTemplate(ctxt, inst,
		(const xmlChar *)"use-attribute-sets", NULL);
	    if (attrSets != NULL) {
		xsltApplyAttributeSet(ctxt, node, inst, attrSets);
		xmlFree(attrSets);
	    }
	}
    }
    /*
    * Instantiate the sequence constructor.
    */
    if (inst->children != NULL)
	xsltApplySequenceConstructor(ctxt, ctxt->node, inst->children,
	    NULL);

error:
    ctxt->insert = oldInsert;
    return;
}
