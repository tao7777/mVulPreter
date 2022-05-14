xmlParseEntityRef(xmlParserCtxtPtr ctxt) {
    const xmlChar *name;
     xmlEntityPtr ent = NULL;
 
     GROW;
 
     if (RAW != '&')
         return(NULL);
    NEXT;
    name = xmlParseName(ctxt);
    if (name == NULL) {
	xmlFatalErrMsg(ctxt, XML_ERR_NAME_REQUIRED,
		       "xmlParseEntityRef: no name\n");
        return(NULL);
    }
    if (RAW != ';') {
	xmlFatalErr(ctxt, XML_ERR_ENTITYREF_SEMICOL_MISSING, NULL);
	return(NULL);
    }
    NEXT;

    /*
     * Predefined entites override any extra definition
     */
    if ((ctxt->options & XML_PARSE_OLDSAX) == 0) {
        ent = xmlGetPredefinedEntity(name);
        if (ent != NULL)
            return(ent);
    }

    /*
     * Increate the number of entity references parsed
     */
    ctxt->nbentities++;

    /*
     * Ask first SAX for entity resolution, otherwise try the
     * entities which may have stored in the parser context.
     */
    if (ctxt->sax != NULL) {
	if (ctxt->sax->getEntity != NULL)
	    ent = ctxt->sax->getEntity(ctxt->userData, name);
	if ((ctxt->wellFormed == 1 ) && (ent == NULL) && 
	    (ctxt->options & XML_PARSE_OLDSAX))
	    ent = xmlGetPredefinedEntity(name);
	if ((ctxt->wellFormed == 1 ) && (ent == NULL) &&
	    (ctxt->userData==ctxt)) {
 	    ent = xmlSAX2GetEntity(ctxt, name);
 	}
     }
     /*
      * [ WFC: Entity Declared ]
      * In a document without any DTD, a document with only an
     * internal DTD subset which contains no parameter entity
     * references, or a document with "standalone='yes'", the
     * Name given in the entity reference must match that in an
     * entity declaration, except that well-formed documents
     * need not declare any of the following entities: amp, lt,
     * gt, apos, quot.
     * The declaration of a parameter entity must precede any
     * reference to it.
     * Similarly, the declaration of a general entity must
     * precede any reference to it which appears in a default
     * value in an attribute-list declaration. Note that if
     * entities are declared in the external subset or in
     * external parameter entities, a non-validating processor
     * is not obligated to read and process their declarations;
     * for such documents, the rule that an entity must be
     * declared is a well-formedness constraint only if
     * standalone='yes'.
     */
    if (ent == NULL) {
	if ((ctxt->standalone == 1) ||
	    ((ctxt->hasExternalSubset == 0) &&
	     (ctxt->hasPErefs == 0))) {
	    xmlFatalErrMsgStr(ctxt, XML_ERR_UNDECLARED_ENTITY,
		     "Entity '%s' not defined\n", name);
	} else {
	    xmlErrMsgStr(ctxt, XML_WAR_UNDECLARED_ENTITY,
		     "Entity '%s' not defined\n", name);
	    if ((ctxt->inSubset == 0) &&
		(ctxt->sax != NULL) &&
		(ctxt->sax->reference != NULL)) {
		ctxt->sax->reference(ctxt->userData, name);
	    }
	}
	ctxt->valid = 0;
    }

    /*
     * [ WFC: Parsed Entity ]
     * An entity reference must not contain the name of an
     * unparsed entity
     */
    else if (ent->etype == XML_EXTERNAL_GENERAL_UNPARSED_ENTITY) {
	xmlFatalErrMsgStr(ctxt, XML_ERR_UNPARSED_ENTITY,
		 "Entity reference to unparsed entity %s\n", name);
    }

    /*
     * [ WFC: No External Entity References ]
     * Attribute values cannot contain direct or indirect
     * entity references to external entities.
     */
    else if ((ctxt->instate == XML_PARSER_ATTRIBUTE_VALUE) &&
	     (ent->etype == XML_EXTERNAL_GENERAL_PARSED_ENTITY)) {
	xmlFatalErrMsgStr(ctxt, XML_ERR_ENTITY_IS_EXTERNAL,
	     "Attribute references external entity '%s'\n", name);
    }
    /*
     * [ WFC: No < in Attribute Values ]
     * The replacement text of any entity referred to directly or
     * indirectly in an attribute value (other than "&lt;") must
     * not contain a <. 
     */
    else if ((ctxt->instate == XML_PARSER_ATTRIBUTE_VALUE) &&
	     (ent != NULL) && (ent->content != NULL) &&
	     (ent->etype != XML_INTERNAL_PREDEFINED_ENTITY) &&
	     (xmlStrchr(ent->content, '<'))) {
	xmlFatalErrMsgStr(ctxt, XML_ERR_LT_IN_ATTRIBUTE,
    "'<' in entity '%s' is not allowed in attributes values\n", name);
    }

    /*
     * Internal check, no parameter entities here ...
     */
    else {
	switch (ent->etype) {
	    case XML_INTERNAL_PARAMETER_ENTITY:
	    case XML_EXTERNAL_PARAMETER_ENTITY:
	    xmlFatalErrMsgStr(ctxt, XML_ERR_ENTITY_IS_PARAMETER,
	     "Attempt to reference the parameter entity '%s'\n",
			      name);
	    break;
	    default:
	    break;
	}
    }

    /*
     * [ WFC: No Recursion ]
     * A parsed entity must not contain a recursive reference
     * to itself, either directly or indirectly. 
     * Done somewhere else
     */
    return(ent);
}
