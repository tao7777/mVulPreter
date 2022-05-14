xmlXPathCompOpEval(xmlXPathParserContextPtr ctxt, xmlXPathStepOpPtr op)
{
    int total = 0;
    int equal, ret;
    xmlXPathCompExprPtr comp;
    xmlXPathObjectPtr arg1, arg2;
    xmlNodePtr bak;
    xmlDocPtr bakd;
    int pp;
    int cs;

    CHECK_ERROR0;
    comp = ctxt->comp;
    switch (op->op) {
        case XPATH_OP_END:
            return (0);
        case XPATH_OP_AND:
	    bakd = ctxt->context->doc;
	    bak = ctxt->context->node;
	    pp = ctxt->context->proximityPosition;
	    cs = ctxt->context->contextSize;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
            xmlXPathBooleanFunction(ctxt, 1);
            if ((ctxt->value == NULL) || (ctxt->value->boolval == 0))
                return (total);
            arg2 = valuePop(ctxt);
	    ctxt->context->doc = bakd;
	    ctxt->context->node = bak;
	    ctxt->context->proximityPosition = pp;
	    ctxt->context->contextSize = cs;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    if (ctxt->error) {
		xmlXPathFreeObject(arg2);
		return(0);
	    }
            xmlXPathBooleanFunction(ctxt, 1);
            arg1 = valuePop(ctxt);
            arg1->boolval &= arg2->boolval;
            valuePush(ctxt, arg1);
	    xmlXPathReleaseObject(ctxt->context, arg2);
            return (total);
        case XPATH_OP_OR:
	    bakd = ctxt->context->doc;
	    bak = ctxt->context->node;
	    pp = ctxt->context->proximityPosition;
	    cs = ctxt->context->contextSize;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
            xmlXPathBooleanFunction(ctxt, 1);
            if ((ctxt->value == NULL) || (ctxt->value->boolval == 1))
                return (total);
            arg2 = valuePop(ctxt);
	    ctxt->context->doc = bakd;
	    ctxt->context->node = bak;
	    ctxt->context->proximityPosition = pp;
	    ctxt->context->contextSize = cs;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    if (ctxt->error) {
		xmlXPathFreeObject(arg2);
		return(0);
	    }
            xmlXPathBooleanFunction(ctxt, 1);
            arg1 = valuePop(ctxt);
            arg1->boolval |= arg2->boolval;
            valuePush(ctxt, arg1);
	    xmlXPathReleaseObject(ctxt->context, arg2);
            return (total);
        case XPATH_OP_EQUAL:
	    bakd = ctxt->context->doc;
	    bak = ctxt->context->node;
	    pp = ctxt->context->proximityPosition;
	    cs = ctxt->context->contextSize;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
	    ctxt->context->doc = bakd;
	    ctxt->context->node = bak;
	    ctxt->context->proximityPosition = pp;
	    ctxt->context->contextSize = cs;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    CHECK_ERROR0;
	    if (op->value)
		equal = xmlXPathEqualValues(ctxt);
	    else
		equal = xmlXPathNotEqualValues(ctxt);
	    valuePush(ctxt, xmlXPathCacheNewBoolean(ctxt->context, equal));
            return (total);
        case XPATH_OP_CMP:
	    bakd = ctxt->context->doc;
	    bak = ctxt->context->node;
	    pp = ctxt->context->proximityPosition;
	    cs = ctxt->context->contextSize;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
	    ctxt->context->doc = bakd;
	    ctxt->context->node = bak;
	    ctxt->context->proximityPosition = pp;
	    ctxt->context->contextSize = cs;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    CHECK_ERROR0;
            ret = xmlXPathCompareValues(ctxt, op->value, op->value2);
	    valuePush(ctxt, xmlXPathCacheNewBoolean(ctxt->context, ret));
            return (total);
        case XPATH_OP_PLUS:
	    bakd = ctxt->context->doc;
	    bak = ctxt->context->node;
	    pp = ctxt->context->proximityPosition;
	    cs = ctxt->context->contextSize;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
            if (op->ch2 != -1) {
		ctxt->context->doc = bakd;
		ctxt->context->node = bak;
		ctxt->context->proximityPosition = pp;
		ctxt->context->contextSize = cs;
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    }
	    CHECK_ERROR0;
            if (op->value == 0)
                xmlXPathSubValues(ctxt);
            else if (op->value == 1)
                xmlXPathAddValues(ctxt);
            else if (op->value == 2)
                xmlXPathValueFlipSign(ctxt);
            else if (op->value == 3) {
                CAST_TO_NUMBER;
                CHECK_TYPE0(XPATH_NUMBER);
            }
            return (total);
        case XPATH_OP_MULT:
	    bakd = ctxt->context->doc;
	    bak = ctxt->context->node;
	    pp = ctxt->context->proximityPosition;
	    cs = ctxt->context->contextSize;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
	    ctxt->context->doc = bakd;
	    ctxt->context->node = bak;
	    ctxt->context->proximityPosition = pp;
	    ctxt->context->contextSize = cs;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    CHECK_ERROR0;
            if (op->value == 0)
                xmlXPathMultValues(ctxt);
            else if (op->value == 1)
                xmlXPathDivValues(ctxt);
            else if (op->value == 2)
                xmlXPathModValues(ctxt);
            return (total);
        case XPATH_OP_UNION:
	    bakd = ctxt->context->doc;
	    bak = ctxt->context->node;
	    pp = ctxt->context->proximityPosition;
	    cs = ctxt->context->contextSize;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
	    ctxt->context->doc = bakd;
	    ctxt->context->node = bak;
	    ctxt->context->proximityPosition = pp;
	    ctxt->context->contextSize = cs;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    CHECK_ERROR0;
            CHECK_TYPE0(XPATH_NODESET);
            arg2 = valuePop(ctxt);

            CHECK_TYPE0(XPATH_NODESET);
            arg1 = valuePop(ctxt);

	    if ((arg1->nodesetval == NULL) ||
		((arg2->nodesetval != NULL) &&
		 (arg2->nodesetval->nodeNr != 0)))
	    {
		arg1->nodesetval = xmlXPathNodeSetMerge(arg1->nodesetval,
							arg2->nodesetval);
	    }

            valuePush(ctxt, arg1);
	    xmlXPathReleaseObject(ctxt->context, arg2);
            return (total);
        case XPATH_OP_ROOT:
            xmlXPathRoot(ctxt);
            return (total);
        case XPATH_OP_NODE:
            if (op->ch1 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
            if (op->ch2 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    CHECK_ERROR0;
	    valuePush(ctxt, xmlXPathCacheNewNodeSet(ctxt->context,
		ctxt->context->node));
            return (total);
        case XPATH_OP_RESET:
            if (op->ch1 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
            if (op->ch2 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    CHECK_ERROR0;
            ctxt->context->node = NULL;
            return (total);
        case XPATH_OP_COLLECT:{
                if (op->ch1 == -1)
                    return (total);

                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
		CHECK_ERROR0;

                total += xmlXPathNodeCollectAndTest(ctxt, op, NULL, NULL, 0);
                return (total);
            }
        case XPATH_OP_VALUE:
            valuePush(ctxt,
                      xmlXPathCacheObjectCopy(ctxt->context,
			(xmlXPathObjectPtr) op->value4));
            return (total);
        case XPATH_OP_VARIABLE:{
		xmlXPathObjectPtr val;

                if (op->ch1 != -1)
                    total +=
                        xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
                if (op->value5 == NULL) {
		    val = xmlXPathVariableLookup(ctxt->context, op->value4);
		    if (val == NULL) {
			ctxt->error = XPATH_UNDEF_VARIABLE_ERROR;
			return(0);
		    }
                    valuePush(ctxt, val);
		} else {
                    const xmlChar *URI;

                    URI = xmlXPathNsLookup(ctxt->context, op->value5);
                    if (URI == NULL) {
                         xmlGenericError(xmlGenericErrorContext,
             "xmlXPathCompOpEval: variable %s bound to undefined prefix %s\n",
                                     (char *) op->value4, (char *)op->value5);
                        ctxt->error = XPATH_UNDEF_PREFIX_ERROR;
                         return (total);
                     }
 		    val = xmlXPathVariableLookupNS(ctxt->context,
                                                       op->value4, URI);
		    if (val == NULL) {
			ctxt->error = XPATH_UNDEF_VARIABLE_ERROR;
			return(0);
		    }
                    valuePush(ctxt, val);
                }
                return (total);
            }
        case XPATH_OP_FUNCTION:{
                xmlXPathFunction func;
                const xmlChar *oldFunc, *oldFuncURI;
		int i;

                if (op->ch1 != -1)
                    total +=
                        xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
		if (ctxt->valueNr < op->value) {
		    xmlGenericError(xmlGenericErrorContext,
			    "xmlXPathCompOpEval: parameter error\n");
		    ctxt->error = XPATH_INVALID_OPERAND;
		    return (total);
		}
		for (i = 0; i < op->value; i++)
		    if (ctxt->valueTab[(ctxt->valueNr - 1) - i] == NULL) {
			xmlGenericError(xmlGenericErrorContext,
				"xmlXPathCompOpEval: parameter error\n");
			ctxt->error = XPATH_INVALID_OPERAND;
			return (total);
		    }
                if (op->cache != NULL)
                    XML_CAST_FPTR(func) = op->cache;
                else {
                    const xmlChar *URI = NULL;

                    if (op->value5 == NULL)
                        func =
                            xmlXPathFunctionLookup(ctxt->context,
                                                   op->value4);
                    else {
                        URI = xmlXPathNsLookup(ctxt->context, op->value5);
                        if (URI == NULL) {
                             xmlGenericError(xmlGenericErrorContext,
             "xmlXPathCompOpEval: function %s bound to undefined prefix %s\n",
                                     (char *)op->value4, (char *)op->value5);
                            ctxt->error = XPATH_UNDEF_PREFIX_ERROR;
                             return (total);
                         }
                         func = xmlXPathFunctionLookupNS(ctxt->context,
                                                        op->value4, URI);
                    }
                    if (func == NULL) {
                        xmlGenericError(xmlGenericErrorContext,
                                "xmlXPathCompOpEval: function %s not found\n",
                                        (char *)op->value4);
                        XP_ERROR0(XPATH_UNKNOWN_FUNC_ERROR);
                    }
                    op->cache = XML_CAST_FPTR(func);
                    op->cacheURI = (void *) URI;
                }
                oldFunc = ctxt->context->function;
                oldFuncURI = ctxt->context->functionURI;
                ctxt->context->function = op->value4;
                ctxt->context->functionURI = op->cacheURI;
                func(ctxt, op->value);
                ctxt->context->function = oldFunc;
                ctxt->context->functionURI = oldFuncURI;
                return (total);
            }
        case XPATH_OP_ARG:
	    bakd = ctxt->context->doc;
	    bak = ctxt->context->node;
	    pp = ctxt->context->proximityPosition;
	    cs = ctxt->context->contextSize;
            if (op->ch1 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    ctxt->context->contextSize = cs;
	    ctxt->context->proximityPosition = pp;
	    ctxt->context->node = bak;
	    ctxt->context->doc = bakd;
	    CHECK_ERROR0;
            if (op->ch2 != -1) {
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	        ctxt->context->doc = bakd;
	        ctxt->context->node = bak;
	        CHECK_ERROR0;
	    }
            return (total);
        case XPATH_OP_PREDICATE:
        case XPATH_OP_FILTER:{
                xmlXPathObjectPtr res;
                xmlXPathObjectPtr obj, tmp;
                xmlNodeSetPtr newset = NULL;
                xmlNodeSetPtr oldset;
                xmlNodePtr oldnode;
		xmlDocPtr oldDoc;
                int i;

                /*
                 * Optimization for ()[1] selection i.e. the first elem
                 */
                if ((op->ch1 != -1) && (op->ch2 != -1) &&
#ifdef XP_OPTIMIZED_FILTER_FIRST
		    /*
		    * FILTER TODO: Can we assume that the inner processing
		    *  will result in an ordered list if we have an
		    *  XPATH_OP_FILTER?
		    *  What about an additional field or flag on
		    *  xmlXPathObject like @sorted ? This way we wouln'd need
		    *  to assume anything, so it would be more robust and
		    *  easier to optimize.
		    */
                    ((comp->steps[op->ch1].op == XPATH_OP_SORT) || /* 18 */
		     (comp->steps[op->ch1].op == XPATH_OP_FILTER)) && /* 17 */
#else
		    (comp->steps[op->ch1].op == XPATH_OP_SORT) &&
#endif
                    (comp->steps[op->ch2].op == XPATH_OP_VALUE)) { /* 12 */
                    xmlXPathObjectPtr val;

                    val = comp->steps[op->ch2].value4;
                    if ((val != NULL) && (val->type == XPATH_NUMBER) &&
                        (val->floatval == 1.0)) {
                        xmlNodePtr first = NULL;

                        total +=
                            xmlXPathCompOpEvalFirst(ctxt,
                                                    &comp->steps[op->ch1],
                                                    &first);
			CHECK_ERROR0;
                        /*
                         * The nodeset should be in document order,
                         * Keep only the first value
                         */
                        if ((ctxt->value != NULL) &&
                            (ctxt->value->type == XPATH_NODESET) &&
                            (ctxt->value->nodesetval != NULL) &&
                            (ctxt->value->nodesetval->nodeNr > 1))
                            ctxt->value->nodesetval->nodeNr = 1;
                        return (total);
                    }
                }
                /*
                 * Optimization for ()[last()] selection i.e. the last elem
                 */
                if ((op->ch1 != -1) && (op->ch2 != -1) &&
                    (comp->steps[op->ch1].op == XPATH_OP_SORT) &&
                    (comp->steps[op->ch2].op == XPATH_OP_SORT)) {
                    int f = comp->steps[op->ch2].ch1;

                    if ((f != -1) &&
                        (comp->steps[f].op == XPATH_OP_FUNCTION) &&
                        (comp->steps[f].value5 == NULL) &&
                        (comp->steps[f].value == 0) &&
                        (comp->steps[f].value4 != NULL) &&
                        (xmlStrEqual
                         (comp->steps[f].value4, BAD_CAST "last"))) {
                        xmlNodePtr last = NULL;

                        total +=
                            xmlXPathCompOpEvalLast(ctxt,
                                                   &comp->steps[op->ch1],
                                                   &last);
			CHECK_ERROR0;
                        /*
                         * The nodeset should be in document order,
                         * Keep only the last value
                         */
                        if ((ctxt->value != NULL) &&
                            (ctxt->value->type == XPATH_NODESET) &&
                            (ctxt->value->nodesetval != NULL) &&
                            (ctxt->value->nodesetval->nodeTab != NULL) &&
                            (ctxt->value->nodesetval->nodeNr > 1)) {
                            ctxt->value->nodesetval->nodeTab[0] =
                                ctxt->value->nodesetval->nodeTab[ctxt->
                                                                 value->
                                                                 nodesetval->
                                                                 nodeNr -
                                                                 1];
                            ctxt->value->nodesetval->nodeNr = 1;
                        }
                        return (total);
                    }
                }
		/*
		* Process inner predicates first.
		* Example "index[parent::book][1]":
		* ...
		*   PREDICATE   <-- we are here "[1]"
		*     PREDICATE <-- process "[parent::book]" first
		*       SORT
		*         COLLECT  'parent' 'name' 'node' book
		*           NODE
		*     ELEM Object is a number : 1
		*/
                if (op->ch1 != -1)
                    total +=
                        xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
		CHECK_ERROR0;
                if (op->ch2 == -1)
                    return (total);
                if (ctxt->value == NULL)
                    return (total);

                oldnode = ctxt->context->node;

#ifdef LIBXML_XPTR_ENABLED
                /*
                 * Hum are we filtering the result of an XPointer expression
                 */
                if (ctxt->value->type == XPATH_LOCATIONSET) {
                    xmlLocationSetPtr newlocset = NULL;
                    xmlLocationSetPtr oldlocset;

                    /*
                     * Extract the old locset, and then evaluate the result of the
                     * expression for all the element in the locset. use it to grow
                     * up a new locset.
                     */
                    CHECK_TYPE0(XPATH_LOCATIONSET);
                    obj = valuePop(ctxt);
                    oldlocset = obj->user;
                    ctxt->context->node = NULL;

                    if ((oldlocset == NULL) || (oldlocset->locNr == 0)) {
                        ctxt->context->contextSize = 0;
                        ctxt->context->proximityPosition = 0;
                        if (op->ch2 != -1)
                            total +=
                                xmlXPathCompOpEval(ctxt,
                                                   &comp->steps[op->ch2]);
                        res = valuePop(ctxt);
                        if (res != NULL) {
			    xmlXPathReleaseObject(ctxt->context, res);
			}
                        valuePush(ctxt, obj);
                        CHECK_ERROR0;
                        return (total);
                    }
                    newlocset = xmlXPtrLocationSetCreate(NULL);

                    for (i = 0; i < oldlocset->locNr; i++) {
                        /*
                         * Run the evaluation with a node list made of a
                         * single item in the nodelocset.
                         */
                        ctxt->context->node = oldlocset->locTab[i]->user;
                        ctxt->context->contextSize = oldlocset->locNr;
                        ctxt->context->proximityPosition = i + 1;
			tmp = xmlXPathCacheNewNodeSet(ctxt->context,
			    ctxt->context->node);
                        valuePush(ctxt, tmp);

                        if (op->ch2 != -1)
                            total +=
                                xmlXPathCompOpEval(ctxt,
                                                   &comp->steps[op->ch2]);
			if (ctxt->error != XPATH_EXPRESSION_OK) {
			    xmlXPathFreeObject(obj);
			    return(0);
			}

                        /*
                         * The result of the evaluation need to be tested to
                         * decided whether the filter succeeded or not
                         */
                        res = valuePop(ctxt);
                        if (xmlXPathEvaluatePredicateResult(ctxt, res)) {
                            xmlXPtrLocationSetAdd(newlocset,
                                                  xmlXPathObjectCopy
                                                  (oldlocset->locTab[i]));
                        }

                        /*
                         * Cleanup
                         */
                        if (res != NULL) {
			    xmlXPathReleaseObject(ctxt->context, res);
			}
                        if (ctxt->value == tmp) {
                            res = valuePop(ctxt);
			    xmlXPathReleaseObject(ctxt->context, res);
                        }

                        ctxt->context->node = NULL;
                    }

                    /*
                     * The result is used as the new evaluation locset.
                     */
		    xmlXPathReleaseObject(ctxt->context, obj);
                    ctxt->context->node = NULL;
                    ctxt->context->contextSize = -1;
                    ctxt->context->proximityPosition = -1;
                    valuePush(ctxt, xmlXPtrWrapLocationSet(newlocset));
                    ctxt->context->node = oldnode;
                    return (total);
                }
#endif /* LIBXML_XPTR_ENABLED */

                /*
                 * Extract the old set, and then evaluate the result of the
                 * expression for all the element in the set. use it to grow
                 * up a new set.
                 */
                CHECK_TYPE0(XPATH_NODESET);
                obj = valuePop(ctxt);
                oldset = obj->nodesetval;

                oldnode = ctxt->context->node;
		oldDoc = ctxt->context->doc;
                ctxt->context->node = NULL;

                if ((oldset == NULL) || (oldset->nodeNr == 0)) {
                    ctxt->context->contextSize = 0;
                    ctxt->context->proximityPosition = 0;
/*
                    if (op->ch2 != -1)
                        total +=
                            xmlXPathCompOpEval(ctxt,
                                               &comp->steps[op->ch2]);
		    CHECK_ERROR0;
                    res = valuePop(ctxt);
                    if (res != NULL)
                        xmlXPathFreeObject(res);
*/
                    valuePush(ctxt, obj);
                    ctxt->context->node = oldnode;
                    CHECK_ERROR0;
                } else {
		    tmp = NULL;
                    /*
                     * Initialize the new set.
		     * Also set the xpath document in case things like
		     * key() evaluation are attempted on the predicate
                     */
                    newset = xmlXPathNodeSetCreate(NULL);
		    /*
		    * SPEC XPath 1.0:
		    *  "For each node in the node-set to be filtered, the
		    *  PredicateExpr is evaluated with that node as the
		    *  context node, with the number of nodes in the
		    *  node-set as the context size, and with the proximity
		    *  position of the node in the node-set with respect to
		    *  the axis as the context position;"
		    * @oldset is the node-set" to be filtered.
		    *
		    * SPEC XPath 1.0:
		    *  "only predicates change the context position and
		    *  context size (see [2.4 Predicates])."
		    * Example:
		    *   node-set  context pos
		    *    nA         1
		    *    nB         2
		    *    nC         3
		    *   After applying predicate [position() > 1] :
		    *   node-set  context pos
		    *    nB         1
		    *    nC         2
		    *
		    * removed the first node in the node-set, then
		    * the context position of the
		    */
                    for (i = 0; i < oldset->nodeNr; i++) {
                        /*
                         * Run the evaluation with a node list made of
                         * a single item in the nodeset.
                         */
                        ctxt->context->node = oldset->nodeTab[i];
			if ((oldset->nodeTab[i]->type != XML_NAMESPACE_DECL) &&
			    (oldset->nodeTab[i]->doc != NULL))
		            ctxt->context->doc = oldset->nodeTab[i]->doc;
			if (tmp == NULL) {
			    tmp = xmlXPathCacheNewNodeSet(ctxt->context,
				ctxt->context->node);
			} else {
			    xmlXPathNodeSetAddUnique(tmp->nodesetval,
				ctxt->context->node);
			}
                        valuePush(ctxt, tmp);
                        ctxt->context->contextSize = oldset->nodeNr;
                        ctxt->context->proximityPosition = i + 1;
			/*
			* Evaluate the predicate against the context node.
			* Can/should we optimize position() predicates
			* here (e.g. "[1]")?
			*/
                        if (op->ch2 != -1)
                            total +=
                                xmlXPathCompOpEval(ctxt,
                                                   &comp->steps[op->ch2]);
			if (ctxt->error != XPATH_EXPRESSION_OK) {
			    xmlXPathFreeNodeSet(newset);
			    xmlXPathFreeObject(obj);
			    return(0);
			}

                        /*
                         * The result of the evaluation needs to be tested to
                         * decide whether the filter succeeded or not
                         */
			/*
			* OPTIMIZE TODO: Can we use
			* xmlXPathNodeSetAdd*Unique()* instead?
			*/
                        res = valuePop(ctxt);
                        if (xmlXPathEvaluatePredicateResult(ctxt, res)) {
                            xmlXPathNodeSetAdd(newset, oldset->nodeTab[i]);
                        }

                        /*
                         * Cleanup
                         */
                        if (res != NULL) {
			    xmlXPathReleaseObject(ctxt->context, res);
			}
                        if (ctxt->value == tmp) {
                            valuePop(ctxt);
			    xmlXPathNodeSetClear(tmp->nodesetval, 1);
			    /*
			    * Don't free the temporary nodeset
			    * in order to avoid massive recreation inside this
			    * loop.
			    */
                        } else
			    tmp = NULL;
                        ctxt->context->node = NULL;
                    }
		    if (tmp != NULL)
			xmlXPathReleaseObject(ctxt->context, tmp);
                    /*
                     * The result is used as the new evaluation set.
                     */
		    xmlXPathReleaseObject(ctxt->context, obj);
                    ctxt->context->node = NULL;
                    ctxt->context->contextSize = -1;
                    ctxt->context->proximityPosition = -1;
		    /* may want to move this past the '}' later */
		    ctxt->context->doc = oldDoc;
		    valuePush(ctxt,
			xmlXPathCacheWrapNodeSet(ctxt->context, newset));
                }
                ctxt->context->node = oldnode;
                return (total);
            }
        case XPATH_OP_SORT:
            if (op->ch1 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
            if ((ctxt->value != NULL) &&
                (ctxt->value->type == XPATH_NODESET) &&
                (ctxt->value->nodesetval != NULL) &&
		(ctxt->value->nodesetval->nodeNr > 1))
	    {
                xmlXPathNodeSetSort(ctxt->value->nodesetval);
	    }
            return (total);
#ifdef LIBXML_XPTR_ENABLED
        case XPATH_OP_RANGETO:{
                xmlXPathObjectPtr range;
                xmlXPathObjectPtr res, obj;
                xmlXPathObjectPtr tmp;
                xmlLocationSetPtr newlocset = NULL;
		    xmlLocationSetPtr oldlocset;
                xmlNodeSetPtr oldset;
                int i, j;

                if (op->ch1 != -1)
                    total +=
                        xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
                if (op->ch2 == -1)
                    return (total);

                if (ctxt->value->type == XPATH_LOCATIONSET) {
                    /*
                     * Extract the old locset, and then evaluate the result of the
                     * expression for all the element in the locset. use it to grow
                     * up a new locset.
                     */
                    CHECK_TYPE0(XPATH_LOCATIONSET);
                    obj = valuePop(ctxt);
                    oldlocset = obj->user;

                    if ((oldlocset == NULL) || (oldlocset->locNr == 0)) {
		        ctxt->context->node = NULL;
                        ctxt->context->contextSize = 0;
                        ctxt->context->proximityPosition = 0;
                        total += xmlXPathCompOpEval(ctxt,&comp->steps[op->ch2]);
                        res = valuePop(ctxt);
                        if (res != NULL) {
			    xmlXPathReleaseObject(ctxt->context, res);
			}
                        valuePush(ctxt, obj);
                        CHECK_ERROR0;
                        return (total);
                    }
                    newlocset = xmlXPtrLocationSetCreate(NULL);

                    for (i = 0; i < oldlocset->locNr; i++) {
                        /*
                         * Run the evaluation with a node list made of a
                         * single item in the nodelocset.
                         */
                        ctxt->context->node = oldlocset->locTab[i]->user;
                        ctxt->context->contextSize = oldlocset->locNr;
                        ctxt->context->proximityPosition = i + 1;
			tmp = xmlXPathCacheNewNodeSet(ctxt->context,
			    ctxt->context->node);
                        valuePush(ctxt, tmp);

                        if (op->ch2 != -1)
                            total +=
                                xmlXPathCompOpEval(ctxt,
                                                   &comp->steps[op->ch2]);
			if (ctxt->error != XPATH_EXPRESSION_OK) {
			    xmlXPathFreeObject(obj);
			    return(0);
			}

                        res = valuePop(ctxt);
			if (res->type == XPATH_LOCATIONSET) {
			    xmlLocationSetPtr rloc =
			        (xmlLocationSetPtr)res->user;
			    for (j=0; j<rloc->locNr; j++) {
			        range = xmlXPtrNewRange(
				  oldlocset->locTab[i]->user,
				  oldlocset->locTab[i]->index,
				  rloc->locTab[j]->user2,
				  rloc->locTab[j]->index2);
				if (range != NULL) {
				    xmlXPtrLocationSetAdd(newlocset, range);
				}
			    }
			} else {
			    range = xmlXPtrNewRangeNodeObject(
				(xmlNodePtr)oldlocset->locTab[i]->user, res);
                            if (range != NULL) {
                                xmlXPtrLocationSetAdd(newlocset,range);
			    }
                        }

                        /*
                         * Cleanup
                         */
                        if (res != NULL) {
			    xmlXPathReleaseObject(ctxt->context, res);
			}
                        if (ctxt->value == tmp) {
                            res = valuePop(ctxt);
			    xmlXPathReleaseObject(ctxt->context, res);
                        }

                        ctxt->context->node = NULL;
                    }
		} else {	/* Not a location set */
                    CHECK_TYPE0(XPATH_NODESET);
                    obj = valuePop(ctxt);
                    oldset = obj->nodesetval;
                    ctxt->context->node = NULL;

                    newlocset = xmlXPtrLocationSetCreate(NULL);

                    if (oldset != NULL) {
                        for (i = 0; i < oldset->nodeNr; i++) {
                            /*
                             * Run the evaluation with a node list made of a single item
                             * in the nodeset.
                             */
                            ctxt->context->node = oldset->nodeTab[i];
			    /*
			    * OPTIMIZE TODO: Avoid recreation for every iteration.
			    */
			    tmp = xmlXPathCacheNewNodeSet(ctxt->context,
				ctxt->context->node);
                            valuePush(ctxt, tmp);

                            if (op->ch2 != -1)
                                total +=
                                    xmlXPathCompOpEval(ctxt,
                                                   &comp->steps[op->ch2]);
			    if (ctxt->error != XPATH_EXPRESSION_OK) {
				xmlXPathFreeObject(obj);
				return(0);
			    }

                            res = valuePop(ctxt);
                            range =
                                xmlXPtrNewRangeNodeObject(oldset->nodeTab[i],
                                                      res);
                            if (range != NULL) {
                                xmlXPtrLocationSetAdd(newlocset, range);
                            }

                            /*
                             * Cleanup
                             */
                            if (res != NULL) {
				xmlXPathReleaseObject(ctxt->context, res);
			    }
                            if (ctxt->value == tmp) {
                                res = valuePop(ctxt);
				xmlXPathReleaseObject(ctxt->context, res);
                            }

                            ctxt->context->node = NULL;
                        }
                    }
                }

                /*
                 * The result is used as the new evaluation set.
                 */
		xmlXPathReleaseObject(ctxt->context, obj);
                ctxt->context->node = NULL;
                ctxt->context->contextSize = -1;
                ctxt->context->proximityPosition = -1;
                valuePush(ctxt, xmlXPtrWrapLocationSet(newlocset));
                return (total);
            }
#endif /* LIBXML_XPTR_ENABLED */
    }
