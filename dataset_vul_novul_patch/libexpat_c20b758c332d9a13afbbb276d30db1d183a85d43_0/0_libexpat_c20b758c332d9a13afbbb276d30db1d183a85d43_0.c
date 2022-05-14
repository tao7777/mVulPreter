externalParEntProcessor(XML_Parser parser, const char *s, const char *end,
                        const char **nextPtr) {
  const char *next = s;
  int tok;

  tok = XmlPrologTok(parser->m_encoding, s, end, &next);
  if (tok <= 0) {
    if (! parser->m_parsingStatus.finalBuffer && tok != XML_TOK_INVALID) {
      *nextPtr = s;
      return XML_ERROR_NONE;
    }
    switch (tok) {
    case XML_TOK_INVALID:
      return XML_ERROR_INVALID_TOKEN;
    case XML_TOK_PARTIAL:
      return XML_ERROR_UNCLOSED_TOKEN;
    case XML_TOK_PARTIAL_CHAR:
      return XML_ERROR_PARTIAL_CHAR;
    case XML_TOK_NONE: /* start == end */
    default:
      break;
    }
  }
  /* This would cause the next stage, i.e. doProlog to be passed XML_TOK_BOM.
     However, when parsing an external subset, doProlog will not accept a BOM
     as valid, and report a syntax error, so we have to skip the BOM
  */
  else if (tok == XML_TOK_BOM) {
    s = next;
    tok = XmlPrologTok(parser->m_encoding, s, end, &next);
  }
 
   parser->m_processor = prologProcessor;
   return doProlog(parser, parser->m_encoding, s, end, tok, next, nextPtr,
                  (XML_Bool)! parser->m_parsingStatus.finalBuffer, XML_TRUE);
 }
