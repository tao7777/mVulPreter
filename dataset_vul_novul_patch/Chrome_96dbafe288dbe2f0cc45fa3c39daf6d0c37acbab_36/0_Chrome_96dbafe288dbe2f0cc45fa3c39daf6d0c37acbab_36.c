xsltCopyTextString(xsltTransformContextPtr ctxt, xmlNodePtr target,
	           const xmlChar *string, int noescape)
{
    xmlNodePtr copy;
    int len;

    if (string == NULL)
	return(NULL);

#ifdef WITH_XSLT_DEBUG_PROCESS
    XSLT_TRACE(ctxt,XSLT_TRACE_COPY_TEXT,xsltGenericDebug(xsltGenericDebugContext,
		     "xsltCopyTextString: copy text %s\n",
		     string));
#endif

    /*
    * Play safe and reset the merging mechanism for every new
    * target node.
    */
    if ((target == NULL) || (target->children == NULL)) {
	ctxt->lasttext = NULL;
    }

    /* handle coalescing of text nodes here */
    len = xmlStrlen(string);
    if ((ctxt->type == XSLT_OUTPUT_XML) &&
	(ctxt->style->cdataSection != NULL) &&
	(target != NULL) &&
	(target->type == XML_ELEMENT_NODE) &&
	(((target->ns == NULL) &&
	  (xmlHashLookup2(ctxt->style->cdataSection,
		          target->name, NULL) != NULL)) ||
	 ((target->ns != NULL) &&
	  (xmlHashLookup2(ctxt->style->cdataSection,
	                  target->name, target->ns->href) != NULL))))
    {
	/*
	* Process "cdata-section-elements".
	*/
	if ((target->last != NULL) &&
	    (target->last->type == XML_CDATA_SECTION_NODE))
	{
	    return(xsltAddTextString(ctxt, target->last, string, len));
	}
	copy = xmlNewCDataBlock(ctxt->output, string, len);
    } else if (noescape) {
	/*
	* Process "disable-output-escaping".
	*/
	if ((target != NULL) && (target->last != NULL) &&
	    (target->last->type == XML_TEXT_NODE) &&
	    (target->last->name == xmlStringTextNoenc))
	{
	    return(xsltAddTextString(ctxt, target->last, string, len));
	}
	copy = xmlNewTextLen(string, len);
	if (copy != NULL)
	    copy->name = xmlStringTextNoenc;
    } else {
	/*
	* Default processing.
	*/
	if ((target != NULL) && (target->last != NULL) &&
	    (target->last->type == XML_TEXT_NODE) &&
	    (target->last->name == xmlStringText)) {
	    return(xsltAddTextString(ctxt, target->last, string, len));
 	}
 	copy = xmlNewTextLen(string, len);
     }
    if (copy != NULL && target != NULL)
	copy = xsltAddChild(target, copy);
     if (copy != NULL) {
 	ctxt->lasttext = copy->content;
 	ctxt->lasttsize = len;
 	ctxt->lasttuse = len;
    } else {
	xsltTransformError(ctxt, NULL, target,
			 "xsltCopyTextString: text copy failed\n");
	ctxt->lasttext = NULL;
    }
    return(copy);
}
