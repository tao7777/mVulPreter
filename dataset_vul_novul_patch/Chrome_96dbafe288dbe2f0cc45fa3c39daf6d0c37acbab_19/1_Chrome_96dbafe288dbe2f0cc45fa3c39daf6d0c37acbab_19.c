xsltNumberFormat(xsltTransformContextPtr ctxt,
		 xsltNumberDataPtr data,
		 xmlNodePtr node)
{
    xmlBufferPtr output = NULL;
     int amount, i;
     double number;
     xsltFormat tokens;
    int tempformat = 0;
 
    if ((data->format == NULL) && (data->has_format != 0)) {
	data->format = xsltEvalAttrValueTemplate(ctxt, data->node,
 					     (const xmlChar *) "format",
 					     XSLT_NAMESPACE);
	tempformat = 1;
    }
    if (data->format == NULL) {
	return;
     }
 
     output = xmlBufferCreate();
     if (output == NULL)
 	goto XSLT_NUMBER_FORMAT_END;
 
    xsltNumberFormatTokenize(data->format, &tokens);
     /*
      * Evaluate the XPath expression to find the value(s)
      */
    if (data->value) {
	amount = xsltNumberFormatGetValue(ctxt->xpathCtxt,
					  node,
					  data->value,
					  &number);
	if (amount == 1) {
	    xsltNumberFormatInsertNumbers(data,
					  &number,
					  1,
					  &tokens,
					  output);
	}

    } else if (data->level) {

	if (xmlStrEqual(data->level, (const xmlChar *) "single")) {
	    amount = xsltNumberFormatGetMultipleLevel(ctxt,
						      node,
 						      data->countPat,
 						      data->fromPat,
 						      &number,
						      1,
						      data->doc,
						      data->node);
 	    if (amount == 1) {
 		xsltNumberFormatInsertNumbers(data,
 					      &number,
					      1,
					      &tokens,
					      output);
	    }
	} else if (xmlStrEqual(data->level, (const xmlChar *) "multiple")) {
	    double numarray[1024];
	    int max = sizeof(numarray)/sizeof(numarray[0]);
	    amount = xsltNumberFormatGetMultipleLevel(ctxt,
						      node,
 						      data->countPat,
 						      data->fromPat,
 						      numarray,
						      max,
						      data->doc,
						      data->node);
 	    if (amount > 0) {
 		xsltNumberFormatInsertNumbers(data,
 					      numarray,
					      amount,
					      &tokens,
					      output);
	    }
	} else if (xmlStrEqual(data->level, (const xmlChar *) "any")) {
	    amount = xsltNumberFormatGetAnyLevel(ctxt,
 						 node,
 						 data->countPat,
 						 data->fromPat,
						 &number,
						 data->doc,
						 data->node);
 	    if (amount > 0) {
 		xsltNumberFormatInsertNumbers(data,
 					      &number,
					      1,
					      &tokens,
					      output);
	    }
	}
    }
     /* Insert number as text node */
     xsltCopyTextString(ctxt, ctxt->insert, xmlBufferContent(output), 0);
 
     if (tokens.start != NULL)
 	xmlFree(tokens.start);
     if (tokens.end != NULL)
	xmlFree(tokens.end);
    for (i = 0;i < tokens.nTokens;i++) {
 	if (tokens.tokens[i].separator != NULL)
 	    xmlFree(tokens.tokens[i].separator);
     }
XSLT_NUMBER_FORMAT_END:
    if (tempformat == 1) {
	/* The format need to be recomputed each time */
	data->format = NULL;
    }
    if (output != NULL)
	xmlBufferFree(output);
 }
