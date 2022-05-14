xsltStylePreCompute(xsltStylesheetPtr style, xmlNodePtr inst) {
    /*
    * URGENT TODO: Normally inst->psvi Should never be reserved here,
    *   BUT: since if we include the same stylesheet from
    *   multiple imports, then the stylesheet will be parsed
    *   again. We simply must not try to compute the stylesheet again.
    * TODO: Get to the point where we don't need to query the
    *   namespace- and local-name of the node, but can evaluate this
    *   using cctxt->style->inode->category;
    */
    if ((inst == NULL) || (inst->type != XML_ELEMENT_NODE) ||
        (inst->psvi != NULL))
	return;

    if (IS_XSLT_ELEM(inst)) {
	xsltStylePreCompPtr cur;

	if (IS_XSLT_NAME(inst, "apply-templates")) {
	    xsltCheckInstructionElement(style, inst);
	    xsltApplyTemplatesComp(style, inst);
	} else if (IS_XSLT_NAME(inst, "with-param")) {
	    xsltCheckParentElement(style, inst, BAD_CAST "apply-templates",
	                           BAD_CAST "call-template");
	    xsltWithParamComp(style, inst);
	} else if (IS_XSLT_NAME(inst, "value-of")) {
	    xsltCheckInstructionElement(style, inst);
	    xsltValueOfComp(style, inst);
	} else if (IS_XSLT_NAME(inst, "copy")) {
	    xsltCheckInstructionElement(style, inst);
	    xsltCopyComp(style, inst);
	} else if (IS_XSLT_NAME(inst, "copy-of")) {
	    xsltCheckInstructionElement(style, inst);
	    xsltCopyOfComp(style, inst);
	} else if (IS_XSLT_NAME(inst, "if")) {
	    xsltCheckInstructionElement(style, inst);
	    xsltIfComp(style, inst);
	} else if (IS_XSLT_NAME(inst, "when")) {
	    xsltCheckParentElement(style, inst, BAD_CAST "choose", NULL);
	    xsltWhenComp(style, inst);
	} else if (IS_XSLT_NAME(inst, "choose")) {
	    xsltCheckInstructionElement(style, inst);
	    xsltChooseComp(style, inst);
	} else if (IS_XSLT_NAME(inst, "for-each")) {
	    xsltCheckInstructionElement(style, inst);
	    xsltForEachComp(style, inst);
	} else if (IS_XSLT_NAME(inst, "apply-imports")) {
	    xsltCheckInstructionElement(style, inst);
	    xsltApplyImportsComp(style, inst);
 	} else if (IS_XSLT_NAME(inst, "attribute")) {
 	    xmlNodePtr parent = inst->parent;
 
	    if ((parent == NULL) || (parent->ns == NULL) ||
 		((parent->ns != inst->ns) &&
 		 (!xmlStrEqual(parent->ns->href, inst->ns->href))) ||
 		(!xmlStrEqual(parent->name, BAD_CAST "attribute-set"))) {
		xsltCheckInstructionElement(style, inst);
	    }
	    xsltAttributeComp(style, inst);
	} else if (IS_XSLT_NAME(inst, "element")) {
	    xsltCheckInstructionElement(style, inst);
	    xsltElementComp(style, inst);
	} else if (IS_XSLT_NAME(inst, "text")) {
	    xsltCheckInstructionElement(style, inst);
	    xsltTextComp(style, inst);
	} else if (IS_XSLT_NAME(inst, "sort")) {
	    xsltCheckParentElement(style, inst, BAD_CAST "apply-templates",
	                           BAD_CAST "for-each");
	    xsltSortComp(style, inst);
	} else if (IS_XSLT_NAME(inst, "comment")) {
	    xsltCheckInstructionElement(style, inst);
	    xsltCommentComp(style, inst);
	} else if (IS_XSLT_NAME(inst, "number")) {
	    xsltCheckInstructionElement(style, inst);
	    xsltNumberComp(style, inst);
	} else if (IS_XSLT_NAME(inst, "processing-instruction")) {
	    xsltCheckInstructionElement(style, inst);
	    xsltProcessingInstructionComp(style, inst);
	} else if (IS_XSLT_NAME(inst, "call-template")) {
	    xsltCheckInstructionElement(style, inst);
	    xsltCallTemplateComp(style, inst);
	} else if (IS_XSLT_NAME(inst, "param")) {
	    if (xsltCheckTopLevelElement(style, inst, 0) == 0)
	        xsltCheckInstructionElement(style, inst);
	    xsltParamComp(style, inst);
	} else if (IS_XSLT_NAME(inst, "variable")) {
	    if (xsltCheckTopLevelElement(style, inst, 0) == 0)
	        xsltCheckInstructionElement(style, inst);
	    xsltVariableComp(style, inst);
	} else if (IS_XSLT_NAME(inst, "otherwise")) {
	    xsltCheckParentElement(style, inst, BAD_CAST "choose", NULL);
	    xsltCheckInstructionElement(style, inst);
	    return;
	} else if (IS_XSLT_NAME(inst, "template")) {
	    xsltCheckTopLevelElement(style, inst, 1);
	    return;
	} else if (IS_XSLT_NAME(inst, "output")) {
	    xsltCheckTopLevelElement(style, inst, 1);
	    return;
	} else if (IS_XSLT_NAME(inst, "preserve-space")) {
	    xsltCheckTopLevelElement(style, inst, 1);
	    return;
	} else if (IS_XSLT_NAME(inst, "strip-space")) {
	    xsltCheckTopLevelElement(style, inst, 1);
	    return;
	} else if ((IS_XSLT_NAME(inst, "stylesheet")) ||
	           (IS_XSLT_NAME(inst, "transform"))) {
	    xmlNodePtr parent = inst->parent;

	    if ((parent == NULL) || (parent->type != XML_DOCUMENT_NODE)) {
		xsltTransformError(NULL, style, inst,
		    "element %s only allowed only as root element\n",
				   inst->name);
		style->errors++;
	    }
	    return;
	} else if (IS_XSLT_NAME(inst, "key")) {
	    xsltCheckTopLevelElement(style, inst, 1);
	    return;
	} else if (IS_XSLT_NAME(inst, "message")) {
	    xsltCheckInstructionElement(style, inst);
	    return;
	} else if (IS_XSLT_NAME(inst, "attribute-set")) {
	    xsltCheckTopLevelElement(style, inst, 1);
	    return;
	} else if (IS_XSLT_NAME(inst, "namespace-alias")) {
	    xsltCheckTopLevelElement(style, inst, 1);
	    return;
	} else if (IS_XSLT_NAME(inst, "include")) {
	    xsltCheckTopLevelElement(style, inst, 1);
	    return;
	} else if (IS_XSLT_NAME(inst, "import")) {
	    xsltCheckTopLevelElement(style, inst, 1);
	    return;
	} else if (IS_XSLT_NAME(inst, "decimal-format")) {
	    xsltCheckTopLevelElement(style, inst, 1);
	    return;
	} else if (IS_XSLT_NAME(inst, "fallback")) {
	    xsltCheckInstructionElement(style, inst);
	    return;
	} else if (IS_XSLT_NAME(inst, "document")) {
	    xsltCheckInstructionElement(style, inst);
	    inst->psvi = (void *) xsltDocumentComp(style, inst,
				(xsltTransformFunction) xsltDocumentElem);
	} else {
	    xsltTransformError(NULL, style, inst,
		 "xsltStylePreCompute: unknown xsl:%s\n", inst->name);
	    if (style != NULL) style->warnings++;
	}

	cur = (xsltStylePreCompPtr) inst->psvi;
	/*
	* A ns-list is build for every XSLT item in the
	* node-tree. This is needed for XPath expressions.
	*/
	if (cur != NULL) {
	    int i = 0;

	    cur->nsList = xmlGetNsList(inst->doc, inst);
            if (cur->nsList != NULL) {
		while (cur->nsList[i] != NULL)
		    i++;
	    }
	    cur->nsNr = i;
	}
    } else {
	inst->psvi =
	    (void *) xsltPreComputeExtModuleElement(style, inst);

	/*
	 * Unknown element, maybe registered at the context
	 * level. Mark it for later recognition.
	 */
	if (inst->psvi == NULL)
	    inst->psvi = (void *) xsltExtMarker;
    }
}
