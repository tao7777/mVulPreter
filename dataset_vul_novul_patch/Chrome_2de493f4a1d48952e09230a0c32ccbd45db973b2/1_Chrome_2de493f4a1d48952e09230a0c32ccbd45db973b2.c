xsltAttrListTemplateProcess(xsltTransformContextPtr ctxt, 
	                    xmlNodePtr target, xmlAttrPtr attrs)
{
    xmlAttrPtr attr, copy, last;
    xmlNodePtr oldInsert, text;
    xmlNsPtr origNs = NULL, copyNs = NULL;
    const xmlChar *value;
    xmlChar *valueAVT;

    if ((ctxt == NULL) || (target == NULL) || (attrs == NULL))
	return(NULL);

    oldInsert = ctxt->insert;
    ctxt->insert = target;        

    /*
    * Instantiate LRE-attributes.
    */
    if (target->properties) {
	last = target->properties;
	while (last->next != NULL)
	    last = last->next;
    } else {
	last = NULL;
    }
    attr = attrs;
    do {
	/*
	* Skip XSLT attributes.
	*/
#ifdef XSLT_REFACTORED
	if (attr->psvi == xsltXSLTAttrMarker) {
	    goto next_attribute;
	}
#else
	if ((attr->ns != NULL) &&
	    xmlStrEqual(attr->ns->href, XSLT_NAMESPACE))
	{
	    goto next_attribute;
	}
#endif
	/*
	* Get the value.
	*/
	if (attr->children != NULL) {
	    if ((attr->children->type != XML_TEXT_NODE) ||
		(attr->children->next != NULL))
	    {
		xsltTransformError(ctxt, NULL, attr->parent,
		    "Internal error: The children of an attribute node of a "
		    "literal result element are not in the expected form.\n");
		goto error;
	    }
	    value = attr->children->content;
	    if (value == NULL)
		value = xmlDictLookup(ctxt->dict, BAD_CAST "", 0);
	} else
	    value = xmlDictLookup(ctxt->dict, BAD_CAST "", 0);

	/*
	* Create a new attribute.
	*/
	copy = xmlNewDocProp(target->doc, attr->name, NULL);
	if (copy == NULL) {
	    if (attr->ns) {
		xsltTransformError(ctxt, NULL, attr->parent,
		    "Internal error: Failed to create attribute '{%s}%s'.\n",
		    attr->ns->href, attr->name);
	    } else {
		xsltTransformError(ctxt, NULL, attr->parent,
		    "Internal error: Failed to create attribute '%s'.\n",
		    attr->name);
	    }
	    goto error;
	}
	/*
	* Attach it to the target element.
	*/
	copy->parent = target;
	if (last == NULL) {
	    target->properties = copy;
	    last = copy;
	} else {
	    last->next = copy;
	    copy->prev = last;
	    last = copy;
	}
	/*
	* Set the namespace. Avoid lookups of same namespaces.
	*/
	if (attr->ns != origNs) {
	    origNs = attr->ns;
	    if (attr->ns != NULL) {
#ifdef XSLT_REFACTORED
		copyNs = xsltGetSpecialNamespace(ctxt, attr->parent,
		    attr->ns->href, attr->ns->prefix, target);
#else
		copyNs = xsltGetNamespace(ctxt, attr->parent,
		    attr->ns, target);
#endif
		if (copyNs == NULL)
		    goto error;
	    } else
		copyNs = NULL;
	}
	copy->ns = copyNs;

	/*
	* Set the value.
	*/
	text = xmlNewText(NULL);
	if (text != NULL) {
	    copy->last = copy->children = text;
	    text->parent = (xmlNodePtr) copy;
	    text->doc = copy->doc;

	    if (attr->psvi != NULL) {
		/*
		* Evaluate the Attribute Value Template.
		*/
		valueAVT = xsltEvalAVT(ctxt, attr->psvi, attr->parent);
		if (valueAVT == NULL) {
		    /*
		    * TODO: Damn, we need an easy mechanism to report
		    * qualified names!
		    */
		    if (attr->ns) {
			xsltTransformError(ctxt, NULL, attr->parent,
			    "Internal error: Failed to evaluate the AVT "
			    "of attribute '{%s}%s'.\n",
			    attr->ns->href, attr->name);
		    } else {
			xsltTransformError(ctxt, NULL, attr->parent,
			    "Internal error: Failed to evaluate the AVT "
			    "of attribute '%s'.\n",
			    attr->name);
		    }
		    text->content = xmlStrdup(BAD_CAST "");
		    goto error;
		} else {
		    text->content = valueAVT;
 		}
 	    } else if ((ctxt->internalized) &&
 		(target->doc != NULL) &&
		(target->doc->dict == ctxt->dict))
 	    {
 		text->content = (xmlChar *) value;
 	    } else {
		text->content = xmlStrdup(value);
	    }
            if ((copy != NULL) && (text != NULL) &&
                (xmlIsID(copy->doc, copy->parent, copy)))
                xmlAddID(NULL, copy->doc, text->content, copy);
	}

next_attribute:
	attr = attr->next;
    } while (attr != NULL);

    /*
    * Apply attribute-sets.
    * The creation of such attributes will not overwrite any existing
    * attribute.
    */
    attr = attrs;
    do {
#ifdef XSLT_REFACTORED
	if ((attr->psvi == xsltXSLTAttrMarker) &&
	    xmlStrEqual(attr->name, (const xmlChar *)"use-attribute-sets"))
	{
	    xsltApplyAttributeSet(ctxt, ctxt->node, (xmlNodePtr) attr, NULL);
	}
#else
	if ((attr->ns != NULL) &&
	    xmlStrEqual(attr->name, (const xmlChar *)"use-attribute-sets") &&
	    xmlStrEqual(attr->ns->href, XSLT_NAMESPACE))
	{
	    xsltApplyAttributeSet(ctxt, ctxt->node, (xmlNodePtr) attr, NULL);
	}
#endif
	attr = attr->next;
    } while (attr != NULL);

    ctxt->insert = oldInsert;
    return(target->properties);

error:
    ctxt->insert = oldInsert;
    return(NULL);
}
