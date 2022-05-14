xsltNumberFormatGetMultipleLevel(xsltTransformContextPtr context,
				 xmlNodePtr node,
 				 xsltCompMatchPtr countPat,
 				 xsltCompMatchPtr fromPat,
 				 double *array,
				 int max,
				 xmlDocPtr doc,
				 xmlNodePtr elem)
 {
     int amount = 0;
     int cnt;
    xmlNodePtr ancestor;
    xmlNodePtr preceding;
    xmlXPathParserContextPtr parser;

    context->xpathCtxt->node = node;
    parser = xmlXPathNewParserContext(NULL, context->xpathCtxt);
    if (parser) {
	/* ancestor-or-self::*[count] */
	for (ancestor = node;
	     (ancestor != NULL) && (ancestor->type != XML_DOCUMENT_NODE);
	     ancestor = xmlXPathNextAncestor(parser, ancestor)) {

	    if ((fromPat != NULL) &&
 		xsltTestCompMatchList(context, ancestor, fromPat))
 		break; /* for */
 
	    if ((countPat == NULL && node->type == ancestor->type &&
		xmlStrEqual(node->name, ancestor->name)) ||
		xsltTestCompMatchList(context, ancestor, countPat)) {
 		/* count(preceding-sibling::*) */
		cnt = 0;
		for (preceding = ancestor;
 		     preceding != NULL;
 		     preceding =
 		        xmlXPathNextPrecedingSibling(parser, preceding)) {
		    if (countPat == NULL) {
			if ((preceding->type == ancestor->type) &&
			    xmlStrEqual(preceding->name, ancestor->name)){
			    if ((preceding->ns == ancestor->ns) ||
			        ((preceding->ns != NULL) &&
				 (ancestor->ns != NULL) &&
			         (xmlStrEqual(preceding->ns->href,
			             ancestor->ns->href) )))
			        cnt++;
			}
		    } else {
			if (xsltTestCompMatchList(context, preceding,
				                  countPat))
			    cnt++;
		    }
 		}
 		array[amount++] = (double)cnt;
 		if (amount >= max)
		    break; /* for */
	    }
	}
	xmlXPathFreeParserContext(parser);
    }
    return amount;
}
