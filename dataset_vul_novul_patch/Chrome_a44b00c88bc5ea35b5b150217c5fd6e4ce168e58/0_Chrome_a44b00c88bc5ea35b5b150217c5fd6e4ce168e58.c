 xmlXPathNextFollowing(xmlXPathParserContextPtr ctxt, xmlNodePtr cur) {
     if ((ctxt == NULL) || (ctxt->context == NULL)) return(NULL);
    if ((cur != NULL) && (cur->type  != XML_ATTRIBUTE_NODE) &&
        (cur->type != XML_NAMESPACE_DECL) && (cur->children != NULL))
        return(cur->children);

    if (cur == NULL) {
        cur = ctxt->context->node;
        if (cur->type == XML_NAMESPACE_DECL)
             return(NULL);
        if (cur->type == XML_ATTRIBUTE_NODE)
            cur = cur->parent;
     }
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
