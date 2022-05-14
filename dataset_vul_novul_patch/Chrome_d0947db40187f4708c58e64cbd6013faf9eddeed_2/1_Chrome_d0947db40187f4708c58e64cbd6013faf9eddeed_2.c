xmlParseAttributeListDecl(xmlParserCtxtPtr ctxt) {
    const xmlChar *elemName;
    const xmlChar *attrName;
    xmlEnumerationPtr tree;

    if (CMP9(CUR_PTR, '<', '!', 'A', 'T', 'T', 'L', 'I', 'S', 'T')) {
	xmlParserInputPtr input = ctxt->input;

	SKIP(9);
	if (!IS_BLANK_CH(CUR)) {
	    xmlFatalErrMsg(ctxt, XML_ERR_SPACE_REQUIRED,
		                 "Space required after '<!ATTLIST'\n");
	}
        SKIP_BLANKS;
        elemName = xmlParseName(ctxt);
	if (elemName == NULL) {
	    xmlFatalErrMsg(ctxt, XML_ERR_NAME_REQUIRED,
			   "ATTLIST: no name for Element\n");
	    return;
 	}
 	SKIP_BLANKS;
 	GROW;
	while (RAW != '>') {
 	    const xmlChar *check = CUR_PTR;
 	    int type;
 	    int def;
	    xmlChar *defaultValue = NULL;

	    GROW;
            tree = NULL;
	    attrName = xmlParseName(ctxt);
	    if (attrName == NULL) {
		xmlFatalErrMsg(ctxt, XML_ERR_NAME_REQUIRED,
			       "ATTLIST: no name for Attribute\n");
		break;
	    }
	    GROW;
	    if (!IS_BLANK_CH(CUR)) {
		xmlFatalErrMsg(ctxt, XML_ERR_SPACE_REQUIRED,
		        "Space required after the attribute name\n");
		break;
	    }
	    SKIP_BLANKS;

	    type = xmlParseAttributeType(ctxt, &tree);
	    if (type <= 0) {
	        break;
	    }

	    GROW;
	    if (!IS_BLANK_CH(CUR)) {
		xmlFatalErrMsg(ctxt, XML_ERR_SPACE_REQUIRED,
			       "Space required after the attribute type\n");
	        if (tree != NULL)
		    xmlFreeEnumeration(tree);
		break;
	    }
	    SKIP_BLANKS;

	    def = xmlParseDefaultDecl(ctxt, &defaultValue);
	    if (def <= 0) {
                if (defaultValue != NULL)
		    xmlFree(defaultValue);
	        if (tree != NULL)
		    xmlFreeEnumeration(tree);
	        break;
	    }
	    if ((type != XML_ATTRIBUTE_CDATA) && (defaultValue != NULL))
	        xmlAttrNormalizeSpace(defaultValue, defaultValue);

	    GROW;
            if (RAW != '>') {
		if (!IS_BLANK_CH(CUR)) {
		    xmlFatalErrMsg(ctxt, XML_ERR_SPACE_REQUIRED,
			"Space required after the attribute default value\n");
		    if (defaultValue != NULL)
			xmlFree(defaultValue);
		    if (tree != NULL)
			xmlFreeEnumeration(tree);
		    break;
		}
		SKIP_BLANKS;
	    }
	    if (check == CUR_PTR) {
		xmlFatalErr(ctxt, XML_ERR_INTERNAL_ERROR,
		            "in xmlParseAttributeListDecl\n");
		if (defaultValue != NULL)
		    xmlFree(defaultValue);
	        if (tree != NULL)
		    xmlFreeEnumeration(tree);
		break;
	    }
	    if ((ctxt->sax != NULL) && (!ctxt->disableSAX) &&
		(ctxt->sax->attributeDecl != NULL))
		ctxt->sax->attributeDecl(ctxt->userData, elemName, attrName,
	                        type, def, defaultValue, tree);
	    else if (tree != NULL)
		xmlFreeEnumeration(tree);

	    if ((ctxt->sax2) && (defaultValue != NULL) &&
	        (def != XML_ATTRIBUTE_IMPLIED) && 
		(def != XML_ATTRIBUTE_REQUIRED)) {
		xmlAddDefAttrs(ctxt, elemName, attrName, defaultValue);
	    }
	    if (ctxt->sax2) {
		xmlAddSpecialAttr(ctxt, elemName, attrName, type);
	    }
	    if (defaultValue != NULL)
	        xmlFree(defaultValue);
	    GROW;
	}
	if (RAW == '>') {
	    if (input != ctxt->input) {
		xmlValidityError(ctxt, XML_ERR_ENTITY_BOUNDARY,
    "Attribute list declaration doesn't start and stop in the same entity\n",
                                 NULL, NULL);
	    }
	    NEXT;
	}
    }
}
