int rdfa_parse_start(rdfacontext* context)
{
   int rval = RDFA_PARSE_SUCCESS;

   context->wb_allocated = sizeof(char) * READ_BUFFER_SIZE;
   context->working_buffer = (char*)malloc(context->wb_allocated + 1);
   *context->working_buffer = '\0';

#ifndef LIBRDFA_IN_RAPTOR
   context->parser = XML_ParserCreate(NULL);
#endif
   context->done = 0;
   context->context_stack = rdfa_create_list(32);

   rdfa_push_item(context->context_stack, context, RDFALIST_FLAG_CONTEXT);

#ifdef LIBRDFA_IN_RAPTOR
   context->sax2 = raptor_new_sax2(context->world, context->locator,
                                   context->context_stack);
#else
#endif

#ifdef LIBRDFA_IN_RAPTOR
   raptor_sax2_set_start_element_handler(context->sax2,
                                         raptor_rdfa_start_element);
   raptor_sax2_set_end_element_handler(context->sax2,
                                       raptor_rdfa_end_element);
   raptor_sax2_set_characters_handler(context->sax2,
                                      raptor_rdfa_character_data);
   raptor_sax2_set_namespace_handler(context->sax2,
                                     raptor_rdfa_namespace_handler);
#else
   XML_SetUserData(context->parser, context->context_stack);
   XML_SetElementHandler(context->parser, start_element, end_element);
   XML_SetCharacterDataHandler(context->parser, character_data);
#endif

   rdfa_init_context(context);

#ifdef LIBRDFA_IN_RAPTOR
   if(1) {
     raptor_parser* rdf_parser = (raptor_parser*)context->callback_data;

     /* Optionally forbid internal network and file requests in the
      * XML parser
      */
     raptor_sax2_set_option(context->sax2,
                            RAPTOR_OPTION_NO_NET, NULL,
                            RAPTOR_OPTIONS_GET_NUMERIC(rdf_parser, RAPTOR_OPTION_NO_NET));
      raptor_sax2_set_option(context->sax2,
                             RAPTOR_OPTION_NO_FILE, NULL,
                             RAPTOR_OPTIONS_GET_NUMERIC(rdf_parser, RAPTOR_OPTION_NO_FILE));
      if(rdf_parser->uri_filter)
        raptor_sax2_set_uri_filter(context->sax2, rdf_parser->uri_filter,
                                   rdf_parser->uri_filter_user_data);
   }
   
   context->base_uri=raptor_new_uri(context->sax2->world, (const unsigned char*)context->base);
   raptor_sax2_parse_start(context->sax2, context->base_uri);
#endif

   return rval;
}
