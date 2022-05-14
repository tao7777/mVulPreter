raptor_libxml_getEntity(void* user_data, const xmlChar *name) {
raptor_libxml_getEntity(void* user_data, const xmlChar *name)
{
   raptor_sax2* sax2 = (raptor_sax2*)user_data;
  xmlParserCtxtPtr xc = sax2->xc;
  xmlEntityPtr ret = NULL;

  if(!xc)
    return NULL;

  if(!xc->inSubset) {
    /* looks for hardcoded set of entity names - lt, gt etc. */
    ret = xmlGetPredefinedEntity(name);
    if(ret) {
      RAPTOR_DEBUG2("Entity '%s' found in predefined set\n", name);
      return ret;
    }
  }

  /* This section uses xmlGetDocEntity which looks for entities in
   * memory only, never from a file or URI 
   */
  if(xc->myDoc && (xc->myDoc->standalone == 1)) {
    RAPTOR_DEBUG2("Entity '%s' document is standalone\n", name);
    /* Document is standalone: no entities are required to interpret doc */
    if(xc->inSubset == 2) {
      xc->myDoc->standalone = 0;
      ret = xmlGetDocEntity(xc->myDoc, name);
      xc->myDoc->standalone = 1;
    } else {
      ret = xmlGetDocEntity(xc->myDoc, name);
      if(!ret) {
        xc->myDoc->standalone = 0;
        ret = xmlGetDocEntity(xc->myDoc, name);
        xc->myDoc->standalone = 1;
      }
    }
  } else {
    ret = xmlGetDocEntity(xc->myDoc, name);
  }

  if(ret && !ret->children &&
    (ret->etype == XML_EXTERNAL_GENERAL_PARSED_ENTITY)) {
    /* Entity is an external general parsed entity. It may be in a
     * catalog file, user file or user URI
     */
    int val = 0;
    xmlNodePtr children;
    int load_entity = 0;

    load_entity = RAPTOR_OPTIONS_GET_NUMERIC(sax2, RAPTOR_OPTION_LOAD_EXTERNAL_ENTITIES);
    if(load_entity)
      load_entity = raptor_sax2_check_load_uri_string(sax2, ret->URI);

    if(!load_entity) {
      RAPTOR_DEBUG2("Not getting entity URI %s by policy\n", ret->URI);
      children = xmlNewText((const xmlChar*)"");
    } else {
      /* Disable SAX2 handlers so that the SAX2 events do not all get
       * sent to callbacks during dealing with the entity parsing.
       */
      sax2->enabled = 0;
      val = xmlParseCtxtExternalEntity(xc, ret->URI, ret->ExternalID, &children);
      sax2->enabled = 1;
    }
    
    if(!val) {
      xmlAddChildList((xmlNodePtr)ret, children);
    } else {
      xc->validate = 0;
      return NULL;
    }
    
    ret->owner = 1;

    /* Mark this entity as having been checked - never do this again */
    if(!ret->checked)
      ret->checked = 1;
  }

  return ret;
 }
