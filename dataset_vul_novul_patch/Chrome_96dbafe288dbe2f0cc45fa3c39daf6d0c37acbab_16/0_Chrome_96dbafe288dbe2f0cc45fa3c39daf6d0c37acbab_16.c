xsltFindTemplate(xsltTransformContextPtr ctxt, const xmlChar *name,
	         const xmlChar *nameURI) {
    xsltTemplatePtr cur;
    xsltStylesheetPtr style;

    if ((ctxt == NULL) || (name == NULL))
 	return(NULL);
     style = ctxt->style;
     while (style != NULL) {
        if (style->namedTemplates != NULL) {
            cur = (xsltTemplatePtr)
                xmlHashLookup2(style->namedTemplates, name, nameURI);
            if (cur != NULL)
                return(cur);
        }
 
 	style = xsltNextImport(style);
     }
    return(NULL);
}
