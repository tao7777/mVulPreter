xsltReverseCompMatch(xsltParserContextPtr ctxt, xsltCompMatchPtr comp) {
    int i = 0;
    int j = comp->nbStep - 1;

    while (j > i) {
	register xmlChar *tmp;
	register xsltOp op;
	register xmlXPathCompExprPtr expr;
	register int t;

	tmp = comp->steps[i].value;
	comp->steps[i].value = comp->steps[j].value;
	comp->steps[j].value = tmp;
	tmp = comp->steps[i].value2;
	comp->steps[i].value2 = comp->steps[j].value2;
	comp->steps[j].value2 = tmp;
	tmp = comp->steps[i].value3;
	comp->steps[i].value3 = comp->steps[j].value3;
	comp->steps[j].value3 = tmp;
	op = comp->steps[i].op;
	comp->steps[i].op = comp->steps[j].op;
	comp->steps[j].op = op;
	expr = comp->steps[i].comp;
	comp->steps[i].comp = comp->steps[j].comp;
	comp->steps[j].comp = expr;
	t = comp->steps[i].previousExtra;
	comp->steps[i].previousExtra = comp->steps[j].previousExtra;
	comp->steps[j].previousExtra = t;
	t = comp->steps[i].indexExtra;
	comp->steps[i].indexExtra = comp->steps[j].indexExtra;
	comp->steps[j].indexExtra = t;
	t = comp->steps[i].lenExtra;
	comp->steps[i].lenExtra = comp->steps[j].lenExtra;
	comp->steps[j].lenExtra = t;
	j--;
	i++;
    }
     xsltCompMatchAdd(ctxt, comp, XSLT_OP_END, NULL, NULL, 0);
 
     /*
     * Detect consecutive XSLT_OP_PREDICATE and predicates on ops which
     * haven't been optimized yet indicating a direct matching should be done.
      */
     for (i = 0;i < comp->nbStep - 1;i++) {
        xsltOp op = comp->steps[i].op;

        if ((op != XSLT_OP_ELEM) &&
            (op != XSLT_OP_ALL) &&
 	    (comp->steps[i + 1].op == XSLT_OP_PREDICATE)) {
 
 	    comp->direct = 1;
	    if (comp->pattern[0] != '/') {
		xmlChar *query;

		query = xmlStrdup((const xmlChar *)"//");
		query = xmlStrcat(query, comp->pattern);

		xmlFree((xmlChar *) comp->pattern);
		comp->pattern = query;
	    }
	    break;
	}
    }
}
