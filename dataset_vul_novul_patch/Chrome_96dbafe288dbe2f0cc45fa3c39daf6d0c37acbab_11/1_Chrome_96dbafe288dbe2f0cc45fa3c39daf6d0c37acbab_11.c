xsltParseStylesheetAttributeSet(xsltStylesheetPtr style, xmlNodePtr cur) {
    const xmlChar *ncname;
    const xmlChar *prefix;
    xmlChar *value;
    xmlNodePtr child;
    xsltAttrElemPtr attrItems;

    if ((cur == NULL) || (style == NULL) || (cur->type != XML_ELEMENT_NODE))
 	return;
 
     value = xmlGetNsProp(cur, (const xmlChar *)"name", NULL);
    if (value == NULL) {
 	xsltGenericError(xsltGenericErrorContext,
 	     "xsl:attribute-set : name is missing\n");
 	return;
     }
 
    ncname = xsltSplitQName(style->dict, value, &prefix);
    xmlFree(value);
    value = NULL;

    if (style->attributeSets == NULL) {
#ifdef WITH_XSLT_DEBUG_ATTRIBUTES
	xsltGenericDebug(xsltGenericDebugContext,
	    "creating attribute set table\n");
#endif
	style->attributeSets = xmlHashCreate(10);
    }
    if (style->attributeSets == NULL)
	return;

    attrItems = xmlHashLookup2(style->attributeSets, ncname, prefix);

    /*
    * Parse the content. Only xsl:attribute elements are allowed.
    */
    child = cur->children;
    while (child != NULL) {
	/*
	* Report invalid nodes.
	*/
	if ((child->type != XML_ELEMENT_NODE) ||
	    (child->ns == NULL) ||
	    (! IS_XSLT_ELEM(child)))
	{
	    if (child->type == XML_ELEMENT_NODE)
		xsltTransformError(NULL, style, child,
			"xsl:attribute-set : unexpected child %s\n",
		                 child->name);
	    else
		xsltTransformError(NULL, style, child,
			"xsl:attribute-set : child of unexpected type\n");
	} else if (!IS_XSLT_NAME(child, "attribute")) {
	    xsltTransformError(NULL, style, child,
		"xsl:attribute-set : unexpected child xsl:%s\n",
		child->name);
	} else {
#ifdef XSLT_REFACTORED
	    xsltAttrElemPtr nextAttr, curAttr;

	    /*
	    * Process xsl:attribute
	    * ---------------------
	    */

#ifdef WITH_XSLT_DEBUG_ATTRIBUTES
	    xsltGenericDebug(xsltGenericDebugContext,
		"add attribute to list %s\n", ncname);
#endif
	    /*
	    * The following was taken over from
	    * xsltAddAttrElemList().
	    */
	    if (attrItems == NULL) {
		attrItems = xsltNewAttrElem(child);
	    } else {
		curAttr = attrItems;
		while (curAttr != NULL) {
		    nextAttr = curAttr->next;
		    if (curAttr->attr == child) {
			/*
			* URGENT TODO: Can somebody explain
			*  why attrItems is set to curAttr
			*  here? Is this somehow related to
			*  avoidance of recursions?
			*/
			attrItems = curAttr;
			goto next_child;
		    }
		    if (curAttr->next == NULL)
			curAttr->next = xsltNewAttrElem(child);
		    curAttr = nextAttr;
		}
	    }
	    /*
	    * Parse the xsl:attribute and its content.
	    */
	    xsltParseAnyXSLTElem(XSLT_CCTXT(style), child);
#else
#ifdef WITH_XSLT_DEBUG_ATTRIBUTES
	    xsltGenericDebug(xsltGenericDebugContext,
		"add attribute to list %s\n", ncname);
#endif
	    /*
	    * OLD behaviour:
	    */
	    attrItems = xsltAddAttrElemList(attrItems, child);
#endif
	}

#ifdef XSLT_REFACTORED
next_child:
#endif
	child = child->next;
    }

    /*
    * Process attribue "use-attribute-sets".
    */
    /* TODO check recursion */
    value = xmlGetNsProp(cur, (const xmlChar *)"use-attribute-sets",
	NULL);
    if (value != NULL) {
	const xmlChar *curval, *endval;
	curval = value;
	while (*curval != 0) {
	    while (IS_BLANK(*curval)) curval++;
	    if (*curval == 0)
		break;
	    endval = curval;
	    while ((*endval != 0) && (!IS_BLANK(*endval))) endval++;
	    curval = xmlDictLookup(style->dict, curval, endval - curval);
	    if (curval) {
		const xmlChar *ncname2 = NULL;
		const xmlChar *prefix2 = NULL;
		xsltAttrElemPtr refAttrItems;

#ifdef WITH_XSLT_DEBUG_ATTRIBUTES
		xsltGenericDebug(xsltGenericDebugContext,
		    "xsl:attribute-set : %s adds use %s\n", ncname, curval);
#endif
		ncname2 = xsltSplitQName(style->dict, curval, &prefix2);
		refAttrItems = xsltNewAttrElem(NULL);
		if (refAttrItems != NULL) {
		    refAttrItems->set = ncname2;
		    refAttrItems->ns = prefix2;
		    attrItems = xsltMergeAttrElemList(style,
			attrItems, refAttrItems);
		    xsltFreeAttrElem(refAttrItems);
		}
	    }
	    curval = endval;
	}
	xmlFree(value);
	value = NULL;
    }

    /*
     * Update the value
     */
    /*
    * TODO: Why is this dummy entry needed.?
    */
    if (attrItems == NULL)
	attrItems = xsltNewAttrElem(NULL);
    xmlHashUpdateEntry2(style->attributeSets, ncname, prefix, attrItems, NULL);
#ifdef WITH_XSLT_DEBUG_ATTRIBUTES
    xsltGenericDebug(xsltGenericDebugContext,
	"updated attribute list %s\n", ncname);
#endif
}
