static xmlDocPtr dom_document_parser(zval *id, int mode, char *source, int source_len, int options TSRMLS_DC) /* {{{ */
{
    xmlDocPtr ret;
    xmlParserCtxtPtr ctxt = NULL;
	dom_doc_propsptr doc_props;
	dom_object *intern;
	php_libxml_ref_obj *document = NULL;
	int validate, recover, resolve_externals, keep_blanks, substitute_ent;
	int resolved_path_len;
	int old_error_reporting = 0;
	char *directory=NULL, resolved_path[MAXPATHLEN];

	if (id != NULL) {
		intern = (dom_object *)zend_object_store_get_object(id TSRMLS_CC);
		document = intern->document;
	}

	doc_props = dom_get_doc_props(document);
	validate = doc_props->validateonparse;
	resolve_externals = doc_props->resolveexternals;
	keep_blanks = doc_props->preservewhitespace;
	substitute_ent = doc_props->substituteentities;
	recover = doc_props->recover;

	if (document == NULL) {
		efree(doc_props);
	}

        xmlInitParser();
 
        if (mode == DOM_LOAD_FILE) {
               if (CHECK_NULL_PATH(source, source_len)) {
                       return NULL;
               }
                char *file_dest = _dom_get_valid_file_path(source, resolved_path, MAXPATHLEN  TSRMLS_CC);
                if (file_dest) {
                        ctxt = xmlCreateFileParserCtxt(file_dest);
		ctxt = xmlCreateMemoryParserCtxt(source, source_len);
	}

	if (ctxt == NULL) {
		return(NULL);
	}

	/* If loading from memory, we need to set the base directory for the document */
	if (mode != DOM_LOAD_FILE) {
#if HAVE_GETCWD
		directory = VCWD_GETCWD(resolved_path, MAXPATHLEN);
#elif HAVE_GETWD
		directory = VCWD_GETWD(resolved_path);
#endif
		if (directory) {
			if(ctxt->directory != NULL) {
				xmlFree((char *) ctxt->directory);
			}
			resolved_path_len = strlen(resolved_path);
			if (resolved_path[resolved_path_len - 1] != DEFAULT_SLASH) {
				resolved_path[resolved_path_len] = DEFAULT_SLASH;
				resolved_path[++resolved_path_len] = '\0';
			}
			ctxt->directory = (char *) xmlCanonicPath((const xmlChar *) resolved_path);
		}
	}

	ctxt->vctxt.error = php_libxml_ctx_error;
	ctxt->vctxt.warning = php_libxml_ctx_warning;

	if (ctxt->sax != NULL) {
		ctxt->sax->error = php_libxml_ctx_error;
		ctxt->sax->warning = php_libxml_ctx_warning;
	}

	if (validate && ! (options & XML_PARSE_DTDVALID)) {
		options |= XML_PARSE_DTDVALID;
	}
	if (resolve_externals && ! (options & XML_PARSE_DTDATTR)) {
		options |= XML_PARSE_DTDATTR;
	}
	if (substitute_ent && ! (options & XML_PARSE_NOENT)) {
		options |= XML_PARSE_NOENT;
	}
	if (keep_blanks == 0 && ! (options & XML_PARSE_NOBLANKS)) {
		options |= XML_PARSE_NOBLANKS;
	}

	xmlCtxtUseOptions(ctxt, options);

	ctxt->recovery = recover;
	if (recover) {
		old_error_reporting = EG(error_reporting);
		EG(error_reporting) = old_error_reporting | E_WARNING;
	}

	xmlParseDocument(ctxt);

	if (ctxt->wellFormed || recover) {
		ret = ctxt->myDoc;
		if (ctxt->recovery) {
			EG(error_reporting) = old_error_reporting;
		}
		/* If loading from memory, set the base reference uri for the document */
		if (ret && ret->URL == NULL && ctxt->directory != NULL) {
			ret->URL = xmlStrdup(ctxt->directory);
		}
	} else {
		ret = NULL;
		xmlFreeDoc(ctxt->myDoc);
		ctxt->myDoc = NULL;
	}

	xmlFreeParserCtxt(ctxt);

	return(ret);
}
/* }}} */

/* {{{ static void dom_parse_document(INTERNAL_FUNCTION_PARAMETERS, int mode) */
static void dom_parse_document(INTERNAL_FUNCTION_PARAMETERS, int mode) {
	zval *id;
	xmlDoc *docp = NULL, *newdoc;
	dom_doc_propsptr doc_prop;
	dom_object *intern;
	char *source;
	int source_len, refcount, ret;
	long options = 0;

	id = getThis();
	if (id != NULL && ! instanceof_function(Z_OBJCE_P(id), dom_document_class_entry TSRMLS_CC)) {
		id = NULL;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &source, &source_len, &options) == FAILURE) {
		return;
	}

	if (!source_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty string supplied as input");
		RETURN_FALSE;
	}

	newdoc = dom_document_parser(id, mode, source, source_len, options TSRMLS_CC);

	if (!newdoc)
		RETURN_FALSE;

	if (id != NULL) {
		intern = (dom_object *)zend_object_store_get_object(id TSRMLS_CC);
		if (intern != NULL) {
			docp = (xmlDocPtr) dom_object_get_node(intern);
			doc_prop = NULL;
			if (docp != NULL) {
				php_libxml_decrement_node_ptr((php_libxml_node_object *) intern TSRMLS_CC);
				doc_prop = intern->document->doc_props;
				intern->document->doc_props = NULL;
				refcount = php_libxml_decrement_doc_ref((php_libxml_node_object *)intern TSRMLS_CC);
				if (refcount != 0) {
					docp->_private = NULL;
				}
			}
			intern->document = NULL;
			if (php_libxml_increment_doc_ref((php_libxml_node_object *)intern, newdoc TSRMLS_CC) == -1) {
				RETURN_FALSE;
			}
			intern->document->doc_props = doc_prop;
		}

		php_libxml_increment_node_ptr((php_libxml_node_object *)intern, (xmlNodePtr)newdoc, (void *)intern TSRMLS_CC);

		RETURN_TRUE;
	} else {
		DOM_RET_OBJ((xmlNodePtr) newdoc, &ret, NULL);
	}
}
/* }}} end dom_parser_document */

/* {{{ proto DOMNode dom_document_load(string source [, int options]);
URL: http://www.w3.org/TR/DOM-Level-3-LS/load-save.html#LS-DocumentLS-load
Since: DOM Level 3
*/
PHP_METHOD(domdocument, load)
{
	dom_parse_document(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_FILE);
}
/* }}} end dom_document_load */

/* {{{ proto DOMNode dom_document_loadxml(string source [, int options]);
URL: http://www.w3.org/TR/DOM-Level-3-LS/load-save.html#LS-DocumentLS-loadXML
Since: DOM Level 3
*/
PHP_METHOD(domdocument, loadXML)
{
	dom_parse_document(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_STRING);
}
/* }}} end dom_document_loadxml */

/* {{{ proto int dom_document_save(string file);
Convenience method to save to file
*/
PHP_FUNCTION(dom_document_save)
{
	zval *id;
	xmlDoc *docp;
	int file_len = 0, bytes, format, saveempty = 0;
	dom_object *intern;
	dom_doc_propsptr doc_props;
	char *file;
	long options = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os|l", &id, dom_document_class_entry, &file, &file_len, &options) == FAILURE) {
		return;
	}

	if (file_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Filename");
		RETURN_FALSE;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	/* encoding handled by property on doc */

	doc_props = dom_get_doc_props(intern->document);
	format = doc_props->formatoutput;
	if (options & LIBXML_SAVE_NOEMPTYTAG) {
		saveempty = xmlSaveNoEmptyTags;
		xmlSaveNoEmptyTags = 1;
	}
	bytes = xmlSaveFormatFileEnc(file, docp, NULL, format);
	if (options & LIBXML_SAVE_NOEMPTYTAG) {
		xmlSaveNoEmptyTags = saveempty;
	}
	if (bytes == -1) {
		RETURN_FALSE;
	}
	RETURN_LONG(bytes);
}
/* }}} end dom_document_save */

/* {{{ proto string dom_document_savexml([node n]);
URL: http://www.w3.org/TR/DOM-Level-3-LS/load-save.html#LS-DocumentLS-saveXML
Since: DOM Level 3
*/
PHP_FUNCTION(dom_document_savexml)
{
	zval *id, *nodep = NULL;
	xmlDoc *docp;
	xmlNode *node;
	xmlBufferPtr buf;
	xmlChar *mem;
	dom_object *intern, *nodeobj;
	dom_doc_propsptr doc_props;
	int size, format, saveempty = 0;
	long options = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|O!l", &id, dom_document_class_entry, &nodep, dom_node_class_entry, &options) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	doc_props = dom_get_doc_props(intern->document);
	format = doc_props->formatoutput;

	if (nodep != NULL) {
		/* Dump contents of Node */
		DOM_GET_OBJ(node, nodep, xmlNodePtr, nodeobj);
		if (node->doc != docp) {
			php_dom_throw_error(WRONG_DOCUMENT_ERR, dom_get_strict_error(intern->document) TSRMLS_CC);
			RETURN_FALSE;
		}
		buf = xmlBufferCreate();
		if (!buf) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not fetch buffer");
			RETURN_FALSE;
		}
		if (options & LIBXML_SAVE_NOEMPTYTAG) {
			saveempty = xmlSaveNoEmptyTags;
			xmlSaveNoEmptyTags = 1;
		}
		xmlNodeDump(buf, docp, node, 0, format);
		if (options & LIBXML_SAVE_NOEMPTYTAG) {
			xmlSaveNoEmptyTags = saveempty;
		}
		mem = (xmlChar*) xmlBufferContent(buf);
		if (!mem) {
			xmlBufferFree(buf);
			RETURN_FALSE;
		}
		RETVAL_STRING(mem, 1);
		xmlBufferFree(buf);
	} else {
		if (options & LIBXML_SAVE_NOEMPTYTAG) {
			saveempty = xmlSaveNoEmptyTags;
			xmlSaveNoEmptyTags = 1;
		}
		/* Encoding is handled from the encoding property set on the document */
		xmlDocDumpFormatMemory(docp, &mem, &size, format);
		if (options & LIBXML_SAVE_NOEMPTYTAG) {
			xmlSaveNoEmptyTags = saveempty;
		}
		if (!size) {
			RETURN_FALSE;
		}
		RETVAL_STRINGL(mem, size, 1);
		xmlFree(mem);
	}
}
/* }}} end dom_document_savexml */

static xmlNodePtr php_dom_free_xinclude_node(xmlNodePtr cur TSRMLS_DC) /* {{{ */
{
	xmlNodePtr xincnode;

	xincnode = cur;
	cur = cur->next;
	xmlUnlinkNode(xincnode);
	php_libxml_node_free_resource(xincnode TSRMLS_CC);

	return cur;
}
/* }}} */

static void php_dom_remove_xinclude_nodes(xmlNodePtr cur TSRMLS_DC) /* {{{ */
{
	while(cur) {
		if (cur->type == XML_XINCLUDE_START) {
			cur = php_dom_free_xinclude_node(cur TSRMLS_CC);

			/* XML_XINCLUDE_END node will be a sibling of XML_XINCLUDE_START */
			while(cur && cur->type != XML_XINCLUDE_END) {
				/* remove xinclude processing nodes from recursive xincludes */
				if (cur->type == XML_ELEMENT_NODE) {
					   php_dom_remove_xinclude_nodes(cur->children TSRMLS_CC);
				}
				cur = cur->next;
			}

			if (cur && cur->type == XML_XINCLUDE_END) {
				cur = php_dom_free_xinclude_node(cur TSRMLS_CC);
			}
		} else {
			if (cur->type == XML_ELEMENT_NODE) {
				php_dom_remove_xinclude_nodes(cur->children TSRMLS_CC);
			}
			cur = cur->next;
		}
	}
}
/* }}} */

/* {{{ proto int dom_document_xinclude([int options])
   Substitutues xincludes in a DomDocument */
PHP_FUNCTION(dom_document_xinclude)
{
	zval *id;
	xmlDoc *docp;
	xmlNodePtr root;
	long flags = 0; 
	int err;
	dom_object *intern;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|l", &id, dom_document_class_entry, &flags) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	err = xmlXIncludeProcessFlags(docp, flags);

	/* XML_XINCLUDE_START and XML_XINCLUDE_END nodes need to be removed as these
	are added via xmlXIncludeProcess to mark beginning and ending of xincluded document 
	but are not wanted in resulting document - must be done even if err as it could fail after
	having processed some xincludes */
	root = (xmlNodePtr) docp->children;
	while(root && root->type != XML_ELEMENT_NODE && root->type != XML_XINCLUDE_START) {
		root = root->next;
	}
	if (root) {
		php_dom_remove_xinclude_nodes(root TSRMLS_CC);
	}

	if (err) {
		RETVAL_LONG(err);
	} else {
		RETVAL_FALSE;
	}
    
}
/* }}} */

/* {{{ proto boolean dom_document_validate();
Since: DOM extended
*/
PHP_FUNCTION(dom_document_validate)
{
	zval *id;
	xmlDoc *docp;
	dom_object *intern;
	xmlValidCtxt *cvp;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &id, dom_document_class_entry) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	cvp = xmlNewValidCtxt();
	
	cvp->userData = NULL;
	cvp->error    = (xmlValidityErrorFunc) php_libxml_error_handler;
	cvp->warning  = (xmlValidityErrorFunc) php_libxml_error_handler;
	
	if (xmlValidateDocument(cvp, docp)) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
	
	xmlFreeValidCtxt(cvp);
	
}
/* }}} */

#if defined(LIBXML_SCHEMAS_ENABLED)
static void _dom_document_schema_validate(INTERNAL_FUNCTION_PARAMETERS, int type) /* {{{ */
{
	zval *id;
	xmlDoc *docp;
	dom_object *intern;
	char *source = NULL, *valid_file = NULL;
	int source_len = 0;
	xmlSchemaParserCtxtPtr  parser;
	xmlSchemaPtr            sptr;
	xmlSchemaValidCtxtPtr   vptr;
	int                     is_valid;
	char resolved_path[MAXPATHLEN + 1];

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Op", &id, dom_document_class_entry, &source, &source_len) == FAILURE) {
		return;
	}

	if (source_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Schema source");
		RETURN_FALSE;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	switch (type) {
	case DOM_LOAD_FILE:
		valid_file = _dom_get_valid_file_path(source, resolved_path, MAXPATHLEN  TSRMLS_CC);
		if (!valid_file) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Schema file source");
			RETURN_FALSE;
		}
		parser = xmlSchemaNewParserCtxt(valid_file);
		break;
	case DOM_LOAD_STRING:
		parser = xmlSchemaNewMemParserCtxt(source, source_len);
		/* If loading from memory, we need to set the base directory for the document 
		   but it is not apparent how to do that for schema's */
		break;
	default:
		return;
	}

	xmlSchemaSetParserErrors(parser,
		(xmlSchemaValidityErrorFunc) php_libxml_error_handler,
		(xmlSchemaValidityWarningFunc) php_libxml_error_handler,
		parser);
	sptr = xmlSchemaParse(parser);
	xmlSchemaFreeParserCtxt(parser);
	if (!sptr) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Schema");
		RETURN_FALSE;
	}

	docp = (xmlDocPtr) dom_object_get_node(intern);

	vptr = xmlSchemaNewValidCtxt(sptr);
	if (!vptr) {
		xmlSchemaFree(sptr);
		php_error(E_ERROR, "Invalid Schema Validation Context");
		RETURN_FALSE;
	}

	xmlSchemaSetValidErrors(vptr, php_libxml_error_handler, php_libxml_error_handler, vptr);
	is_valid = xmlSchemaValidateDoc(vptr, docp);
	xmlSchemaFree(sptr);
	xmlSchemaFreeValidCtxt(vptr);

	if (is_valid == 0) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto boolean dom_document_schema_validate_file(string filename); */
PHP_FUNCTION(dom_document_schema_validate_file)
{
	_dom_document_schema_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_FILE);
}
/* }}} end dom_document_schema_validate_file */

/* {{{ proto boolean dom_document_schema_validate(string source); */
PHP_FUNCTION(dom_document_schema_validate_xml)
{
	_dom_document_schema_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_STRING);
}
/* }}} end dom_document_schema_validate */

static void _dom_document_relaxNG_validate(INTERNAL_FUNCTION_PARAMETERS, int type) /* {{{ */
{
	zval *id;
	xmlDoc *docp;
	dom_object *intern;
	char *source = NULL, *valid_file = NULL;
	int source_len = 0;
	xmlRelaxNGParserCtxtPtr parser;
	xmlRelaxNGPtr           sptr;
	xmlRelaxNGValidCtxtPtr  vptr;
	int                     is_valid;
	char resolved_path[MAXPATHLEN + 1];

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Op", &id, dom_document_class_entry, &source, &source_len) == FAILURE) {
		return;
	}

	if (source_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Schema source");
		RETURN_FALSE;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	switch (type) {
	case DOM_LOAD_FILE:
		valid_file = _dom_get_valid_file_path(source, resolved_path, MAXPATHLEN  TSRMLS_CC);
		if (!valid_file) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid RelaxNG file source");
			RETURN_FALSE;
		}
		parser = xmlRelaxNGNewParserCtxt(valid_file);
		break;
	case DOM_LOAD_STRING:
		parser = xmlRelaxNGNewMemParserCtxt(source, source_len);
		/* If loading from memory, we need to set the base directory for the document 
		   but it is not apparent how to do that for schema's */
		break;
	default:
		return;
	}

	xmlRelaxNGSetParserErrors(parser,
		(xmlRelaxNGValidityErrorFunc) php_libxml_error_handler,
		(xmlRelaxNGValidityWarningFunc) php_libxml_error_handler,
		parser);
	sptr = xmlRelaxNGParse(parser);
	xmlRelaxNGFreeParserCtxt(parser);
	if (!sptr) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid RelaxNG");
		RETURN_FALSE;
	}

	docp = (xmlDocPtr) dom_object_get_node(intern);

	vptr = xmlRelaxNGNewValidCtxt(sptr);
	if (!vptr) {
		xmlRelaxNGFree(sptr);
		php_error(E_ERROR, "Invalid RelaxNG Validation Context");
		RETURN_FALSE;
	}

	xmlRelaxNGSetValidErrors(vptr, php_libxml_error_handler, php_libxml_error_handler, vptr);
	is_valid = xmlRelaxNGValidateDoc(vptr, docp);
	xmlRelaxNGFree(sptr);
	xmlRelaxNGFreeValidCtxt(vptr);

	if (is_valid == 0) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto boolean dom_document_relaxNG_validate_file(string filename); */
PHP_FUNCTION(dom_document_relaxNG_validate_file)
{
	_dom_document_relaxNG_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_FILE);
}
/* }}} end dom_document_relaxNG_validate_file */

/* {{{ proto boolean dom_document_relaxNG_validate_xml(string source); */
PHP_FUNCTION(dom_document_relaxNG_validate_xml)
{
	_dom_document_relaxNG_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_STRING);
}
/* }}} end dom_document_relaxNG_validate_xml */

#endif

#if defined(LIBXML_HTML_ENABLED)

static void dom_load_html(INTERNAL_FUNCTION_PARAMETERS, int mode) /* {{{ */
{
	zval *id;
	xmlDoc *docp = NULL, *newdoc;
	dom_object *intern;
	dom_doc_propsptr doc_prop;
	char *source;
	int source_len, refcount, ret;
	long options = 0;
	htmlParserCtxtPtr ctxt;
	
	id = getThis();

        
        id = getThis();
 
       if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p|l", &source, &source_len, &options) == FAILURE) {
                return;
        }
	}

	if (mode == DOM_LOAD_FILE) {
		ctxt = htmlCreateFileParserCtxt(source, NULL);
	} else {
		source_len = xmlStrlen(source);
		ctxt = htmlCreateMemoryParserCtxt(source, source_len);
	}

	if (!ctxt) {
		RETURN_FALSE;
	}

	if (options) {
		htmlCtxtUseOptions(ctxt, options);
	}

	ctxt->vctxt.error = php_libxml_ctx_error;
	ctxt->vctxt.warning = php_libxml_ctx_warning;
	if (ctxt->sax != NULL) {
		ctxt->sax->error = php_libxml_ctx_error;
		ctxt->sax->warning = php_libxml_ctx_warning;
	}
	htmlParseDocument(ctxt);
	newdoc = ctxt->myDoc;
	htmlFreeParserCtxt(ctxt);
	
	if (!newdoc)
		RETURN_FALSE;

	if (id != NULL && instanceof_function(Z_OBJCE_P(id), dom_document_class_entry TSRMLS_CC)) {
		intern = (dom_object *)zend_object_store_get_object(id TSRMLS_CC);
		if (intern != NULL) {
			docp = (xmlDocPtr) dom_object_get_node(intern);
			doc_prop = NULL;
			if (docp != NULL) {
				php_libxml_decrement_node_ptr((php_libxml_node_object *) intern TSRMLS_CC);
				doc_prop = intern->document->doc_props;
				intern->document->doc_props = NULL;
				refcount = php_libxml_decrement_doc_ref((php_libxml_node_object *)intern TSRMLS_CC);
				if (refcount != 0) {
					docp->_private = NULL;
				}
			}
			intern->document = NULL;
			if (php_libxml_increment_doc_ref((php_libxml_node_object *)intern, newdoc TSRMLS_CC) == -1) {
				RETURN_FALSE;
			}
			intern->document->doc_props = doc_prop;
		}

		php_libxml_increment_node_ptr((php_libxml_node_object *)intern, (xmlNodePtr)newdoc, (void *)intern TSRMLS_CC);

		RETURN_TRUE;
	} else {
		DOM_RET_OBJ((xmlNodePtr) newdoc, &ret, NULL);
	}
}
/* }}} */
