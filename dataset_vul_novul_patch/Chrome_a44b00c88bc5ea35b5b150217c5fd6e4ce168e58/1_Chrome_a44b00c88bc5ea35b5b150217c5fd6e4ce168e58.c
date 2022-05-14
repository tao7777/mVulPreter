 xmlXPathNextFollowing(xmlXPathParserContextPtr ctxt, xmlNodePtr cur) {
     if ((ctxt == NULL) || (ctxt->context == NULL)) return(NULL);
    if ((ctxt->context->node->type == XML_ATTRIBUTE_NODE) ||
	(ctxt->context->node->type == XML_NAMESPACE_DECL))
	return(NULL);
    if (cur != NULL) {
        if ((cur->type == XML_ATTRIBUTE_NODE) ||
            (cur->type == XML_NAMESPACE_DECL))
             return(NULL);
        if (cur->children != NULL)
            return cur->children ;
     }
    if (cur == NULL) cur = ctxt->context->node;
     if (cur == NULL) return(NULL) ; /* ERROR */
     if (cur->next != NULL) return(cur->next) ;
     do {
        cur = cur->parent;
        if (cur == NULL) break;
        if (cur == (xmlNodePtr) ctxt->context->doc) return(NULL);
        if (cur->next != NULL) return(cur->next);
    } while (cur != NULL);
    return(cur);
}
