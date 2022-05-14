xmlParseEntityDecl(xmlParserCtxtPtr ctxt) {
    const xmlChar *name = NULL;
    xmlChar *value = NULL;
    xmlChar *URI = NULL, *literal = NULL;
    const xmlChar *ndata = NULL;
    int isParameter = 0;
    xmlChar *orig = NULL;
    int skipped;
    
    /* GROW; done in the caller */
    if (CMP8(CUR_PTR, '<', '!', 'E', 'N', 'T', 'I', 'T', 'Y')) {
	xmlParserInputPtr input = ctxt->input;
	SHRINK;
	SKIP(8);
	skipped = SKIP_BLANKS;
	if (skipped == 0) {
	    xmlFatalErrMsg(ctxt, XML_ERR_SPACE_REQUIRED,
			   "Space required after '<!ENTITY'\n");
	}

	if (RAW == '%') {
	    NEXT;
	    skipped = SKIP_BLANKS;
	    if (skipped == 0) {
		xmlFatalErrMsg(ctxt, XML_ERR_SPACE_REQUIRED,
			       "Space required after '%'\n");
	    }
	    isParameter = 1;
	}

        name = xmlParseName(ctxt);
	if (name == NULL) {
	    xmlFatalErrMsg(ctxt, XML_ERR_NAME_REQUIRED,
	                   "xmlParseEntityDecl: no name\n");
            return;
	}
	if (xmlStrchr(name, ':') != NULL) {
	    xmlNsErr(ctxt, XML_NS_ERR_COLON, 
		     "colon are forbidden from entities names '%s'\n",
		     name, NULL, NULL);
	}
        skipped = SKIP_BLANKS;
	if (skipped == 0) {
	    xmlFatalErrMsg(ctxt, XML_ERR_SPACE_REQUIRED,
			   "Space required after the entity name\n");
	}

	ctxt->instate = XML_PARSER_ENTITY_DECL;
	/*
	 * handle the various case of definitions...
	 */
	if (isParameter) {
	    if ((RAW == '"') || (RAW == '\'')) {
	        value = xmlParseEntityValue(ctxt, &orig);
		if (value) {
		    if ((ctxt->sax != NULL) &&
			(!ctxt->disableSAX) && (ctxt->sax->entityDecl != NULL))
			ctxt->sax->entityDecl(ctxt->userData, name,
		                    XML_INTERNAL_PARAMETER_ENTITY,
				    NULL, NULL, value);
		}
	    } else {
	        URI = xmlParseExternalID(ctxt, &literal, 1);
		if ((URI == NULL) && (literal == NULL)) {
		    xmlFatalErr(ctxt, XML_ERR_VALUE_REQUIRED, NULL);
		}
		if (URI) {
		    xmlURIPtr uri;

		    uri = xmlParseURI((const char *) URI);
		    if (uri == NULL) {
		        xmlErrMsgStr(ctxt, XML_ERR_INVALID_URI,
				     "Invalid URI: %s\n", URI);
			/*
			 * This really ought to be a well formedness error
			 * but the XML Core WG decided otherwise c.f. issue
			 * E26 of the XML erratas.
			 */
		    } else {
			if (uri->fragment != NULL) {
			    /*
			     * Okay this is foolish to block those but not
			     * invalid URIs.
			     */
			    xmlFatalErr(ctxt, XML_ERR_URI_FRAGMENT, NULL);
			} else {
			    if ((ctxt->sax != NULL) &&
				(!ctxt->disableSAX) &&
				(ctxt->sax->entityDecl != NULL))
				ctxt->sax->entityDecl(ctxt->userData, name,
					    XML_EXTERNAL_PARAMETER_ENTITY,
					    literal, URI, NULL);
			}
			xmlFreeURI(uri);
		    }
		}
	    }
	} else {
	    if ((RAW == '"') || (RAW == '\'')) {
	        value = xmlParseEntityValue(ctxt, &orig);
		if ((ctxt->sax != NULL) &&
		    (!ctxt->disableSAX) && (ctxt->sax->entityDecl != NULL))
		    ctxt->sax->entityDecl(ctxt->userData, name,
				XML_INTERNAL_GENERAL_ENTITY,
				NULL, NULL, value);
		/*
		 * For expat compatibility in SAX mode.
		 */
		if ((ctxt->myDoc == NULL) ||
		    (xmlStrEqual(ctxt->myDoc->version, SAX_COMPAT_MODE))) {
		    if (ctxt->myDoc == NULL) {
			ctxt->myDoc = xmlNewDoc(SAX_COMPAT_MODE);
			if (ctxt->myDoc == NULL) {
			    xmlErrMemory(ctxt, "New Doc failed");
			    return;
			}
			ctxt->myDoc->properties = XML_DOC_INTERNAL;
		    }
		    if (ctxt->myDoc->intSubset == NULL)
			ctxt->myDoc->intSubset = xmlNewDtd(ctxt->myDoc,
					    BAD_CAST "fake", NULL, NULL);

		    xmlSAX2EntityDecl(ctxt, name, XML_INTERNAL_GENERAL_ENTITY,
			              NULL, NULL, value);
		}
	    } else {
	        URI = xmlParseExternalID(ctxt, &literal, 1);
		if ((URI == NULL) && (literal == NULL)) {
		    xmlFatalErr(ctxt, XML_ERR_VALUE_REQUIRED, NULL);
		}
		if (URI) {
		    xmlURIPtr uri;

		    uri = xmlParseURI((const char *)URI);
		    if (uri == NULL) {
		        xmlErrMsgStr(ctxt, XML_ERR_INVALID_URI,
				     "Invalid URI: %s\n", URI);
			/*
			 * This really ought to be a well formedness error
			 * but the XML Core WG decided otherwise c.f. issue
			 * E26 of the XML erratas.
			 */
		    } else {
			if (uri->fragment != NULL) {
			    /*
			     * Okay this is foolish to block those but not
			     * invalid URIs.
			     */
			    xmlFatalErr(ctxt, XML_ERR_URI_FRAGMENT, NULL);
			}
			xmlFreeURI(uri);
		    }
		}
		if ((RAW != '>') && (!IS_BLANK_CH(CUR))) {
		    xmlFatalErrMsg(ctxt, XML_ERR_SPACE_REQUIRED,
				   "Space required before 'NDATA'\n");
		}
		SKIP_BLANKS;
		if (CMP5(CUR_PTR, 'N', 'D', 'A', 'T', 'A')) {
		    SKIP(5);
		    if (!IS_BLANK_CH(CUR)) {
			xmlFatalErrMsg(ctxt, XML_ERR_SPACE_REQUIRED,
				       "Space required after 'NDATA'\n");
		    }
		    SKIP_BLANKS;
		    ndata = xmlParseName(ctxt);
		    if ((ctxt->sax != NULL) && (!ctxt->disableSAX) &&
		        (ctxt->sax->unparsedEntityDecl != NULL))
			ctxt->sax->unparsedEntityDecl(ctxt->userData, name,
				    literal, URI, ndata);
		} else {
		    if ((ctxt->sax != NULL) &&
		        (!ctxt->disableSAX) && (ctxt->sax->entityDecl != NULL))
			ctxt->sax->entityDecl(ctxt->userData, name,
				    XML_EXTERNAL_GENERAL_PARSED_ENTITY,
				    literal, URI, NULL);
		    /*
		     * For expat compatibility in SAX mode.
		     * assuming the entity repalcement was asked for
		     */
		    if ((ctxt->replaceEntities != 0) &&
			((ctxt->myDoc == NULL) ||
			(xmlStrEqual(ctxt->myDoc->version, SAX_COMPAT_MODE)))) {
			if (ctxt->myDoc == NULL) {
			    ctxt->myDoc = xmlNewDoc(SAX_COMPAT_MODE);
			    if (ctxt->myDoc == NULL) {
			        xmlErrMemory(ctxt, "New Doc failed");
				return;
			    }
			    ctxt->myDoc->properties = XML_DOC_INTERNAL;
			}

			if (ctxt->myDoc->intSubset == NULL)
			    ctxt->myDoc->intSubset = xmlNewDtd(ctxt->myDoc,
						BAD_CAST "fake", NULL, NULL);
			xmlSAX2EntityDecl(ctxt, name,
				          XML_EXTERNAL_GENERAL_PARSED_ENTITY,
				          literal, URI, NULL);
		    }
 		}
 	    }
 	}
 	SKIP_BLANKS;
 	if (RAW != '>') {
 	    xmlFatalErrMsgStr(ctxt, XML_ERR_ENTITY_NOT_FINISHED,
	            "xmlParseEntityDecl: entity %s not terminated\n", name);
	} else {
	    if (input != ctxt->input) {
		xmlFatalErrMsg(ctxt, XML_ERR_ENTITY_BOUNDARY,
	"Entity declaration doesn't start and stop in the same entity\n");
	    }
	    NEXT;
	}
	if (orig != NULL) {
	    /*
	     * Ugly mechanism to save the raw entity value.
	     */
	    xmlEntityPtr cur = NULL;

	    if (isParameter) {
	        if ((ctxt->sax != NULL) &&
		    (ctxt->sax->getParameterEntity != NULL))
		    cur = ctxt->sax->getParameterEntity(ctxt->userData, name);
	    } else {
	        if ((ctxt->sax != NULL) &&
		    (ctxt->sax->getEntity != NULL))
		    cur = ctxt->sax->getEntity(ctxt->userData, name);
		if ((cur == NULL) && (ctxt->userData==ctxt)) {
		    cur = xmlSAX2GetEntity(ctxt, name);
		}
	    }
            if (cur != NULL) {
	        if (cur->orig != NULL)
		    xmlFree(orig);
		else
		    cur->orig = orig;
	    } else
		xmlFree(orig);
	}
	if (value != NULL) xmlFree(value);
	if (URI != NULL) xmlFree(URI);
	if (literal != NULL) xmlFree(literal);
    }
}
