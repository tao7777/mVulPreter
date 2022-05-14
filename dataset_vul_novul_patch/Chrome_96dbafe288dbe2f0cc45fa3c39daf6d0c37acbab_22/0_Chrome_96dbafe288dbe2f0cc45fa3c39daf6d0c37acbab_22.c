xsltNumberFormatGetMultipleLevel(xsltTransformContextPtr context,
				 xmlNodePtr node,
 				 xsltCompMatchPtr countPat,
 				 xsltCompMatchPtr fromPat,
 				 double *array,
				 int max)
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
 
	    if (xsltTestCompMatchCount(context, ancestor, countPat, node)) {
 		/* count(preceding-sibling::*) */
		cnt = 1;
		for (preceding =
                        xmlXPathNextPrecedingSibling(parser, ancestor);
 		     preceding != NULL;
 		     preceding =
 		        xmlXPathNextPrecedingSibling(parser, preceding)) {

	            if (xsltTestCompMatchCount(context, preceding, countPat,
                                               node))
			cnt++;
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
