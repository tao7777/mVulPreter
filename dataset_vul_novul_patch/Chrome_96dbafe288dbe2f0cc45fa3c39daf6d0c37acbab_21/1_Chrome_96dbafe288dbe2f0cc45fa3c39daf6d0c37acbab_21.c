 xsltNumberFormatGetAnyLevel(xsltTransformContextPtr context,
 			    xmlNodePtr node,
 			    xsltCompMatchPtr countPat,
 			    xsltCompMatchPtr fromPat,
			    double *array,
			    xmlDocPtr doc,
			    xmlNodePtr elem)
 {
     int amount = 0;
     int cnt = 0;
    xmlNodePtr cur;

    /* select the starting node */
    switch (node->type) {
	case XML_ELEMENT_NODE:
	    cur = node;
	    break;
	case XML_ATTRIBUTE_NODE:
	    cur = ((xmlAttrPtr) node)->parent;
	    break;
	case XML_TEXT_NODE:
	case XML_PI_NODE:
	case XML_COMMENT_NODE:
	    cur = node->parent;
	    break;
	default:
	    cur = NULL;
	    break;
    }
 
     while (cur != NULL) {
 	/* process current node */
	if (countPat == NULL) {
	    if ((node->type == cur->type) &&
		/* FIXME: must use expanded-name instead of local name */
		xmlStrEqual(node->name, cur->name)) {
		    if ((node->ns == cur->ns) ||
		        ((node->ns != NULL) &&
			 (cur->ns != NULL) &&
		         (xmlStrEqual(node->ns->href,
		             cur->ns->href) )))
		        cnt++;
	    }
	} else {
	    if (xsltTestCompMatchList(context, cur, countPat))
		cnt++;
	}
 	if ((fromPat != NULL) &&
 	    xsltTestCompMatchList(context, cur, fromPat)) {
 	    break; /* while */
	}

	/* Skip to next preceding or ancestor */
	if ((cur->type == XML_DOCUMENT_NODE) ||
#ifdef LIBXML_DOCB_ENABLED
            (cur->type == XML_DOCB_DOCUMENT_NODE) ||
#endif
            (cur->type == XML_HTML_DOCUMENT_NODE))
	    break; /* while */

	while ((cur->prev != NULL) && ((cur->prev->type == XML_DTD_NODE) ||
	       (cur->prev->type == XML_XINCLUDE_START) ||
	       (cur->prev->type == XML_XINCLUDE_END)))
	    cur = cur->prev;
	if (cur->prev != NULL) {
	    for (cur = cur->prev; cur->last != NULL; cur = cur->last);
	} else {
	    cur = cur->parent;
	}

    }

    array[amount++] = (double) cnt;

    return(amount);
}
