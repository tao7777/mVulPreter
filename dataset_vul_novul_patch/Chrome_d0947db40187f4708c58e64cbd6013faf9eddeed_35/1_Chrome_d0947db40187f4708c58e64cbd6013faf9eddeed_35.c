xmlParseStringPEReference(xmlParserCtxtPtr ctxt, const xmlChar **str) {
    const xmlChar *ptr;
    xmlChar cur;
    xmlChar *name;
    xmlEntityPtr entity = NULL;

    if ((str == NULL) || (*str == NULL)) return(NULL);
    ptr = *str;
    cur = *ptr;
    if (cur != '%')
        return(NULL);
    ptr++;
    name = xmlParseStringName(ctxt, &ptr);
    if (name == NULL) {
	xmlFatalErrMsg(ctxt, XML_ERR_NAME_REQUIRED,
		       "xmlParseStringPEReference: no name\n");
	*str = ptr;
	return(NULL);
    }
    cur = *ptr;
    if (cur != ';') {
	xmlFatalErr(ctxt, XML_ERR_ENTITYREF_SEMICOL_MISSING, NULL);
	xmlFree(name);
	*str = ptr;
	return(NULL);
    }
    ptr++;

    /*
     * Increate the number of entity references parsed
     */
    ctxt->nbentities++;

    /*
     * Request the entity from SAX
      */
     if ((ctxt->sax != NULL) &&
 	(ctxt->sax->getParameterEntity != NULL))
	entity = ctxt->sax->getParameterEntity(ctxt->userData,
					       name);
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
	    ((ctxt->hasExternalSubset == 0) && (ctxt->hasPErefs == 0))) {
	    xmlFatalErrMsgStr(ctxt, XML_ERR_UNDECLARED_ENTITY,
		 "PEReference: %%%s; not found\n", name);
	} else {
	    /*
	     * [ VC: Entity Declared ]
	     * In a document with an external subset or external
	     * parameter entities with "standalone='no'", ...
	     * ... The declaration of a parameter entity must
	     * precede any reference to it...
	     */
	    xmlWarningMsg(ctxt, XML_WAR_UNDECLARED_ENTITY,
			  "PEReference: %%%s; not found\n",
			  name, NULL);
	    ctxt->valid = 0;
	}
    } else {
	/*
	 * Internal checking in case the entity quest barfed
	 */
	if ((entity->etype != XML_INTERNAL_PARAMETER_ENTITY) &&
	    (entity->etype != XML_EXTERNAL_PARAMETER_ENTITY)) {
	    xmlWarningMsg(ctxt, XML_WAR_UNDECLARED_ENTITY,
			  "%%%s; is not a parameter entity\n",
			  name, NULL);
	}
    }
    ctxt->hasPErefs = 1;
    xmlFree(name);
    *str = ptr;
    return(entity);
}
