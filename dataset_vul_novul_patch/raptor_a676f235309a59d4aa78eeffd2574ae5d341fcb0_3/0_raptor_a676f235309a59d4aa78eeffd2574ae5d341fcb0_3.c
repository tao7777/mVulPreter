raptor_rss_parse_start(raptor_parser *rdf_parser) 
{
  raptor_uri *uri = rdf_parser->base_uri;
  raptor_rss_parser* rss_parser = (raptor_rss_parser*)rdf_parser->context;
  int n;
  
  /* base URI required for RSS */
  if(!uri)
    return 1;

  for(n = 0; n < RAPTOR_RSS_NAMESPACES_SIZE; n++)
    rss_parser->nspaces_seen[n] = 'N';

  /* Optionally forbid internal network and file requests in the XML parser */
  raptor_sax2_set_option(rss_parser->sax2, 
                         RAPTOR_OPTION_NO_NET, NULL,
                         RAPTOR_OPTIONS_GET_NUMERIC(rdf_parser, RAPTOR_OPTION_NO_NET));
   raptor_sax2_set_option(rss_parser->sax2, 
                          RAPTOR_OPTION_NO_FILE, NULL,
                          RAPTOR_OPTIONS_GET_NUMERIC(rdf_parser, RAPTOR_OPTION_NO_FILE));
  raptor_sax2_set_option(rss_parser->sax2, 
                         RAPTOR_OPTION_LOAD_EXTERNAL_ENTITIES, NULL,
                         RAPTOR_OPTIONS_GET_NUMERIC(rdf_parser, RAPTOR_OPTION_LOAD_EXTERNAL_ENTITIES));
   if(rdf_parser->uri_filter)
     raptor_sax2_set_uri_filter(rss_parser->sax2, rdf_parser->uri_filter,
                                rdf_parser->uri_filter_user_data);
  
  raptor_sax2_parse_start(rss_parser->sax2, uri);

  return 0;
}
