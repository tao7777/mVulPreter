xmlParsePEReference(xmlParserCtxtPtr ctxt)
{
 const xmlChar *name;
    xmlEntityPtr entity = NULL;
    xmlParserInputPtr input;

 if (RAW != '%')
 return;
    NEXT;
    name = xmlParseName(ctxt);
 if (name == NULL) {
	xmlFatalErrMsg(ctxt, XML_ERR_NAME_REQUIRED,
 "xmlParsePEReference: no name\n");
 return;
 }
 if (RAW != ';') {
	xmlFatalErr(ctxt, XML_ERR_ENTITYREF_SEMICOL_MISSING, NULL);
 return;
 }

    NEXT;

 /*
     * Increate the number of entity references parsed
     */
    ctxt->nbentities++;

 /*
     * Request the entity from SAX
     */
 if ((ctxt->sax != NULL) &&
 (ctxt->sax->getParameterEntity != NULL))
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
 "PEReference: %%%s; not found\n",
			      name);
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
	xmlParserEntityCheck(ctxt, 0, NULL, 0);
 } else {
 /*
	 * Internal checking in case the entity quest barfed
	 */
 if ((entity->etype != XML_INTERNAL_PARAMETER_ENTITY) &&
 (entity->etype != XML_EXTERNAL_PARAMETER_ENTITY)) {
	    xmlWarningMsg(ctxt, XML_WAR_UNDECLARED_ENTITY,
 "Internal: %%%s; is not a parameter entity\n",
			  name, NULL);
 } else if (ctxt->input->free != deallocblankswrapper) {
	    input = xmlNewBlanksWrapperInputStream(ctxt, entity);

 	    if (xmlPushInput(ctxt, input) < 0)
 		return;
 	} else {
 	    /*
 	     * TODO !!!
 	     * handle the extra spaces added before and after
	     * c.f. http://www.w3.org/TR/REC-xml#as-PE
	     */
	    input = xmlNewEntityInputStream(ctxt, entity);
 if (xmlPushInput(ctxt, input) < 0)
 return;
 if ((entity->etype == XML_EXTERNAL_PARAMETER_ENTITY) &&
 (CMP5(CUR_PTR, '<', '?', 'x', 'm', 'l')) &&
 (IS_BLANK_CH(NXT(5)))) {
		xmlParseTextDecl(ctxt);
 if (ctxt->errNo ==
		    XML_ERR_UNSUPPORTED_ENCODING) {
 /*
		     * The XML REC instructs us to stop parsing
		     * right here
		     */
		    xmlHaltParser(ctxt);
 return;
 }
 }
 }
 }
    ctxt->hasPErefs = 1;
}
