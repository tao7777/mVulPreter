xsltDocumentElem(xsltTransformContextPtr ctxt, xmlNodePtr node,
                 xmlNodePtr inst, xsltStylePreCompPtr castedComp)
{
#ifdef XSLT_REFACTORED
    xsltStyleItemDocumentPtr comp = (xsltStyleItemDocumentPtr) castedComp;
#else
    xsltStylePreCompPtr comp = castedComp;
#endif
    xsltStylesheetPtr style = NULL;
    int ret;
    xmlChar *filename = NULL, *prop, *elements;
    xmlChar *element, *end;
    xmlDocPtr res = NULL;
    xmlDocPtr oldOutput;
    xmlNodePtr oldInsert, root;
    const char *oldOutputFile;
    xsltOutputType oldType;
    xmlChar *URL = NULL;
    const xmlChar *method;
    const xmlChar *doctypePublic;
    const xmlChar *doctypeSystem;
    const xmlChar *version;
    const xmlChar *encoding;
    int redirect_write_append = 0;

    if ((ctxt == NULL) || (node == NULL) || (inst == NULL) || (comp == NULL))
        return;

    if (comp->filename == NULL) {

        if (xmlStrEqual(inst->name, (const xmlChar *) "output")) {
	    /*
	    * The element "output" is in the namespace XSLT_SAXON_NAMESPACE
	    *   (http://icl.com/saxon)
	    * The @file is in no namespace.
	    */
#ifdef WITH_XSLT_DEBUG_EXTRA
            xsltGenericDebug(xsltGenericDebugContext,
                             "Found saxon:output extension\n");
#endif
            URL = xsltEvalAttrValueTemplate(ctxt, inst,
                                                 (const xmlChar *) "file",
                                                 XSLT_SAXON_NAMESPACE);

	    if (URL == NULL)
		URL = xsltEvalAttrValueTemplate(ctxt, inst,
                                                 (const xmlChar *) "href",
                                                 XSLT_SAXON_NAMESPACE);
        } else if (xmlStrEqual(inst->name, (const xmlChar *) "write")) {
#ifdef WITH_XSLT_DEBUG_EXTRA
            xsltGenericDebug(xsltGenericDebugContext,
                             "Found xalan:write extension\n");
#endif
            URL = xsltEvalAttrValueTemplate(ctxt, inst,
                                                 (const xmlChar *)
                                                 "select",
                                                 XSLT_XALAN_NAMESPACE);
	    if (URL != NULL) {
		xmlXPathCompExprPtr cmp;
		xmlChar *val;

		/*
		 * Trying to handle bug #59212
		 * The value of the "select" attribute is an
		 * XPath expression.
		 * (see http://xml.apache.org/xalan-j/extensionslib.html#redirect)
		 */
		cmp = xmlXPathCompile(URL);
                val = xsltEvalXPathString(ctxt, cmp);
		xmlXPathFreeCompExpr(cmp);
		xmlFree(URL);
		URL = val;
	    }
	    if (URL == NULL)
		URL = xsltEvalAttrValueTemplate(ctxt, inst,
						     (const xmlChar *)
						     "file",
						     XSLT_XALAN_NAMESPACE);
	    if (URL == NULL)
		URL = xsltEvalAttrValueTemplate(ctxt, inst,
						     (const xmlChar *)
						     "href",
						     XSLT_XALAN_NAMESPACE);
        } else if (xmlStrEqual(inst->name, (const xmlChar *) "document")) {
            URL = xsltEvalAttrValueTemplate(ctxt, inst,
                                                 (const xmlChar *) "href",
                                                 NULL);
        }

    } else {
        URL = xmlStrdup(comp->filename);
    }

    if (URL == NULL) {
	xsltTransformError(ctxt, NULL, inst,
		         "xsltDocumentElem: href/URI-Reference not found\n");
	return;
    }

    /*
     * If the computation failed, it's likely that the URL wasn't escaped
     */
    filename = xmlBuildURI(URL, (const xmlChar *) ctxt->outputFile);
    if (filename == NULL) {
	xmlChar *escURL;

	escURL=xmlURIEscapeStr(URL, BAD_CAST ":/.?,");
	if (escURL != NULL) {
	    filename = xmlBuildURI(escURL, (const xmlChar *) ctxt->outputFile);
	    xmlFree(escURL);
	}
    }

    if (filename == NULL) {
	xsltTransformError(ctxt, NULL, inst,
		         "xsltDocumentElem: URL computation failed for %s\n",
			 URL);
	xmlFree(URL);
	return;
    }

    /*
     * Security checking: can we write to this resource
     */
    if (ctxt->sec != NULL) {
	ret = xsltCheckWrite(ctxt->sec, ctxt, filename);
	if (ret == 0) {
	    xsltTransformError(ctxt, NULL, inst,
		 "xsltDocumentElem: write rights for %s denied\n",
			     filename);
	    xmlFree(URL);
	    xmlFree(filename);
	    return;
	}
    }

    oldOutputFile = ctxt->outputFile;
    oldOutput = ctxt->output;
    oldInsert = ctxt->insert;
    oldType = ctxt->type;
    ctxt->outputFile = (const char *) filename;

    style = xsltNewStylesheet();
    if (style == NULL) {
	xsltTransformError(ctxt, NULL, inst,
                         "xsltDocumentElem: out of memory\n");
        goto error;
    }

    /*
     * Version described in 1.1 draft allows full parameterization
     * of the output.
     */
    prop = xsltEvalAttrValueTemplate(ctxt, inst,
				     (const xmlChar *) "version",
				     NULL);
    if (prop != NULL) {
	if (style->version != NULL)
	    xmlFree(style->version);
	style->version = prop;
    }
    prop = xsltEvalAttrValueTemplate(ctxt, inst,
				     (const xmlChar *) "encoding",
				     NULL);
    if (prop != NULL) {
	if (style->encoding != NULL)
	    xmlFree(style->encoding);
	style->encoding = prop;
    }
    prop = xsltEvalAttrValueTemplate(ctxt, inst,
				     (const xmlChar *) "method",
				     NULL);
    if (prop != NULL) {
	const xmlChar *URI;

	if (style->method != NULL)
	    xmlFree(style->method);
	style->method = NULL;
	if (style->methodURI != NULL)
	    xmlFree(style->methodURI);
	style->methodURI = NULL;

	URI = xsltGetQNameURI(inst, &prop);
	if (prop == NULL) {
	    if (style != NULL) style->errors++;
	} else if (URI == NULL) {
	    if ((xmlStrEqual(prop, (const xmlChar *) "xml")) ||
		(xmlStrEqual(prop, (const xmlChar *) "html")) ||
		(xmlStrEqual(prop, (const xmlChar *) "text"))) {
		style->method = prop;
	    } else {
		xsltTransformError(ctxt, NULL, inst,
				 "invalid value for method: %s\n", prop);
		if (style != NULL) style->warnings++;
	    }
	} else {
	    style->method = prop;
	    style->methodURI = xmlStrdup(URI);
	}
    }
    prop = xsltEvalAttrValueTemplate(ctxt, inst,
				     (const xmlChar *)
				     "doctype-system", NULL);
    if (prop != NULL) {
	if (style->doctypeSystem != NULL)
	    xmlFree(style->doctypeSystem);
	style->doctypeSystem = prop;
    }
    prop = xsltEvalAttrValueTemplate(ctxt, inst,
				     (const xmlChar *)
				     "doctype-public", NULL);
    if (prop != NULL) {
	if (style->doctypePublic != NULL)
	    xmlFree(style->doctypePublic);
	style->doctypePublic = prop;
    }
    prop = xsltEvalAttrValueTemplate(ctxt, inst,
				     (const xmlChar *) "standalone",
				     NULL);
    if (prop != NULL) {
	if (xmlStrEqual(prop, (const xmlChar *) "yes")) {
	    style->standalone = 1;
	} else if (xmlStrEqual(prop, (const xmlChar *) "no")) {
	    style->standalone = 0;
	} else {
	    xsltTransformError(ctxt, NULL, inst,
			     "invalid value for standalone: %s\n",
			     prop);
	    if (style != NULL) style->warnings++;
	}
	xmlFree(prop);
    }

    prop = xsltEvalAttrValueTemplate(ctxt, inst,
				     (const xmlChar *) "indent",
				     NULL);
    if (prop != NULL) {
	if (xmlStrEqual(prop, (const xmlChar *) "yes")) {
	    style->indent = 1;
	} else if (xmlStrEqual(prop, (const xmlChar *) "no")) {
	    style->indent = 0;
	} else {
	    xsltTransformError(ctxt, NULL, inst,
			     "invalid value for indent: %s\n", prop);
	    if (style != NULL) style->warnings++;
	}
	xmlFree(prop);
    }

    prop = xsltEvalAttrValueTemplate(ctxt, inst,
				     (const xmlChar *)
				     "omit-xml-declaration",
				     NULL);
    if (prop != NULL) {
	if (xmlStrEqual(prop, (const xmlChar *) "yes")) {
	    style->omitXmlDeclaration = 1;
	} else if (xmlStrEqual(prop, (const xmlChar *) "no")) {
	    style->omitXmlDeclaration = 0;
	} else {
	    xsltTransformError(ctxt, NULL, inst,
			     "invalid value for omit-xml-declaration: %s\n",
			     prop);
	    if (style != NULL) style->warnings++;
	}
	xmlFree(prop);
    }

    elements = xsltEvalAttrValueTemplate(ctxt, inst,
					 (const xmlChar *)
					 "cdata-section-elements",
					 NULL);
    if (elements != NULL) {
	if (style->stripSpaces == NULL)
	    style->stripSpaces = xmlHashCreate(10);
	if (style->stripSpaces == NULL)
	    return;

	element = elements;
	while (*element != 0) {
	    while (IS_BLANK_CH(*element))
		element++;
	    if (*element == 0)
		break;
	    end = element;
	    while ((*end != 0) && (!IS_BLANK_CH(*end)))
		end++;
	    element = xmlStrndup(element, end - element);
	    if (element) {
		const xmlChar *URI;

#ifdef WITH_XSLT_DEBUG_PARSING
		xsltGenericDebug(xsltGenericDebugContext,
				 "add cdata section output element %s\n",
				 element);
#endif
                URI = xsltGetQNameURI(inst, &element);

		xmlHashAddEntry2(style->stripSpaces, element, URI,
			        (xmlChar *) "cdata");
		xmlFree(element);
	    }
	    element = end;
	}
	xmlFree(elements);
    }

    /*
     * Create a new document tree and process the element template
     */
    XSLT_GET_IMPORT_PTR(method, style, method)
    XSLT_GET_IMPORT_PTR(doctypePublic, style, doctypePublic)
    XSLT_GET_IMPORT_PTR(doctypeSystem, style, doctypeSystem)
    XSLT_GET_IMPORT_PTR(version, style, version)
    XSLT_GET_IMPORT_PTR(encoding, style, encoding)

    if ((method != NULL) &&
	(!xmlStrEqual(method, (const xmlChar *) "xml"))) {
	if (xmlStrEqual(method, (const xmlChar *) "html")) {
	    ctxt->type = XSLT_OUTPUT_HTML;
	    if (((doctypePublic != NULL) || (doctypeSystem != NULL)))
		res = htmlNewDoc(doctypeSystem, doctypePublic);
	    else {
		if (version != NULL) {
#ifdef XSLT_GENERATE_HTML_DOCTYPE
		    xsltGetHTMLIDs(version, &doctypePublic, &doctypeSystem);
#endif
                }
		res = htmlNewDocNoDtD(doctypeSystem, doctypePublic);
	    }
	    if (res == NULL)
		goto error;
	    res->dict = ctxt->dict;
 	    xmlDictReference(res->dict);
 	} else if (xmlStrEqual(method, (const xmlChar *) "xhtml")) {
 	    xsltTransformError(ctxt, NULL, inst,
	     "xsltDocumentElem: unsupported method xhtml\n",
		             style->method);
 	    ctxt->type = XSLT_OUTPUT_HTML;
 	    res = htmlNewDocNoDtD(doctypeSystem, doctypePublic);
 	    if (res == NULL)
		goto error;
	    res->dict = ctxt->dict;
	    xmlDictReference(res->dict);
	} else if (xmlStrEqual(method, (const xmlChar *) "text")) {
	    ctxt->type = XSLT_OUTPUT_TEXT;
	    res = xmlNewDoc(style->version);
	    if (res == NULL)
		goto error;
	    res->dict = ctxt->dict;
	    xmlDictReference(res->dict);
#ifdef WITH_XSLT_DEBUG
	    xsltGenericDebug(xsltGenericDebugContext,
                     "reusing transformation dict for output\n");
 #endif
 	} else {
 	    xsltTransformError(ctxt, NULL, inst,
			     "xsltDocumentElem: unsupported method %s\n",
		             style->method);
 	    goto error;
 	}
     } else {
	ctxt->type = XSLT_OUTPUT_XML;
	res = xmlNewDoc(style->version);
	if (res == NULL)
	    goto error;
	res->dict = ctxt->dict;
	xmlDictReference(res->dict);
#ifdef WITH_XSLT_DEBUG
	xsltGenericDebug(xsltGenericDebugContext,
                     "reusing transformation dict for output\n");
#endif
    }
    res->charset = XML_CHAR_ENCODING_UTF8;
    if (encoding != NULL)
	res->encoding = xmlStrdup(encoding);
    ctxt->output = res;
    ctxt->insert = (xmlNodePtr) res;
    xsltApplySequenceConstructor(ctxt, node, inst->children, NULL);

    /*
     * Do some post processing work depending on the generated output
     */
    root = xmlDocGetRootElement(res);
    if (root != NULL) {
        const xmlChar *doctype = NULL;

        if ((root->ns != NULL) && (root->ns->prefix != NULL))
	    doctype = xmlDictQLookup(ctxt->dict, root->ns->prefix, root->name);
	if (doctype == NULL)
	    doctype = root->name;

        /*
         * Apply the default selection of the method
         */
        if ((method == NULL) &&
            (root->ns == NULL) &&
            (!xmlStrcasecmp(root->name, (const xmlChar *) "html"))) {
            xmlNodePtr tmp;

            tmp = res->children;
            while ((tmp != NULL) && (tmp != root)) {
                if (tmp->type == XML_ELEMENT_NODE)
                    break;
                if ((tmp->type == XML_TEXT_NODE) && (!xmlIsBlankNode(tmp)))
                    break;
		tmp = tmp->next;
            }
            if (tmp == root) {
                ctxt->type = XSLT_OUTPUT_HTML;
                res->type = XML_HTML_DOCUMENT_NODE;
                if (((doctypePublic != NULL) || (doctypeSystem != NULL))) {
                    res->intSubset = xmlCreateIntSubset(res, doctype,
                                                        doctypePublic,
                                                        doctypeSystem);
#ifdef XSLT_GENERATE_HTML_DOCTYPE
		} else if (version != NULL) {
                    xsltGetHTMLIDs(version, &doctypePublic,
                                   &doctypeSystem);
                    if (((doctypePublic != NULL) || (doctypeSystem != NULL)))
                        res->intSubset =
                            xmlCreateIntSubset(res, doctype,
                                               doctypePublic,
                                               doctypeSystem);
#endif
                }
            }

        }
        if (ctxt->type == XSLT_OUTPUT_XML) {
            XSLT_GET_IMPORT_PTR(doctypePublic, style, doctypePublic)
                XSLT_GET_IMPORT_PTR(doctypeSystem, style, doctypeSystem)
                if (((doctypePublic != NULL) || (doctypeSystem != NULL)))
                res->intSubset = xmlCreateIntSubset(res, doctype,
                                                    doctypePublic,
                                                    doctypeSystem);
        }
    }

    /*
     * Calls to redirect:write also take an optional attribute append.
     * Attribute append="true|yes" which will attempt to simply append
     * to an existing file instead of always opening a new file. The
     * default behavior of always overwriting the file still happens
     * if we do not specify append.
     * Note that append use will forbid use of remote URI target.
     */
    prop = xsltEvalAttrValueTemplate(ctxt, inst, (const xmlChar *)"append",
				     NULL);
    if (prop != NULL) {
	if (xmlStrEqual(prop, (const xmlChar *) "true") ||
	    xmlStrEqual(prop, (const xmlChar *) "yes")) {
	    style->omitXmlDeclaration = 1;
	    redirect_write_append = 1;
	} else
	    style->omitXmlDeclaration = 0;
	xmlFree(prop);
    }

    if (redirect_write_append) {
        FILE *f;

	f = fopen((const char *) filename, "ab");
	if (f == NULL) {
	    ret = -1;
	} else {
	    ret = xsltSaveResultToFile(f, res, style);
	    fclose(f);
	}
    } else {
	ret = xsltSaveResultToFilename((const char *) filename, res, style, 0);
    }
    if (ret < 0) {
	xsltTransformError(ctxt, NULL, inst,
                         "xsltDocumentElem: unable to save to %s\n",
                         filename);
	ctxt->state = XSLT_STATE_ERROR;
#ifdef WITH_XSLT_DEBUG_EXTRA
    } else {
        xsltGenericDebug(xsltGenericDebugContext,
                         "Wrote %d bytes to %s\n", ret, filename);
#endif
    }

  error:
    ctxt->output = oldOutput;
    ctxt->insert = oldInsert;
    ctxt->type = oldType;
    ctxt->outputFile = oldOutputFile;
    if (URL != NULL)
        xmlFree(URL);
    if (filename != NULL)
        xmlFree(filename);
    if (style != NULL)
        xsltFreeStylesheet(style);
    if (res != NULL)
        xmlFreeDoc(res);
}
