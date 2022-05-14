processInternalEntity(XML_Parser parser, ENTITY *entity, XML_Bool betweenDecl) {
  const char *textStart, *textEnd;
  const char *next;
  enum XML_Error result;
  OPEN_INTERNAL_ENTITY *openEntity;

  if (parser->m_freeInternalEntities) {
    openEntity = parser->m_freeInternalEntities;
    parser->m_freeInternalEntities = openEntity->next;
  } else {
    openEntity
        = (OPEN_INTERNAL_ENTITY *)MALLOC(parser, sizeof(OPEN_INTERNAL_ENTITY));
    if (! openEntity)
      return XML_ERROR_NO_MEMORY;
  }
  entity->open = XML_TRUE;
  entity->processed = 0;
  openEntity->next = parser->m_openInternalEntities;
  parser->m_openInternalEntities = openEntity;
  openEntity->entity = entity;
  openEntity->startTagLevel = parser->m_tagLevel;
  openEntity->betweenDecl = betweenDecl;
  openEntity->internalEventPtr = NULL;
  openEntity->internalEventEndPtr = NULL;
  textStart = (char *)entity->textPtr;
  textEnd = (char *)(entity->textPtr + entity->textLen);
  /* Set a safe default value in case 'next' does not get set */
  next = textStart;

#ifdef XML_DTD
  if (entity->is_param) {
     int tok
         = XmlPrologTok(parser->m_internalEncoding, textStart, textEnd, &next);
     result = doProlog(parser, parser->m_internalEncoding, textStart, textEnd,
                      tok, next, &next, XML_FALSE, XML_FALSE);
   } else
 #endif /* XML_DTD */
     result = doContent(parser, parser->m_tagLevel, parser->m_internalEncoding,
                       textStart, textEnd, &next, XML_FALSE);

  if (result == XML_ERROR_NONE) {
    if (textEnd != next && parser->m_parsingStatus.parsing == XML_SUSPENDED) {
      entity->processed = (int)(next - textStart);
      parser->m_processor = internalEntityProcessor;
    } else {
      entity->open = XML_FALSE;
      parser->m_openInternalEntities = openEntity->next;
      /* put openEntity back in list of free instances */
      openEntity->next = parser->m_freeInternalEntities;
      parser->m_freeInternalEntities = openEntity;
    }
  }
  return result;
}
