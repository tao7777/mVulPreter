xsltCompileLocationPathPattern(xsltParserContextPtr ctxt, int novar) {
    SKIP_BLANKS;
    if ((CUR == '/') && (NXT(1) == '/')) {
	/*
	 * since we reverse the query
	 * a leading // can be safely ignored
	 */
	NEXT;
	NEXT;
	ctxt->comp->priority = 0.5;	/* '//' means not 0 priority */
	xsltCompileRelativePathPattern(ctxt, NULL, novar);
    } else if (CUR == '/') {
	/*
	 * We need to find root as the parent
	 */
	NEXT;
	SKIP_BLANKS;
	PUSH(XSLT_OP_ROOT, NULL, NULL, novar);
	if ((CUR != 0) && (CUR != '|')) {
	    PUSH(XSLT_OP_PARENT, NULL, NULL, novar);
	    xsltCompileRelativePathPattern(ctxt, NULL, novar);
	}
    } else if (CUR == '*') {
	xsltCompileRelativePathPattern(ctxt, NULL, novar);
    } else if (CUR == '@') {
	xsltCompileRelativePathPattern(ctxt, NULL, novar);
    } else {
	xmlChar *name;
	name = xsltScanNCName(ctxt);
	if (name == NULL) {
	    xsltTransformError(NULL, NULL, NULL,
		    "xsltCompileLocationPathPattern : Name expected\n");
	    ctxt->error = 1;
	    return;
	}
 	SKIP_BLANKS;
 	if ((CUR == '(') && !xmlXPathIsNodeType(name)) {
 	    xsltCompileIdKeyPattern(ctxt, name, 1, novar, 0);
	    if (ctxt->error)
		return;
 	    if ((CUR == '/') && (NXT(1) == '/')) {
 		PUSH(XSLT_OP_ANCESTOR, NULL, NULL, novar);
 		NEXT;
		NEXT;
		SKIP_BLANKS;
		xsltCompileRelativePathPattern(ctxt, NULL, novar);
	    } else if (CUR == '/') {
		PUSH(XSLT_OP_PARENT, NULL, NULL, novar);
		NEXT;
		SKIP_BLANKS;
		xsltCompileRelativePathPattern(ctxt, NULL, novar);
	    }
	    return;
	}
	xsltCompileRelativePathPattern(ctxt, name, novar);
    }
error:
    return;
}
