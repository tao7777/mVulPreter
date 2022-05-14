xsltCompileIdKeyPattern(xsltParserContextPtr ctxt, xmlChar *name,
		int aid, int novar, xsltAxis axis) {
    xmlChar *lit = NULL;
    xmlChar *lit2 = NULL;

    if (CUR != '(') {
	xsltTransformError(NULL, NULL, NULL,
		"xsltCompileIdKeyPattern : ( expected\n");
	ctxt->error = 1;
	return;
    }
    if ((aid) && (xmlStrEqual(name, (const xmlChar *)"id"))) {
	if (axis != 0) {
	    xsltTransformError(NULL, NULL, NULL,
		    "xsltCompileIdKeyPattern : NodeTest expected\n");
	    ctxt->error = 1;
	    return;
	}
	NEXT;
	SKIP_BLANKS;
        lit = xsltScanLiteral(ctxt);
	if (ctxt->error) {
	    xsltTransformError(NULL, NULL, NULL,
		    "xsltCompileIdKeyPattern : Literal expected\n");
	    return;
	}
	SKIP_BLANKS;
	if (CUR != ')') {
	    xsltTransformError(NULL, NULL, NULL,
		    "xsltCompileIdKeyPattern : ) expected\n");
	    xmlFree(lit);
	    ctxt->error = 1;
	    return;
	}
	NEXT;
	PUSH(XSLT_OP_ID, lit, NULL, novar);
	lit = NULL;
    } else if ((aid) && (xmlStrEqual(name, (const xmlChar *)"key"))) {
	if (axis != 0) {
	    xsltTransformError(NULL, NULL, NULL,
		    "xsltCompileIdKeyPattern : NodeTest expected\n");
	    ctxt->error = 1;
	    return;
	}
	NEXT;
	SKIP_BLANKS;
        lit = xsltScanLiteral(ctxt);
	if (ctxt->error) {
	    xsltTransformError(NULL, NULL, NULL,
		    "xsltCompileIdKeyPattern : Literal expected\n");
	    return;
	}
	SKIP_BLANKS;
 	if (CUR != ',') {
 	    xsltTransformError(NULL, NULL, NULL,
 		    "xsltCompileIdKeyPattern : , expected\n");
	    xmlFree(lit);
 	    ctxt->error = 1;
 	    return;
 	}
	NEXT;
	SKIP_BLANKS;
        lit2 = xsltScanLiteral(ctxt);
	if (ctxt->error) {
	    xsltTransformError(NULL, NULL, NULL,
		    "xsltCompileIdKeyPattern : Literal expected\n");
	    xmlFree(lit);
	    return;
	}
	SKIP_BLANKS;
	if (CUR != ')') {
	    xsltTransformError(NULL, NULL, NULL,
		    "xsltCompileIdKeyPattern : ) expected\n");
	    xmlFree(lit);
	    xmlFree(lit2);
	    ctxt->error = 1;
	    return;
	}
	NEXT;
	/* URGENT TODO: support namespace in keys */
	PUSH(XSLT_OP_KEY, lit, lit2, novar);
	lit = NULL;
	lit2 = NULL;
    } else if (xmlStrEqual(name, (const xmlChar *)"processing-instruction")) {
	NEXT;
	SKIP_BLANKS;
	if (CUR != ')') {
	    lit = xsltScanLiteral(ctxt);
	    if (ctxt->error) {
		xsltTransformError(NULL, NULL, NULL,
			"xsltCompileIdKeyPattern : Literal expected\n");
		return;
	    }
	    SKIP_BLANKS;
	    if (CUR != ')') {
		xsltTransformError(NULL, NULL, NULL,
			"xsltCompileIdKeyPattern : ) expected\n");
		ctxt->error = 1;
		return;
	    }
	}
	NEXT;
	PUSH(XSLT_OP_PI, lit, NULL, novar);
	lit = NULL;
    } else if (xmlStrEqual(name, (const xmlChar *)"text")) {
	NEXT;
	SKIP_BLANKS;
	if (CUR != ')') {
	    xsltTransformError(NULL, NULL, NULL,
		    "xsltCompileIdKeyPattern : ) expected\n");
	    ctxt->error = 1;
	    return;
	}
	NEXT;
	PUSH(XSLT_OP_TEXT, NULL, NULL, novar);
    } else if (xmlStrEqual(name, (const xmlChar *)"comment")) {
	NEXT;
	SKIP_BLANKS;
	if (CUR != ')') {
	    xsltTransformError(NULL, NULL, NULL,
		    "xsltCompileIdKeyPattern : ) expected\n");
	    ctxt->error = 1;
	    return;
	}
	NEXT;
	PUSH(XSLT_OP_COMMENT, NULL, NULL, novar);
    } else if (xmlStrEqual(name, (const xmlChar *)"node")) {
	NEXT;
	SKIP_BLANKS;
	if (CUR != ')') {
	    xsltTransformError(NULL, NULL, NULL,
		    "xsltCompileIdKeyPattern : ) expected\n");
	    ctxt->error = 1;
	    return;
	}
	NEXT;
	if (axis == AXIS_ATTRIBUTE) {
	    PUSH(XSLT_OP_ATTR, NULL, NULL, novar);
	}
	else {
	    PUSH(XSLT_OP_NODE, NULL, NULL, novar);
	}
    } else if (aid) {
	xsltTransformError(NULL, NULL, NULL,
	    "xsltCompileIdKeyPattern : expecting 'key' or 'id' or node type\n");
	ctxt->error = 1;
	return;
    } else {
	xsltTransformError(NULL, NULL, NULL,
	    "xsltCompileIdKeyPattern : node type\n");
	ctxt->error = 1;
	return;
    }
error:
    return;
}
