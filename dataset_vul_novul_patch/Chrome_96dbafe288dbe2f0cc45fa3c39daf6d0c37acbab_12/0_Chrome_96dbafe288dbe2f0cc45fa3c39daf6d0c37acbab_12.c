xsltResolveSASCallback(xsltAttrElemPtr values, xsltStylesheetPtr style,
xsltResolveSASCallbackInt(xsltAttrElemPtr values, xsltStylesheetPtr style,
 	               const xmlChar *name, const xmlChar *ns,
		       int depth) {
     xsltAttrElemPtr tmp;
     xsltAttrElemPtr refs;
 
     tmp = values;
    if ((name == NULL) || (name[0] == 0))
        return;
    if (depth > 100) {
	xsltGenericError(xsltGenericErrorContext,
	"xsl:attribute-set : use-attribute-sets recursion detected on %s\n",
			 name);
	return;
    }
     while (tmp != NULL) {
 	if (tmp->set != NULL) {
 	    /*
	     * Check against cycles !
	     */
	    if ((xmlStrEqual(name, tmp->set)) && (xmlStrEqual(ns, tmp->ns))) {
		xsltGenericError(xsltGenericErrorContext,
     "xsl:attribute-set : use-attribute-sets recursion detected on %s\n",
                                 name);
	    } else {
#ifdef WITH_XSLT_DEBUG_ATTRIBUTES
		xsltGenericDebug(xsltGenericDebugContext,
			"Importing attribute list %s\n", tmp->set);
#endif

		refs = xsltGetSAS(style, tmp->set, tmp->ns);
		if (refs == NULL) {
		    xsltGenericError(xsltGenericErrorContext,
     "xsl:attribute-set : use-attribute-sets %s reference missing %s\n",
				     name, tmp->set);
		} else {
 		    /*
 		     * recurse first for cleanup
 		     */
		    xsltResolveSASCallbackInt(refs, style, name, ns, depth + 1);
 		    /*
 		     * Then merge
 		     */
		    xsltMergeAttrElemList(style, values, refs);
		    /*
		     * Then suppress the reference
		     */
		    tmp->set = NULL;
		    tmp->ns = NULL;
		}
	    }
	}
	tmp = tmp->next;
     }
 }
