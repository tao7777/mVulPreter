exsltFuncFunctionFunction (xmlXPathParserContextPtr ctxt, int nargs) {
    xmlXPathObjectPtr oldResult, ret;
    exsltFuncData *data;
    exsltFuncFunctionData *func;
    xmlNodePtr paramNode, oldInsert, fake;
    int oldBase;
    xsltStackElemPtr params = NULL, param;
    xsltTransformContextPtr tctxt = xsltXPathGetTransformContext(ctxt);
    int i, notSet;
    struct objChain {
	struct objChain *next;
	xmlXPathObjectPtr obj;
    };
    struct objChain	*savedObjChain = NULL, *savedObj;

    /*
     * retrieve func:function template
     */
    data = (exsltFuncData *) xsltGetExtData (tctxt,
					     EXSLT_FUNCTIONS_NAMESPACE);
    oldResult = data->result;
    data->result = NULL;

     func = (exsltFuncFunctionData*) xmlHashLookup2 (data->funcs,
 						    ctxt->context->functionURI,
 						    ctxt->context->function);
 
     /*
      * params handling
     */
    if (nargs > func->nargs) {
	xsltGenericError(xsltGenericErrorContext,
			 "{%s}%s: called with too many arguments\n",
			 ctxt->context->functionURI, ctxt->context->function);
	ctxt->error = XPATH_INVALID_ARITY;
	return;
    }
    if (func->content != NULL) {
	paramNode = func->content->prev;
    }
    else
	paramNode = NULL;
    if ((paramNode == NULL) && (func->nargs != 0)) {
	xsltGenericError(xsltGenericErrorContext,
			 "exsltFuncFunctionFunction: nargs != 0 and "
			 "param == NULL\n");
	return;
    }
    if (tctxt->funcLevel > MAX_FUNC_RECURSION) {
	xsltGenericError(xsltGenericErrorContext,
			 "{%s}%s: detected a recursion\n",
			 ctxt->context->functionURI, ctxt->context->function);
	ctxt->error = XPATH_MEMORY_ERROR;
	return;
    }
    tctxt->funcLevel++;

    /*
     * We have a problem with the evaluation of function parameters.
     * The original library code did not evaluate XPath expressions until
     * the last moment.  After version 1.1.17 of the libxslt, the logic
     * of other parts of the library was changed, and the evaluation of
     * XPath expressions within parameters now takes place as soon as the
     * parameter is parsed/evaluated (xsltParseStylesheetCallerParam).
     * This means that the parameters need to be evaluated in lexical
     * order (since a variable is "in scope" as soon as it is declared).
     * However, on entry to this routine, the values (from the caller) are
     * in reverse order (held on the XPath context variable stack).  To
     * accomplish what is required, I have added code to pop the XPath
     * objects off of the stack at the beginning and save them, then use
     * them (in the reverse order) as the params are evaluated.  This
     * requires an xmlMalloc/xmlFree for each param set by the caller,
     * which is not very nice.  There is probably a much better solution
     * (like change other code to delay the evaluation).
     */
    /*
     * In order to give the function params and variables a new 'scope'
     * we change varsBase in the context.
     */
    oldBase = tctxt->varsBase;
    tctxt->varsBase = tctxt->varsNr;
    /* If there are any parameters */
    if (paramNode != NULL) {
        /* Fetch the stored argument values from the caller */
	for (i = 0; i < nargs; i++) {
	    savedObj = xmlMalloc(sizeof(struct objChain));
	    savedObj->next = savedObjChain;
	    savedObj->obj = valuePop(ctxt);
	    savedObjChain = savedObj;
	}

	/*
	 * Prepare to process params in reverse order.  First, go to
	 * the beginning of the param chain.
	 */
	for (i = 1; i <= func->nargs; i++) {
	    if (paramNode->prev == NULL)
	        break;
	    paramNode = paramNode->prev;
	}
	/*
	 * i has total # params found, nargs is number which are present
	 * as arguments from the caller
	 * Calculate the number of un-set parameters
	 */
	notSet = func->nargs - nargs;
	for (; i > 0; i--) {
	    param = xsltParseStylesheetCallerParam (tctxt, paramNode);
	    if (i > notSet) {	/* if parameter value set */
		param->computed = 1;
		if (param->value != NULL)
		    xmlXPathFreeObject(param->value);
		savedObj = savedObjChain;	/* get next val from chain */
		param->value = savedObj->obj;
		savedObjChain = savedObjChain->next;
		xmlFree(savedObj);
	    }
	    xsltLocalVariablePush(tctxt, param, -1);
	    param->next = params;
	    params = param;
	    paramNode = paramNode->next;
	}
    }
    /*
     * actual processing
     */
    fake = xmlNewDocNode(tctxt->output, NULL,
			 (const xmlChar *)"fake", NULL);
    oldInsert = tctxt->insert;
    tctxt->insert = fake;
    xsltApplyOneTemplate (tctxt, xmlXPathGetContextNode(ctxt),
			  func->content, NULL, NULL);
    xsltLocalVariablePop(tctxt, tctxt->varsBase, -2);
    tctxt->insert = oldInsert;
    tctxt->varsBase = oldBase;	/* restore original scope */
    if (params != NULL)
	xsltFreeStackElemList(params);

    if (data->error != 0)
	goto error;

    if (data->result != NULL) {
	ret = data->result;
    } else
	ret = xmlXPathNewCString("");

    data->result = oldResult;

    /*
     * It is an error if the instantiation of the template results in
     * the generation of result nodes.
     */
    if (fake->children != NULL) {
#ifdef LIBXML_DEBUG_ENABLED
	xmlDebugDumpNode (stderr, fake, 1);
#endif
	xsltGenericError(xsltGenericErrorContext,
			 "{%s}%s: cannot write to result tree while "
			 "executing a function\n",
			 ctxt->context->functionURI, ctxt->context->function);
	xmlFreeNode(fake);
	goto error;
    }
    xmlFreeNode(fake);
    valuePush(ctxt, ret);

error:
    /*
    * IMPORTANT: This enables previously tree fragments marked as
    * being results of a function, to be garbage-collected after
    * the calling process exits.
    */
    xsltExtensionInstructionResultFinalize(tctxt);
    tctxt->funcLevel--;
}
