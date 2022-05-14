raptor_turtle_writer_set_option(raptor_turtle_writer *turtle_writer, 
                                 raptor_option option, int value)
{
  if(value < 0 ||
     !raptor_option_is_valid_for_area(option, RAPTOR_OPTION_AREA_TURTLE_WRITER))
    return 1;
  
  switch(option) {
    case RAPTOR_OPTION_WRITER_AUTO_INDENT:
      if(value)
        turtle_writer->flags |= TURTLE_WRITER_AUTO_INDENT;
      else
        turtle_writer->flags &= ~TURTLE_WRITER_AUTO_INDENT;        
      break;

    case RAPTOR_OPTION_WRITER_INDENT_WIDTH:
      turtle_writer->indent = value;
      break;
    
    case RAPTOR_OPTION_WRITER_AUTO_EMPTY:
    case RAPTOR_OPTION_WRITER_XML_VERSION:
    case RAPTOR_OPTION_WRITER_XML_DECLARATION:
      break;
        
    /* parser options */
    case RAPTOR_OPTION_SCANNING:
    case RAPTOR_OPTION_ALLOW_NON_NS_ATTRIBUTES:
    case RAPTOR_OPTION_ALLOW_OTHER_PARSETYPES:
    case RAPTOR_OPTION_ALLOW_BAGID:
    case RAPTOR_OPTION_ALLOW_RDF_TYPE_RDF_LIST:
    case RAPTOR_OPTION_NORMALIZE_LANGUAGE:
    case RAPTOR_OPTION_NON_NFC_FATAL:
    case RAPTOR_OPTION_WARN_OTHER_PARSETYPES:
    case RAPTOR_OPTION_CHECK_RDF_ID:
    case RAPTOR_OPTION_HTML_TAG_SOUP:
    case RAPTOR_OPTION_MICROFORMATS:
    case RAPTOR_OPTION_HTML_LINK:
    case RAPTOR_OPTION_WWW_TIMEOUT:
    case RAPTOR_OPTION_STRICT:
      
     /* Shared */
     case RAPTOR_OPTION_NO_NET:
     case RAPTOR_OPTION_NO_FILE:
    case RAPTOR_OPTION_LOAD_EXTERNAL_ENTITIES:
 
     /* XML writer options */
     case RAPTOR_OPTION_RELATIVE_URIS:

    /* DOT serializer options */
    case RAPTOR_OPTION_RESOURCE_BORDER:
    case RAPTOR_OPTION_LITERAL_BORDER:
    case RAPTOR_OPTION_BNODE_BORDER:
    case RAPTOR_OPTION_RESOURCE_FILL:
    case RAPTOR_OPTION_LITERAL_FILL:
    case RAPTOR_OPTION_BNODE_FILL:

    /* JSON serializer options */
    case RAPTOR_OPTION_JSON_CALLBACK:
    case RAPTOR_OPTION_JSON_EXTRA_DATA:
    case RAPTOR_OPTION_RSS_TRIPLES:
    case RAPTOR_OPTION_ATOM_ENTRY_URI:
    case RAPTOR_OPTION_PREFIX_ELEMENTS:
    
    /* Turtle serializer option */
    case RAPTOR_OPTION_WRITE_BASE_URI:

    /* WWW option */
    case RAPTOR_OPTION_WWW_HTTP_CACHE_CONTROL:
    case RAPTOR_OPTION_WWW_HTTP_USER_AGENT:
    case RAPTOR_OPTION_WWW_CERT_FILENAME:
    case RAPTOR_OPTION_WWW_CERT_TYPE:
    case RAPTOR_OPTION_WWW_CERT_PASSPHRASE:
    case RAPTOR_OPTION_WWW_SSL_VERIFY_PEER:
    case RAPTOR_OPTION_WWW_SSL_VERIFY_HOST:
      
    default:
      return -1;
      break;
  }

  return 0;
}
