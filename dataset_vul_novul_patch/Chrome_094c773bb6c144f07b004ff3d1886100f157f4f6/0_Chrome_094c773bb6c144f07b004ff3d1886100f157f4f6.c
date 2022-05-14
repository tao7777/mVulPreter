xsltCompilePatternInternal(const xmlChar *pattern, xmlDocPtr doc,
	           xmlNodePtr node, xsltStylesheetPtr style,
		   xsltTransformContextPtr runtime, int novar) {
    xsltParserContextPtr ctxt = NULL;
    xsltCompMatchPtr element, first = NULL, previous = NULL;
    int current, start, end, level, j;

    if (pattern == NULL) {
	xsltTransformError(NULL, NULL, node,
			 "xsltCompilePattern : NULL pattern\n");
	return(NULL);
    }

    ctxt = xsltNewParserContext(style, runtime);
    if (ctxt == NULL)
	return(NULL);
    ctxt->doc = doc;
    ctxt->elem = node;
    current = end = 0;
    while (pattern[current] != 0) {
	start = current;
	while (IS_BLANK_CH(pattern[current]))
	    current++;
	end = current;
	level = 0;
	while ((pattern[end] != 0) && ((pattern[end] != '|') || (level != 0))) {
	    if (pattern[end] == '[')
		level++;
	    else if (pattern[end] == ']')
		level--;
	    else if (pattern[end] == '\'') {
		end++;
		while ((pattern[end] != 0) && (pattern[end] != '\''))
		    end++;
	    } else if (pattern[end] == '"') {
		end++;
 		while ((pattern[end] != 0) && (pattern[end] != '"'))
 		    end++;
 	    }
      if (pattern[end] == 0)
          break;
 	    end++;
 	}
 	if (current == end) {
	    xsltTransformError(NULL, NULL, node,
			     "xsltCompilePattern : NULL pattern\n");
	    goto error;
	}
	element = xsltNewCompMatch();
	if (element == NULL) {
	    goto error;
	}
	if (first == NULL)
	    first = element;
	else if (previous != NULL)
	    previous->next = element;
	previous = element;

	ctxt->comp = element;
	ctxt->base = xmlStrndup(&pattern[start], end - start);
	if (ctxt->base == NULL)
	    goto error;
	ctxt->cur = &(ctxt->base)[current - start];
	element->pattern = ctxt->base;
	element->nsList = xmlGetNsList(doc, node);
	j = 0;
	if (element->nsList != NULL) {
	    while (element->nsList[j] != NULL)
		j++;
	}
	element->nsNr = j;


#ifdef WITH_XSLT_DEBUG_PATTERN
	xsltGenericDebug(xsltGenericDebugContext,
			 "xsltCompilePattern : parsing '%s'\n",
			 element->pattern);
#endif
	/*
	 Preset default priority to be zero.
	 This may be changed by xsltCompileLocationPathPattern.
	 */
	element->priority = 0;
	xsltCompileLocationPathPattern(ctxt, novar);
	if (ctxt->error) {
	    xsltTransformError(NULL, style, node,
			     "xsltCompilePattern : failed to compile '%s'\n",
			     element->pattern);
	    if (style != NULL) style->errors++;
	    goto error;
	}

	/*
	 * Reverse for faster interpretation.
	 */
	xsltReverseCompMatch(ctxt, element);

	/*
	 * Set-up the priority
	 */
	if (element->priority == 0) {	/* if not yet determined */
	    if (((element->steps[0].op == XSLT_OP_ELEM) ||
		 (element->steps[0].op == XSLT_OP_ATTR) ||
		 (element->steps[0].op == XSLT_OP_PI)) &&
		(element->steps[0].value != NULL) &&
		(element->steps[1].op == XSLT_OP_END)) {
		;	/* previously preset */
	    } else if ((element->steps[0].op == XSLT_OP_ATTR) &&
		       (element->steps[0].value2 != NULL) &&
		       (element->steps[1].op == XSLT_OP_END)) {
			element->priority = -0.25;
	    } else if ((element->steps[0].op == XSLT_OP_NS) &&
		       (element->steps[0].value != NULL) &&
		       (element->steps[1].op == XSLT_OP_END)) {
			element->priority = -0.25;
	    } else if ((element->steps[0].op == XSLT_OP_ATTR) &&
		       (element->steps[0].value == NULL) &&
		       (element->steps[0].value2 == NULL) &&
		       (element->steps[1].op == XSLT_OP_END)) {
			element->priority = -0.5;
	    } else if (((element->steps[0].op == XSLT_OP_PI) ||
		       (element->steps[0].op == XSLT_OP_TEXT) ||
		       (element->steps[0].op == XSLT_OP_ALL) ||
		       (element->steps[0].op == XSLT_OP_NODE) ||
		       (element->steps[0].op == XSLT_OP_COMMENT)) &&
		       (element->steps[1].op == XSLT_OP_END)) {
			element->priority = -0.5;
	    } else {
		element->priority = 0.5;
	    }
	}
#ifdef WITH_XSLT_DEBUG_PATTERN
	xsltGenericDebug(xsltGenericDebugContext,
		     "xsltCompilePattern : parsed %s, default priority %f\n",
			 element->pattern, element->priority);
#endif
	if (pattern[end] == '|')
	    end++;
	current = end;
    }
    if (end == 0) {
	xsltTransformError(NULL, style, node,
			 "xsltCompilePattern : NULL pattern\n");
	if (style != NULL) style->errors++;
	goto error;
    }

    xsltFreeParserContext(ctxt);
    return(first);

error:
    if (ctxt != NULL)
	xsltFreeParserContext(ctxt);
    if (first != NULL)
	xsltFreeCompMatchList(first);
    return(NULL);
}
