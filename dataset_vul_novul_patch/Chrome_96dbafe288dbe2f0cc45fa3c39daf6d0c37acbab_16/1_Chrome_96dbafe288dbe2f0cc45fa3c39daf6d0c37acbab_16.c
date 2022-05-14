xsltFindTemplate(xsltTransformContextPtr ctxt, const xmlChar *name,
	         const xmlChar *nameURI) {
    xsltTemplatePtr cur;
    xsltStylesheetPtr style;

    if ((ctxt == NULL) || (name == NULL))
 	return(NULL);
     style = ctxt->style;
     while (style != NULL) {
	cur = style->templates;
	while (cur != NULL) {
	    if (xmlStrEqual(name, cur->name)) {
		if (((nameURI == NULL) && (cur->nameURI == NULL)) ||
		    ((nameURI != NULL) && (cur->nameURI != NULL) &&
		     (xmlStrEqual(nameURI, cur->nameURI)))) {
		    return(cur);
		}
	    }
	    cur = cur->next;
	}
 
 	style = xsltNextImport(style);
     }
    return(NULL);
}
