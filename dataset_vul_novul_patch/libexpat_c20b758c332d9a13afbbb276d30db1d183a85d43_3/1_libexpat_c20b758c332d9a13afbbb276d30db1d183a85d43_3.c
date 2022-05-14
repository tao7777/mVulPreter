prologProcessor(XML_Parser parser, const char *s, const char *end,
                const char **nextPtr) {
   const char *next = s;
   int tok = XmlPrologTok(parser->m_encoding, s, end, &next);
   return doProlog(parser, parser->m_encoding, s, end, tok, next, nextPtr,
                  (XML_Bool)! parser->m_parsingStatus.finalBuffer);
 }
