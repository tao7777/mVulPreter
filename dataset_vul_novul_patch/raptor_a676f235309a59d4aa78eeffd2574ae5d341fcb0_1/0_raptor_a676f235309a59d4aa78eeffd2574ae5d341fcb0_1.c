 raptor_libxml_resolveEntity(void* user_data, 
                            const xmlChar *publicId, const xmlChar *systemId)
{
   raptor_sax2* sax2 = (raptor_sax2*)user_data;
  xmlParserCtxtPtr ctxt = sax2->xc;
  const unsigned char *uri_string = NULL;
  xmlParserInputPtr entity_input;
  int load_entity = 0;

  if(ctxt->input)
    uri_string = RAPTOR_GOOD_CAST(const unsigned char *, ctxt->input->filename);

  if(!uri_string)
    uri_string = RAPTOR_GOOD_CAST(const unsigned char *, ctxt->directory);

  load_entity = RAPTOR_OPTIONS_GET_NUMERIC(sax2, RAPTOR_OPTION_LOAD_EXTERNAL_ENTITIES);
  if(load_entity)
    load_entity = raptor_sax2_check_load_uri_string(sax2, uri_string);

  if(load_entity) {
    entity_input = xmlLoadExternalEntity(RAPTOR_GOOD_CAST(const char*, uri_string),
                                         RAPTOR_GOOD_CAST(const char*, publicId),
                                         ctxt);
  } else {
    RAPTOR_DEBUG4("Not loading entity URI %s by policy for publicId '%s' systemId '%s'\n", uri_string, publicId, systemId);
  }
  
  return entity_input;
 }
