xmlParserHandlePEReference(xmlParserCtxtPtr ctxt) {
    const xmlChar *name;
    xmlEntityPtr entity = NULL;
    xmlParserInputPtr input;

    if (RAW != '%') return;
    switch(ctxt->instate) {
	case XML_PARSER_CDATA_SECTION:
	    return;
        case XML_PARSER_COMMENT:
	    return;
	case XML_PARSER_START_TAG:
	    return;
	case XML_PARSER_END_TAG:
	    return;
        case XML_PARSER_EOF:
	    xmlFatalErr(ctxt, XML_ERR_PEREF_AT_EOF, NULL);
	    return;
        case XML_PARSER_PROLOG:
	case XML_PARSER_START:
	case XML_PARSER_MISC:
	    xmlFatalErr(ctxt, XML_ERR_PEREF_IN_PROLOG, NULL);
	    return;
	case XML_PARSER_ENTITY_DECL:
        case XML_PARSER_CONTENT:
        case XML_PARSER_ATTRIBUTE_VALUE:
        case XML_PARSER_PI:
	case XML_PARSER_SYSTEM_LITERAL:
	case XML_PARSER_PUBLIC_LITERAL:
	    /* we just ignore it there */
	    return;
        case XML_PARSER_EPILOG:
	    xmlFatalErr(ctxt, XML_ERR_PEREF_IN_EPILOG, NULL);
	    return;
	case XML_PARSER_ENTITY_VALUE:
	    /*
	     * NOTE: in the case of entity values, we don't do the
	     *       substitution here since we need the literal
	     *       entity value to be able to save the internal
	     *       subset of the document.
	     *       This will be handled by xmlStringDecodeEntities
	     */
	    return;
        case XML_PARSER_DTD:
	    /*
	     * [WFC: Well-Formedness Constraint: PEs in Internal Subset]
	     * In the internal DTD subset, parameter-entity references
	     * can occur only where markup declarations can occur, not
	     * within markup declarations.
	     * In that case this is handled in xmlParseMarkupDecl
	     */
	    if ((ctxt->external == 0) && (ctxt->inputNr == 1))
		return;
	    if (IS_BLANK_CH(NXT(1)) || NXT(1) == 0)
		return;
            break;
        case XML_PARSER_IGNORE:
            return;
    }

    NEXT;
    name = xmlParseName(ctxt);
    if (xmlParserDebugEntities)
	xmlGenericError(xmlGenericErrorContext,
		"PEReference: %s\n", name);
    if (name == NULL) {
	xmlFatalErr(ctxt, XML_ERR_PEREF_NO_NAME, NULL);
    } else {
	if (RAW == ';') {
 	    NEXT;
 	    if ((ctxt->sax != NULL) && (ctxt->sax->getParameterEntity != NULL))
 		entity = ctxt->sax->getParameterEntity(ctxt->userData, name);
	    if (ctxt->instate == XML_PARSER_EOF)
	        return;
 	    if (entity == NULL) {
 	        
 		/*
		 * [ WFC: Entity Declared ]
		 * In a document without any DTD, a document with only an
		 * internal DTD subset which contains no parameter entity
		 * references, or a document with "standalone='yes'", ...
		 * ... The declaration of a parameter entity must precede
		 * any reference to it...
		 */
		if ((ctxt->standalone == 1) ||
		    ((ctxt->hasExternalSubset == 0) &&
		     (ctxt->hasPErefs == 0))) {
		    xmlFatalErrMsgStr(ctxt, XML_ERR_UNDECLARED_ENTITY,
			 "PEReference: %%%s; not found\n", name);
	        } else {
		    /*
		     * [ VC: Entity Declared ]
		     * In a document with an external subset or external
		     * parameter entities with "standalone='no'", ...
		     * ... The declaration of a parameter entity must precede
		     * any reference to it...
		     */
		    if ((ctxt->validate) && (ctxt->vctxt.error != NULL)) {
		        xmlValidityError(ctxt, XML_WAR_UNDECLARED_ENTITY,
			                 "PEReference: %%%s; not found\n",
				         name, NULL);
		    } else 
		        xmlWarningMsg(ctxt, XML_WAR_UNDECLARED_ENTITY,
			              "PEReference: %%%s; not found\n",
				      name, NULL);
		    ctxt->valid = 0;
		}
	    } else if (ctxt->input->free != deallocblankswrapper) {
		    input = xmlNewBlanksWrapperInputStream(ctxt, entity);
		    if (xmlPushInput(ctxt, input) < 0)
		        return;
	    } else {
	        if ((entity->etype == XML_INTERNAL_PARAMETER_ENTITY) ||
		    (entity->etype == XML_EXTERNAL_PARAMETER_ENTITY)) {
		    xmlChar start[4];
		    xmlCharEncoding enc;

		    /*
		     * handle the extra spaces added before and after
		     * c.f. http://www.w3.org/TR/REC-xml#as-PE
		     * this is done independently.
		     */
		    input = xmlNewEntityInputStream(ctxt, entity);
		    if (xmlPushInput(ctxt, input) < 0)
		        return;

		    /* 
		     * Get the 4 first bytes and decode the charset
		     * if enc != XML_CHAR_ENCODING_NONE
		     * plug some encoding conversion routines.
		     * Note that, since we may have some non-UTF8
		     * encoding (like UTF16, bug 135229), the 'length'
		     * is not known, but we can calculate based upon
 		     * the amount of data in the buffer.
 		     */
 		    GROW
                    if (ctxt->instate == XML_PARSER_EOF)
                        return;
 		    if ((ctxt->input->end - ctxt->input->cur)>=4) {
 			start[0] = RAW;
 			start[1] = NXT(1);
			start[2] = NXT(2);
			start[3] = NXT(3);
			enc = xmlDetectCharEncoding(start, 4);
			if (enc != XML_CHAR_ENCODING_NONE) {
			    xmlSwitchEncoding(ctxt, enc);
			}
		    }

		    if ((entity->etype == XML_EXTERNAL_PARAMETER_ENTITY) &&
			(CMP5(CUR_PTR, '<', '?', 'x', 'm', 'l' )) &&
			(IS_BLANK_CH(NXT(5)))) {
			xmlParseTextDecl(ctxt);
		    }
		} else {
		    xmlFatalErrMsgStr(ctxt, XML_ERR_ENTITY_IS_PARAMETER,
			     "PEReference: %s is not a parameter entity\n",
				      name);
		}
	    }
	} else {
	    xmlFatalErr(ctxt, XML_ERR_PEREF_SEMICOL_MISSING, NULL);
	}
    }
}
