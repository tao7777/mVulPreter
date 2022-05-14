xsltElementComp(xsltStylesheetPtr style, xmlNodePtr inst) {
#ifdef XSLT_REFACTORED
    xsltStyleItemElementPtr comp;
#else
    xsltStylePreCompPtr comp;
#endif

    /*
    * <xsl:element
    *   name = { qname }
    *   namespace = { uri-reference }
    *   use-attribute-sets = qnames>
    *   <!-- Content: template -->
    * </xsl:element>
    */
    if ((style == NULL) || (inst == NULL) || (inst->type != XML_ELEMENT_NODE))
	return;

#ifdef XSLT_REFACTORED
    comp = (xsltStyleItemElementPtr) xsltNewStylePreComp(style, XSLT_FUNC_ELEMENT);
#else
    comp = xsltNewStylePreComp(style, XSLT_FUNC_ELEMENT);
#endif

    if (comp == NULL)
	return;
    inst->psvi = comp;
    comp->inst = inst;

    /*
    * Attribute "name".
    */
    /*
    * TODO: Precompile the AVT. See bug #344894.
    */
    comp->name = xsltEvalStaticAttrValueTemplate(style, inst,
	(const xmlChar *)"name", NULL, &comp->has_name);
    if (! comp->has_name) {
	xsltTransformError(NULL, style, inst,
	    "xsl:element: The attribute 'name' is missing.\n");
	style->errors++;
	goto error;
    }
    /*
    * Attribute "namespace".
    */
    /*
    * TODO: Precompile the AVT. See bug #344894.
    */
    comp->ns = xsltEvalStaticAttrValueTemplate(style, inst,
	(const xmlChar *)"namespace", NULL, &comp->has_ns);

    if (comp->name != NULL) {
	if (xmlValidateQName(comp->name, 0)) {
	    xsltTransformError(NULL, style, inst,
		"xsl:element: The value '%s' of the attribute 'name' is "
		"not a valid QName.\n", comp->name);
	    style->errors++;
	} else {
	    const xmlChar *prefix = NULL, *name;

	    name = xsltSplitQName(style->dict, comp->name, &prefix);
	    if (comp->has_ns == 0) {
		xmlNsPtr ns;

		/*
		* SPEC XSLT 1.0:
		*  "If the namespace attribute is not present, then the QName is
		*  expanded into an expanded-name using the namespace declarations
		*  in effect for the xsl:element element, including any default
		*  namespace declaration.
		*/
		ns = xmlSearchNs(inst->doc, inst, prefix);
		if (ns != NULL) {
		    comp->ns = xmlDictLookup(style->dict, ns->href, -1);
		    comp->has_ns = 1;
 #ifdef XSLT_REFACTORED
 		    comp->nsPrefix = prefix;
 		    comp->name = name;
 #endif
 		} else if (prefix != NULL) {
 		    xsltTransformError(NULL, style, inst,
			"xsl:element: The prefixed QName '%s' "
			"has no namespace binding in scope in the "
			"stylesheet; this is an error, since the namespace was "
			"not specified by the instruction itself.\n", comp->name);
		    style->errors++;
		}
	    }
	    if ((prefix != NULL) &&
		(!xmlStrncasecmp(prefix, (xmlChar *)"xml", 3)))
	    {
		/*
		* Mark is to be skipped.
		*/
		comp->has_name = 0;
	    }
	}
    }
    /*
    * Attribute "use-attribute-sets",
    */
    comp->use = xsltEvalStaticAttrValueTemplate(style, inst,
		       (const xmlChar *)"use-attribute-sets",
		       NULL, &comp->has_use);

error:
    return;
}
