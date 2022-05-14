 xmlXPathNextPreceding(xmlXPathParserContextPtr ctxt, xmlNodePtr cur)
 {
     if ((ctxt == NULL) || (ctxt->context == NULL)) return(NULL);
    if (cur == NULL) {
         cur = ctxt->context->node;
        if (cur->type == XML_NAMESPACE_DECL)
            return(NULL);
        if (cur->type == XML_ATTRIBUTE_NODE)
            return(cur->parent);
    }
     if (cur == NULL)
 	return (NULL);
     if ((cur->prev != NULL) && (cur->prev->type == XML_DTD_NODE))
	cur = cur->prev;
    do {
        if (cur->prev != NULL) {
            for (cur = cur->prev; cur->last != NULL; cur = cur->last) ;
            return (cur);
        }

        cur = cur->parent;
        if (cur == NULL)
            return (NULL);
        if (cur == ctxt->context->doc->children)
            return (NULL);
    } while (xmlXPathIsAncestor(cur, ctxt->context->node));
    return (cur);
}
