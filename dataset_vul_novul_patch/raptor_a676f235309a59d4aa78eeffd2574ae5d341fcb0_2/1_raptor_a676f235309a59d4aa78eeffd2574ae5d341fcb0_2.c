raptor_rdfxml_parse_start(raptor_parser* rdf_parser)
{
  raptor_uri *uri = rdf_parser->base_uri;
  raptor_rdfxml_parser* rdf_xml_parser;

  rdf_xml_parser = (raptor_rdfxml_parser*)rdf_parser->context;

  /* base URI required for RDF/XML */
  if(!uri)
    return 1;

  /* Optionally normalize language to lowercase
   * http://www.w3.org/TR/rdf-concepts/#dfn-language-identifier
   */
  raptor_sax2_set_option(rdf_xml_parser->sax2,
                         RAPTOR_OPTION_NORMALIZE_LANGUAGE, NULL,
                         RAPTOR_OPTIONS_GET_NUMERIC(rdf_parser, RAPTOR_OPTION_NORMALIZE_LANGUAGE));

  /* Optionally forbid internal network and file requests in the XML parser */
  raptor_sax2_set_option(rdf_xml_parser->sax2, 
                         RAPTOR_OPTION_NO_NET, NULL,
                         RAPTOR_OPTIONS_GET_NUMERIC(rdf_parser, RAPTOR_OPTION_NO_NET));
   raptor_sax2_set_option(rdf_xml_parser->sax2, 
                          RAPTOR_OPTION_NO_FILE, NULL,
                          RAPTOR_OPTIONS_GET_NUMERIC(rdf_parser, RAPTOR_OPTION_NO_FILE));
   if(rdf_parser->uri_filter)
     raptor_sax2_set_uri_filter(rdf_xml_parser->sax2, rdf_parser->uri_filter,
                                rdf_parser->uri_filter_user_data);

  raptor_sax2_parse_start(rdf_xml_parser->sax2, uri);

  /* Delete any existing id_set */
  if(rdf_xml_parser->id_set) {
    raptor_free_id_set(rdf_xml_parser->id_set);
    rdf_xml_parser->id_set = NULL;
  }
  
  /* Create a new id_set if needed */
  if(RAPTOR_OPTIONS_GET_NUMERIC(rdf_parser, RAPTOR_OPTION_CHECK_RDF_ID)) {
    rdf_xml_parser->id_set = raptor_new_id_set(rdf_parser->world);
    if(!rdf_xml_parser->id_set)
      return 1;
  }
  
  return 0;
}
