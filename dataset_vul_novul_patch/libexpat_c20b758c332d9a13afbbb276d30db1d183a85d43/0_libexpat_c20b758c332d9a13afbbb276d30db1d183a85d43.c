 doProlog(XML_Parser parser, const ENCODING *enc, const char *s, const char *end,
         int tok, const char *next, const char **nextPtr, XML_Bool haveMore,
         XML_Bool allowClosingDoctype) {
 #ifdef XML_DTD
   static const XML_Char externalSubsetName[] = {ASCII_HASH, '\0'};
 #endif /* XML_DTD */
  static const XML_Char atypeCDATA[]
      = {ASCII_C, ASCII_D, ASCII_A, ASCII_T, ASCII_A, '\0'};
  static const XML_Char atypeID[] = {ASCII_I, ASCII_D, '\0'};
  static const XML_Char atypeIDREF[]
      = {ASCII_I, ASCII_D, ASCII_R, ASCII_E, ASCII_F, '\0'};
  static const XML_Char atypeIDREFS[]
      = {ASCII_I, ASCII_D, ASCII_R, ASCII_E, ASCII_F, ASCII_S, '\0'};
  static const XML_Char atypeENTITY[]
      = {ASCII_E, ASCII_N, ASCII_T, ASCII_I, ASCII_T, ASCII_Y, '\0'};
  static const XML_Char atypeENTITIES[]
      = {ASCII_E, ASCII_N, ASCII_T, ASCII_I, ASCII_T,
         ASCII_I, ASCII_E, ASCII_S, '\0'};
  static const XML_Char atypeNMTOKEN[]
      = {ASCII_N, ASCII_M, ASCII_T, ASCII_O, ASCII_K, ASCII_E, ASCII_N, '\0'};
  static const XML_Char atypeNMTOKENS[]
      = {ASCII_N, ASCII_M, ASCII_T, ASCII_O, ASCII_K,
         ASCII_E, ASCII_N, ASCII_S, '\0'};
  static const XML_Char notationPrefix[]
      = {ASCII_N, ASCII_O, ASCII_T, ASCII_A,      ASCII_T,
         ASCII_I, ASCII_O, ASCII_N, ASCII_LPAREN, '\0'};
  static const XML_Char enumValueSep[] = {ASCII_PIPE, '\0'};
  static const XML_Char enumValueStart[] = {ASCII_LPAREN, '\0'};

  /* save one level of indirection */
  DTD *const dtd = parser->m_dtd;

  const char **eventPP;
  const char **eventEndPP;
  enum XML_Content_Quant quant;

  if (enc == parser->m_encoding) {
    eventPP = &parser->m_eventPtr;
    eventEndPP = &parser->m_eventEndPtr;
  } else {
    eventPP = &(parser->m_openInternalEntities->internalEventPtr);
    eventEndPP = &(parser->m_openInternalEntities->internalEventEndPtr);
  }

  for (;;) {
    int role;
    XML_Bool handleDefault = XML_TRUE;
    *eventPP = s;
    *eventEndPP = next;
    if (tok <= 0) {
      if (haveMore && tok != XML_TOK_INVALID) {
        *nextPtr = s;
        return XML_ERROR_NONE;
      }
      switch (tok) {
      case XML_TOK_INVALID:
        *eventPP = next;
        return XML_ERROR_INVALID_TOKEN;
      case XML_TOK_PARTIAL:
        return XML_ERROR_UNCLOSED_TOKEN;
      case XML_TOK_PARTIAL_CHAR:
        return XML_ERROR_PARTIAL_CHAR;
      case -XML_TOK_PROLOG_S:
        tok = -tok;
        break;
      case XML_TOK_NONE:
#ifdef XML_DTD
        /* for internal PE NOT referenced between declarations */
        if (enc != parser->m_encoding
            && ! parser->m_openInternalEntities->betweenDecl) {
          *nextPtr = s;
          return XML_ERROR_NONE;
        }
        /* WFC: PE Between Declarations - must check that PE contains
           complete markup, not only for external PEs, but also for
           internal PEs if the reference occurs between declarations.
        */
        if (parser->m_isParamEntity || enc != parser->m_encoding) {
          if (XmlTokenRole(&parser->m_prologState, XML_TOK_NONE, end, end, enc)
              == XML_ROLE_ERROR)
            return XML_ERROR_INCOMPLETE_PE;
          *nextPtr = s;
          return XML_ERROR_NONE;
        }
#endif /* XML_DTD */
        return XML_ERROR_NO_ELEMENTS;
      default:
        tok = -tok;
        next = end;
        break;
      }
    }
    role = XmlTokenRole(&parser->m_prologState, tok, s, next, enc);
    switch (role) {
    case XML_ROLE_XML_DECL: {
      enum XML_Error result = processXmlDecl(parser, 0, s, next);
      if (result != XML_ERROR_NONE)
        return result;
      enc = parser->m_encoding;
      handleDefault = XML_FALSE;
    } break;
    case XML_ROLE_DOCTYPE_NAME:
      if (parser->m_startDoctypeDeclHandler) {
        parser->m_doctypeName
            = poolStoreString(&parser->m_tempPool, enc, s, next);
        if (! parser->m_doctypeName)
          return XML_ERROR_NO_MEMORY;
        poolFinish(&parser->m_tempPool);
        parser->m_doctypePubid = NULL;
        handleDefault = XML_FALSE;
      }
      parser->m_doctypeSysid = NULL; /* always initialize to NULL */
      break;
    case XML_ROLE_DOCTYPE_INTERNAL_SUBSET:
      if (parser->m_startDoctypeDeclHandler) {
        parser->m_startDoctypeDeclHandler(
            parser->m_handlerArg, parser->m_doctypeName, parser->m_doctypeSysid,
            parser->m_doctypePubid, 1);
        parser->m_doctypeName = NULL;
        poolClear(&parser->m_tempPool);
        handleDefault = XML_FALSE;
      }
      break;
#ifdef XML_DTD
    case XML_ROLE_TEXT_DECL: {
      enum XML_Error result = processXmlDecl(parser, 1, s, next);
      if (result != XML_ERROR_NONE)
        return result;
      enc = parser->m_encoding;
      handleDefault = XML_FALSE;
    } break;
#endif /* XML_DTD */
    case XML_ROLE_DOCTYPE_PUBLIC_ID:
#ifdef XML_DTD
      parser->m_useForeignDTD = XML_FALSE;
      parser->m_declEntity = (ENTITY *)lookup(
          parser, &dtd->paramEntities, externalSubsetName, sizeof(ENTITY));
      if (! parser->m_declEntity)
        return XML_ERROR_NO_MEMORY;
#endif /* XML_DTD */
      dtd->hasParamEntityRefs = XML_TRUE;
      if (parser->m_startDoctypeDeclHandler) {
        XML_Char *pubId;
        if (! XmlIsPublicId(enc, s, next, eventPP))
          return XML_ERROR_PUBLICID;
        pubId = poolStoreString(&parser->m_tempPool, enc,
                                s + enc->minBytesPerChar,
                                next - enc->minBytesPerChar);
        if (! pubId)
          return XML_ERROR_NO_MEMORY;
        normalizePublicId(pubId);
        poolFinish(&parser->m_tempPool);
        parser->m_doctypePubid = pubId;
        handleDefault = XML_FALSE;
        goto alreadyChecked;
      }
      /* fall through */
    case XML_ROLE_ENTITY_PUBLIC_ID:
      if (! XmlIsPublicId(enc, s, next, eventPP))
        return XML_ERROR_PUBLICID;
    alreadyChecked:
      if (dtd->keepProcessing && parser->m_declEntity) {
        XML_Char *tem
            = poolStoreString(&dtd->pool, enc, s + enc->minBytesPerChar,
                              next - enc->minBytesPerChar);
        if (! tem)
          return XML_ERROR_NO_MEMORY;
        normalizePublicId(tem);
        parser->m_declEntity->publicId = tem;
        poolFinish(&dtd->pool);
        /* Don't suppress the default handler if we fell through from
         * the XML_ROLE_DOCTYPE_PUBLIC_ID case.
         */
        if (parser->m_entityDeclHandler && role == XML_ROLE_ENTITY_PUBLIC_ID)
          handleDefault = XML_FALSE;
       }
       break;
     case XML_ROLE_DOCTYPE_CLOSE:
      if (allowClosingDoctype != XML_TRUE) {
        /* Must not close doctype from within expanded parameter entities */
        return XML_ERROR_INVALID_TOKEN;
      }

       if (parser->m_doctypeName) {
         parser->m_startDoctypeDeclHandler(
             parser->m_handlerArg, parser->m_doctypeName, parser->m_doctypeSysid,
            parser->m_doctypePubid, 0);
        poolClear(&parser->m_tempPool);
        handleDefault = XML_FALSE;
      }
      /* parser->m_doctypeSysid will be non-NULL in the case of a previous
         XML_ROLE_DOCTYPE_SYSTEM_ID, even if parser->m_startDoctypeDeclHandler
         was not set, indicating an external subset
      */
#ifdef XML_DTD
      if (parser->m_doctypeSysid || parser->m_useForeignDTD) {
        XML_Bool hadParamEntityRefs = dtd->hasParamEntityRefs;
        dtd->hasParamEntityRefs = XML_TRUE;
        if (parser->m_paramEntityParsing
            && parser->m_externalEntityRefHandler) {
          ENTITY *entity = (ENTITY *)lookup(parser, &dtd->paramEntities,
                                            externalSubsetName, sizeof(ENTITY));
          if (! entity) {
            /* The external subset name "#" will have already been
             * inserted into the hash table at the start of the
             * external entity parsing, so no allocation will happen
             * and lookup() cannot fail.
             */
            return XML_ERROR_NO_MEMORY; /* LCOV_EXCL_LINE */
          }
          if (parser->m_useForeignDTD)
            entity->base = parser->m_curBase;
          dtd->paramEntityRead = XML_FALSE;
          if (! parser->m_externalEntityRefHandler(
                  parser->m_externalEntityRefHandlerArg, 0, entity->base,
                  entity->systemId, entity->publicId))
            return XML_ERROR_EXTERNAL_ENTITY_HANDLING;
          if (dtd->paramEntityRead) {
            if (! dtd->standalone && parser->m_notStandaloneHandler
                && ! parser->m_notStandaloneHandler(parser->m_handlerArg))
              return XML_ERROR_NOT_STANDALONE;
          }
          /* if we didn't read the foreign DTD then this means that there
             is no external subset and we must reset dtd->hasParamEntityRefs
          */
          else if (! parser->m_doctypeSysid)
            dtd->hasParamEntityRefs = hadParamEntityRefs;
          /* end of DTD - no need to update dtd->keepProcessing */
        }
        parser->m_useForeignDTD = XML_FALSE;
      }
#endif /* XML_DTD */
      if (parser->m_endDoctypeDeclHandler) {
        parser->m_endDoctypeDeclHandler(parser->m_handlerArg);
        handleDefault = XML_FALSE;
      }
      break;
    case XML_ROLE_INSTANCE_START:
#ifdef XML_DTD
      /* if there is no DOCTYPE declaration then now is the
         last chance to read the foreign DTD
      */
      if (parser->m_useForeignDTD) {
        XML_Bool hadParamEntityRefs = dtd->hasParamEntityRefs;
        dtd->hasParamEntityRefs = XML_TRUE;
        if (parser->m_paramEntityParsing
            && parser->m_externalEntityRefHandler) {
          ENTITY *entity = (ENTITY *)lookup(parser, &dtd->paramEntities,
                                            externalSubsetName, sizeof(ENTITY));
          if (! entity)
            return XML_ERROR_NO_MEMORY;
          entity->base = parser->m_curBase;
          dtd->paramEntityRead = XML_FALSE;
          if (! parser->m_externalEntityRefHandler(
                  parser->m_externalEntityRefHandlerArg, 0, entity->base,
                  entity->systemId, entity->publicId))
            return XML_ERROR_EXTERNAL_ENTITY_HANDLING;
          if (dtd->paramEntityRead) {
            if (! dtd->standalone && parser->m_notStandaloneHandler
                && ! parser->m_notStandaloneHandler(parser->m_handlerArg))
              return XML_ERROR_NOT_STANDALONE;
          }
          /* if we didn't read the foreign DTD then this means that there
             is no external subset and we must reset dtd->hasParamEntityRefs
          */
          else
            dtd->hasParamEntityRefs = hadParamEntityRefs;
          /* end of DTD - no need to update dtd->keepProcessing */
        }
      }
#endif /* XML_DTD */
      parser->m_processor = contentProcessor;
      return contentProcessor(parser, s, end, nextPtr);
    case XML_ROLE_ATTLIST_ELEMENT_NAME:
      parser->m_declElementType = getElementType(parser, enc, s, next);
      if (! parser->m_declElementType)
        return XML_ERROR_NO_MEMORY;
      goto checkAttListDeclHandler;
    case XML_ROLE_ATTRIBUTE_NAME:
      parser->m_declAttributeId = getAttributeId(parser, enc, s, next);
      if (! parser->m_declAttributeId)
        return XML_ERROR_NO_MEMORY;
      parser->m_declAttributeIsCdata = XML_FALSE;
      parser->m_declAttributeType = NULL;
      parser->m_declAttributeIsId = XML_FALSE;
      goto checkAttListDeclHandler;
    case XML_ROLE_ATTRIBUTE_TYPE_CDATA:
      parser->m_declAttributeIsCdata = XML_TRUE;
      parser->m_declAttributeType = atypeCDATA;
      goto checkAttListDeclHandler;
    case XML_ROLE_ATTRIBUTE_TYPE_ID:
      parser->m_declAttributeIsId = XML_TRUE;
      parser->m_declAttributeType = atypeID;
      goto checkAttListDeclHandler;
    case XML_ROLE_ATTRIBUTE_TYPE_IDREF:
      parser->m_declAttributeType = atypeIDREF;
      goto checkAttListDeclHandler;
    case XML_ROLE_ATTRIBUTE_TYPE_IDREFS:
      parser->m_declAttributeType = atypeIDREFS;
      goto checkAttListDeclHandler;
    case XML_ROLE_ATTRIBUTE_TYPE_ENTITY:
      parser->m_declAttributeType = atypeENTITY;
      goto checkAttListDeclHandler;
    case XML_ROLE_ATTRIBUTE_TYPE_ENTITIES:
      parser->m_declAttributeType = atypeENTITIES;
      goto checkAttListDeclHandler;
    case XML_ROLE_ATTRIBUTE_TYPE_NMTOKEN:
      parser->m_declAttributeType = atypeNMTOKEN;
      goto checkAttListDeclHandler;
    case XML_ROLE_ATTRIBUTE_TYPE_NMTOKENS:
      parser->m_declAttributeType = atypeNMTOKENS;
    checkAttListDeclHandler:
      if (dtd->keepProcessing && parser->m_attlistDeclHandler)
        handleDefault = XML_FALSE;
      break;
    case XML_ROLE_ATTRIBUTE_ENUM_VALUE:
    case XML_ROLE_ATTRIBUTE_NOTATION_VALUE:
      if (dtd->keepProcessing && parser->m_attlistDeclHandler) {
        const XML_Char *prefix;
        if (parser->m_declAttributeType) {
          prefix = enumValueSep;
        } else {
          prefix = (role == XML_ROLE_ATTRIBUTE_NOTATION_VALUE ? notationPrefix
                                                              : enumValueStart);
        }
        if (! poolAppendString(&parser->m_tempPool, prefix))
          return XML_ERROR_NO_MEMORY;
        if (! poolAppend(&parser->m_tempPool, enc, s, next))
          return XML_ERROR_NO_MEMORY;
        parser->m_declAttributeType = parser->m_tempPool.start;
        handleDefault = XML_FALSE;
      }
      break;
    case XML_ROLE_IMPLIED_ATTRIBUTE_VALUE:
    case XML_ROLE_REQUIRED_ATTRIBUTE_VALUE:
      if (dtd->keepProcessing) {
        if (! defineAttribute(parser->m_declElementType,
                              parser->m_declAttributeId,
                              parser->m_declAttributeIsCdata,
                              parser->m_declAttributeIsId, 0, parser))
          return XML_ERROR_NO_MEMORY;
        if (parser->m_attlistDeclHandler && parser->m_declAttributeType) {
          if (*parser->m_declAttributeType == XML_T(ASCII_LPAREN)
              || (*parser->m_declAttributeType == XML_T(ASCII_N)
                  && parser->m_declAttributeType[1] == XML_T(ASCII_O))) {
            /* Enumerated or Notation type */
            if (! poolAppendChar(&parser->m_tempPool, XML_T(ASCII_RPAREN))
                || ! poolAppendChar(&parser->m_tempPool, XML_T('\0')))
              return XML_ERROR_NO_MEMORY;
            parser->m_declAttributeType = parser->m_tempPool.start;
            poolFinish(&parser->m_tempPool);
          }
          *eventEndPP = s;
          parser->m_attlistDeclHandler(
              parser->m_handlerArg, parser->m_declElementType->name,
              parser->m_declAttributeId->name, parser->m_declAttributeType, 0,
              role == XML_ROLE_REQUIRED_ATTRIBUTE_VALUE);
          poolClear(&parser->m_tempPool);
          handleDefault = XML_FALSE;
        }
      }
      break;
    case XML_ROLE_DEFAULT_ATTRIBUTE_VALUE:
    case XML_ROLE_FIXED_ATTRIBUTE_VALUE:
      if (dtd->keepProcessing) {
        const XML_Char *attVal;
        enum XML_Error result = storeAttributeValue(
            parser, enc, parser->m_declAttributeIsCdata,
            s + enc->minBytesPerChar, next - enc->minBytesPerChar, &dtd->pool);
        if (result)
          return result;
        attVal = poolStart(&dtd->pool);
        poolFinish(&dtd->pool);
        /* ID attributes aren't allowed to have a default */
        if (! defineAttribute(
                parser->m_declElementType, parser->m_declAttributeId,
                parser->m_declAttributeIsCdata, XML_FALSE, attVal, parser))
          return XML_ERROR_NO_MEMORY;
        if (parser->m_attlistDeclHandler && parser->m_declAttributeType) {
          if (*parser->m_declAttributeType == XML_T(ASCII_LPAREN)
              || (*parser->m_declAttributeType == XML_T(ASCII_N)
                  && parser->m_declAttributeType[1] == XML_T(ASCII_O))) {
            /* Enumerated or Notation type */
            if (! poolAppendChar(&parser->m_tempPool, XML_T(ASCII_RPAREN))
                || ! poolAppendChar(&parser->m_tempPool, XML_T('\0')))
              return XML_ERROR_NO_MEMORY;
            parser->m_declAttributeType = parser->m_tempPool.start;
            poolFinish(&parser->m_tempPool);
          }
          *eventEndPP = s;
          parser->m_attlistDeclHandler(
              parser->m_handlerArg, parser->m_declElementType->name,
              parser->m_declAttributeId->name, parser->m_declAttributeType,
              attVal, role == XML_ROLE_FIXED_ATTRIBUTE_VALUE);
          poolClear(&parser->m_tempPool);
          handleDefault = XML_FALSE;
        }
      }
      break;
    case XML_ROLE_ENTITY_VALUE:
      if (dtd->keepProcessing) {
        enum XML_Error result = storeEntityValue(
            parser, enc, s + enc->minBytesPerChar, next - enc->minBytesPerChar);
        if (parser->m_declEntity) {
          parser->m_declEntity->textPtr = poolStart(&dtd->entityValuePool);
          parser->m_declEntity->textLen
              = (int)(poolLength(&dtd->entityValuePool));
          poolFinish(&dtd->entityValuePool);
          if (parser->m_entityDeclHandler) {
            *eventEndPP = s;
            parser->m_entityDeclHandler(
                parser->m_handlerArg, parser->m_declEntity->name,
                parser->m_declEntity->is_param, parser->m_declEntity->textPtr,
                parser->m_declEntity->textLen, parser->m_curBase, 0, 0, 0);
            handleDefault = XML_FALSE;
          }
        } else
          poolDiscard(&dtd->entityValuePool);
        if (result != XML_ERROR_NONE)
          return result;
      }
      break;
    case XML_ROLE_DOCTYPE_SYSTEM_ID:
#ifdef XML_DTD
      parser->m_useForeignDTD = XML_FALSE;
#endif /* XML_DTD */
      dtd->hasParamEntityRefs = XML_TRUE;
      if (parser->m_startDoctypeDeclHandler) {
        parser->m_doctypeSysid = poolStoreString(&parser->m_tempPool, enc,
                                                 s + enc->minBytesPerChar,
                                                 next - enc->minBytesPerChar);
        if (parser->m_doctypeSysid == NULL)
          return XML_ERROR_NO_MEMORY;
        poolFinish(&parser->m_tempPool);
        handleDefault = XML_FALSE;
      }
#ifdef XML_DTD
      else
        /* use externalSubsetName to make parser->m_doctypeSysid non-NULL
           for the case where no parser->m_startDoctypeDeclHandler is set */
        parser->m_doctypeSysid = externalSubsetName;
#endif /* XML_DTD */
      if (! dtd->standalone
#ifdef XML_DTD
          && ! parser->m_paramEntityParsing
#endif /* XML_DTD */
          && parser->m_notStandaloneHandler
          && ! parser->m_notStandaloneHandler(parser->m_handlerArg))
        return XML_ERROR_NOT_STANDALONE;
#ifndef XML_DTD
      break;
#else  /* XML_DTD */
      if (! parser->m_declEntity) {
        parser->m_declEntity = (ENTITY *)lookup(
            parser, &dtd->paramEntities, externalSubsetName, sizeof(ENTITY));
        if (! parser->m_declEntity)
          return XML_ERROR_NO_MEMORY;
        parser->m_declEntity->publicId = NULL;
      }
#endif /* XML_DTD */
      /* fall through */
    case XML_ROLE_ENTITY_SYSTEM_ID:
      if (dtd->keepProcessing && parser->m_declEntity) {
        parser->m_declEntity->systemId
            = poolStoreString(&dtd->pool, enc, s + enc->minBytesPerChar,
                              next - enc->minBytesPerChar);
        if (! parser->m_declEntity->systemId)
          return XML_ERROR_NO_MEMORY;
        parser->m_declEntity->base = parser->m_curBase;
        poolFinish(&dtd->pool);
        /* Don't suppress the default handler if we fell through from
         * the XML_ROLE_DOCTYPE_SYSTEM_ID case.
         */
        if (parser->m_entityDeclHandler && role == XML_ROLE_ENTITY_SYSTEM_ID)
          handleDefault = XML_FALSE;
      }
      break;
    case XML_ROLE_ENTITY_COMPLETE:
      if (dtd->keepProcessing && parser->m_declEntity
          && parser->m_entityDeclHandler) {
        *eventEndPP = s;
        parser->m_entityDeclHandler(
            parser->m_handlerArg, parser->m_declEntity->name,
            parser->m_declEntity->is_param, 0, 0, parser->m_declEntity->base,
            parser->m_declEntity->systemId, parser->m_declEntity->publicId, 0);
        handleDefault = XML_FALSE;
      }
      break;
    case XML_ROLE_ENTITY_NOTATION_NAME:
      if (dtd->keepProcessing && parser->m_declEntity) {
        parser->m_declEntity->notation
            = poolStoreString(&dtd->pool, enc, s, next);
        if (! parser->m_declEntity->notation)
          return XML_ERROR_NO_MEMORY;
        poolFinish(&dtd->pool);
        if (parser->m_unparsedEntityDeclHandler) {
          *eventEndPP = s;
          parser->m_unparsedEntityDeclHandler(
              parser->m_handlerArg, parser->m_declEntity->name,
              parser->m_declEntity->base, parser->m_declEntity->systemId,
              parser->m_declEntity->publicId, parser->m_declEntity->notation);
          handleDefault = XML_FALSE;
        } else if (parser->m_entityDeclHandler) {
          *eventEndPP = s;
          parser->m_entityDeclHandler(
              parser->m_handlerArg, parser->m_declEntity->name, 0, 0, 0,
              parser->m_declEntity->base, parser->m_declEntity->systemId,
              parser->m_declEntity->publicId, parser->m_declEntity->notation);
          handleDefault = XML_FALSE;
        }
      }
      break;
    case XML_ROLE_GENERAL_ENTITY_NAME: {
      if (XmlPredefinedEntityName(enc, s, next)) {
        parser->m_declEntity = NULL;
        break;
      }
      if (dtd->keepProcessing) {
        const XML_Char *name = poolStoreString(&dtd->pool, enc, s, next);
        if (! name)
          return XML_ERROR_NO_MEMORY;
        parser->m_declEntity = (ENTITY *)lookup(parser, &dtd->generalEntities,
                                                name, sizeof(ENTITY));
        if (! parser->m_declEntity)
          return XML_ERROR_NO_MEMORY;
        if (parser->m_declEntity->name != name) {
          poolDiscard(&dtd->pool);
          parser->m_declEntity = NULL;
        } else {
          poolFinish(&dtd->pool);
          parser->m_declEntity->publicId = NULL;
          parser->m_declEntity->is_param = XML_FALSE;
          /* if we have a parent parser or are reading an internal parameter
             entity, then the entity declaration is not considered "internal"
          */
          parser->m_declEntity->is_internal
              = ! (parser->m_parentParser || parser->m_openInternalEntities);
          if (parser->m_entityDeclHandler)
            handleDefault = XML_FALSE;
        }
      } else {
        poolDiscard(&dtd->pool);
        parser->m_declEntity = NULL;
      }
    } break;
    case XML_ROLE_PARAM_ENTITY_NAME:
#ifdef XML_DTD
      if (dtd->keepProcessing) {
        const XML_Char *name = poolStoreString(&dtd->pool, enc, s, next);
        if (! name)
          return XML_ERROR_NO_MEMORY;
        parser->m_declEntity = (ENTITY *)lookup(parser, &dtd->paramEntities,
                                                name, sizeof(ENTITY));
        if (! parser->m_declEntity)
          return XML_ERROR_NO_MEMORY;
        if (parser->m_declEntity->name != name) {
          poolDiscard(&dtd->pool);
          parser->m_declEntity = NULL;
        } else {
          poolFinish(&dtd->pool);
          parser->m_declEntity->publicId = NULL;
          parser->m_declEntity->is_param = XML_TRUE;
          /* if we have a parent parser or are reading an internal parameter
             entity, then the entity declaration is not considered "internal"
          */
          parser->m_declEntity->is_internal
              = ! (parser->m_parentParser || parser->m_openInternalEntities);
          if (parser->m_entityDeclHandler)
            handleDefault = XML_FALSE;
        }
      } else {
        poolDiscard(&dtd->pool);
        parser->m_declEntity = NULL;
      }
#else  /* not XML_DTD */
      parser->m_declEntity = NULL;
#endif /* XML_DTD */
      break;
    case XML_ROLE_NOTATION_NAME:
      parser->m_declNotationPublicId = NULL;
      parser->m_declNotationName = NULL;
      if (parser->m_notationDeclHandler) {
        parser->m_declNotationName
            = poolStoreString(&parser->m_tempPool, enc, s, next);
        if (! parser->m_declNotationName)
          return XML_ERROR_NO_MEMORY;
        poolFinish(&parser->m_tempPool);
        handleDefault = XML_FALSE;
      }
      break;
    case XML_ROLE_NOTATION_PUBLIC_ID:
      if (! XmlIsPublicId(enc, s, next, eventPP))
        return XML_ERROR_PUBLICID;
      if (parser
              ->m_declNotationName) { /* means m_notationDeclHandler != NULL */
        XML_Char *tem = poolStoreString(&parser->m_tempPool, enc,
                                        s + enc->minBytesPerChar,
                                        next - enc->minBytesPerChar);
        if (! tem)
          return XML_ERROR_NO_MEMORY;
        normalizePublicId(tem);
        parser->m_declNotationPublicId = tem;
        poolFinish(&parser->m_tempPool);
        handleDefault = XML_FALSE;
      }
      break;
    case XML_ROLE_NOTATION_SYSTEM_ID:
      if (parser->m_declNotationName && parser->m_notationDeclHandler) {
        const XML_Char *systemId = poolStoreString(&parser->m_tempPool, enc,
                                                   s + enc->minBytesPerChar,
                                                   next - enc->minBytesPerChar);
        if (! systemId)
          return XML_ERROR_NO_MEMORY;
        *eventEndPP = s;
        parser->m_notationDeclHandler(
            parser->m_handlerArg, parser->m_declNotationName, parser->m_curBase,
            systemId, parser->m_declNotationPublicId);
        handleDefault = XML_FALSE;
      }
      poolClear(&parser->m_tempPool);
      break;
    case XML_ROLE_NOTATION_NO_SYSTEM_ID:
      if (parser->m_declNotationPublicId && parser->m_notationDeclHandler) {
        *eventEndPP = s;
        parser->m_notationDeclHandler(
            parser->m_handlerArg, parser->m_declNotationName, parser->m_curBase,
            0, parser->m_declNotationPublicId);
        handleDefault = XML_FALSE;
      }
      poolClear(&parser->m_tempPool);
      break;
    case XML_ROLE_ERROR:
      switch (tok) {
      case XML_TOK_PARAM_ENTITY_REF:
        /* PE references in internal subset are
           not allowed within declarations. */
        return XML_ERROR_PARAM_ENTITY_REF;
      case XML_TOK_XML_DECL:
        return XML_ERROR_MISPLACED_XML_PI;
      default:
        return XML_ERROR_SYNTAX;
      }
#ifdef XML_DTD
    case XML_ROLE_IGNORE_SECT: {
      enum XML_Error result;
      if (parser->m_defaultHandler)
        reportDefault(parser, enc, s, next);
      handleDefault = XML_FALSE;
      result = doIgnoreSection(parser, enc, &next, end, nextPtr, haveMore);
      if (result != XML_ERROR_NONE)
        return result;
      else if (! next) {
        parser->m_processor = ignoreSectionProcessor;
        return result;
      }
    } break;
#endif /* XML_DTD */
    case XML_ROLE_GROUP_OPEN:
      if (parser->m_prologState.level >= parser->m_groupSize) {
        if (parser->m_groupSize) {
          {
            char *const new_connector = (char *)REALLOC(
                parser, parser->m_groupConnector, parser->m_groupSize *= 2);
            if (new_connector == NULL) {
              parser->m_groupSize /= 2;
              return XML_ERROR_NO_MEMORY;
            }
            parser->m_groupConnector = new_connector;
          }

          if (dtd->scaffIndex) {
            int *const new_scaff_index = (int *)REALLOC(
                parser, dtd->scaffIndex, parser->m_groupSize * sizeof(int));
            if (new_scaff_index == NULL)
              return XML_ERROR_NO_MEMORY;
            dtd->scaffIndex = new_scaff_index;
          }
        } else {
          parser->m_groupConnector
              = (char *)MALLOC(parser, parser->m_groupSize = 32);
          if (! parser->m_groupConnector) {
            parser->m_groupSize = 0;
            return XML_ERROR_NO_MEMORY;
          }
        }
      }
      parser->m_groupConnector[parser->m_prologState.level] = 0;
      if (dtd->in_eldecl) {
        int myindex = nextScaffoldPart(parser);
        if (myindex < 0)
          return XML_ERROR_NO_MEMORY;
        assert(dtd->scaffIndex != NULL);
        dtd->scaffIndex[dtd->scaffLevel] = myindex;
        dtd->scaffLevel++;
        dtd->scaffold[myindex].type = XML_CTYPE_SEQ;
        if (parser->m_elementDeclHandler)
          handleDefault = XML_FALSE;
      }
      break;
    case XML_ROLE_GROUP_SEQUENCE:
      if (parser->m_groupConnector[parser->m_prologState.level] == ASCII_PIPE)
        return XML_ERROR_SYNTAX;
      parser->m_groupConnector[parser->m_prologState.level] = ASCII_COMMA;
      if (dtd->in_eldecl && parser->m_elementDeclHandler)
        handleDefault = XML_FALSE;
      break;
    case XML_ROLE_GROUP_CHOICE:
      if (parser->m_groupConnector[parser->m_prologState.level] == ASCII_COMMA)
        return XML_ERROR_SYNTAX;
      if (dtd->in_eldecl
          && ! parser->m_groupConnector[parser->m_prologState.level]
          && (dtd->scaffold[dtd->scaffIndex[dtd->scaffLevel - 1]].type
              != XML_CTYPE_MIXED)) {
        dtd->scaffold[dtd->scaffIndex[dtd->scaffLevel - 1]].type
            = XML_CTYPE_CHOICE;
        if (parser->m_elementDeclHandler)
          handleDefault = XML_FALSE;
      }
      parser->m_groupConnector[parser->m_prologState.level] = ASCII_PIPE;
      break;
    case XML_ROLE_PARAM_ENTITY_REF:
#ifdef XML_DTD
    case XML_ROLE_INNER_PARAM_ENTITY_REF:
      dtd->hasParamEntityRefs = XML_TRUE;
      if (! parser->m_paramEntityParsing)
        dtd->keepProcessing = dtd->standalone;
      else {
        const XML_Char *name;
        ENTITY *entity;
        name = poolStoreString(&dtd->pool, enc, s + enc->minBytesPerChar,
                               next - enc->minBytesPerChar);
        if (! name)
          return XML_ERROR_NO_MEMORY;
        entity = (ENTITY *)lookup(parser, &dtd->paramEntities, name, 0);
        poolDiscard(&dtd->pool);
        /* first, determine if a check for an existing declaration is needed;
           if yes, check that the entity exists, and that it is internal,
           otherwise call the skipped entity handler
        */
        if (parser->m_prologState.documentEntity
            && (dtd->standalone ? ! parser->m_openInternalEntities
                                : ! dtd->hasParamEntityRefs)) {
          if (! entity)
            return XML_ERROR_UNDEFINED_ENTITY;
          else if (! entity->is_internal) {
            /* It's hard to exhaustively search the code to be sure,
             * but there doesn't seem to be a way of executing the
             * following line.  There are two cases:
             *
             * If 'standalone' is false, the DTD must have no
             * parameter entities or we wouldn't have passed the outer
             * 'if' statement.  That measn the only entity in the hash
             * table is the external subset name "#" which cannot be
             * given as a parameter entity name in XML syntax, so the
             * lookup must have returned NULL and we don't even reach
             * the test for an internal entity.
             *
             * If 'standalone' is true, it does not seem to be
             * possible to create entities taking this code path that
             * are not internal entities, so fail the test above.
             *
             * Because this analysis is very uncertain, the code is
             * being left in place and merely removed from the
             * coverage test statistics.
             */
            return XML_ERROR_ENTITY_DECLARED_IN_PE; /* LCOV_EXCL_LINE */
          }
        } else if (! entity) {
          dtd->keepProcessing = dtd->standalone;
          /* cannot report skipped entities in declarations */
          if ((role == XML_ROLE_PARAM_ENTITY_REF)
              && parser->m_skippedEntityHandler) {
            parser->m_skippedEntityHandler(parser->m_handlerArg, name, 1);
            handleDefault = XML_FALSE;
          }
          break;
        }
        if (entity->open)
          return XML_ERROR_RECURSIVE_ENTITY_REF;
        if (entity->textPtr) {
          enum XML_Error result;
          XML_Bool betweenDecl
              = (role == XML_ROLE_PARAM_ENTITY_REF ? XML_TRUE : XML_FALSE);
          result = processInternalEntity(parser, entity, betweenDecl);
          if (result != XML_ERROR_NONE)
            return result;
          handleDefault = XML_FALSE;
          break;
        }
        if (parser->m_externalEntityRefHandler) {
          dtd->paramEntityRead = XML_FALSE;
          entity->open = XML_TRUE;
          if (! parser->m_externalEntityRefHandler(
                  parser->m_externalEntityRefHandlerArg, 0, entity->base,
                  entity->systemId, entity->publicId)) {
            entity->open = XML_FALSE;
            return XML_ERROR_EXTERNAL_ENTITY_HANDLING;
          }
          entity->open = XML_FALSE;
          handleDefault = XML_FALSE;
          if (! dtd->paramEntityRead) {
            dtd->keepProcessing = dtd->standalone;
            break;
          }
        } else {
          dtd->keepProcessing = dtd->standalone;
          break;
        }
      }
#endif /* XML_DTD */
      if (! dtd->standalone && parser->m_notStandaloneHandler
          && ! parser->m_notStandaloneHandler(parser->m_handlerArg))
        return XML_ERROR_NOT_STANDALONE;
      break;

      /* Element declaration stuff */

    case XML_ROLE_ELEMENT_NAME:
      if (parser->m_elementDeclHandler) {
        parser->m_declElementType = getElementType(parser, enc, s, next);
        if (! parser->m_declElementType)
          return XML_ERROR_NO_MEMORY;
        dtd->scaffLevel = 0;
        dtd->scaffCount = 0;
        dtd->in_eldecl = XML_TRUE;
        handleDefault = XML_FALSE;
      }
      break;

    case XML_ROLE_CONTENT_ANY:
    case XML_ROLE_CONTENT_EMPTY:
      if (dtd->in_eldecl) {
        if (parser->m_elementDeclHandler) {
          XML_Content *content
              = (XML_Content *)MALLOC(parser, sizeof(XML_Content));
          if (! content)
            return XML_ERROR_NO_MEMORY;
          content->quant = XML_CQUANT_NONE;
          content->name = NULL;
          content->numchildren = 0;
          content->children = NULL;
          content->type = ((role == XML_ROLE_CONTENT_ANY) ? XML_CTYPE_ANY
                                                          : XML_CTYPE_EMPTY);
          *eventEndPP = s;
          parser->m_elementDeclHandler(
              parser->m_handlerArg, parser->m_declElementType->name, content);
          handleDefault = XML_FALSE;
        }
        dtd->in_eldecl = XML_FALSE;
      }
      break;

    case XML_ROLE_CONTENT_PCDATA:
      if (dtd->in_eldecl) {
        dtd->scaffold[dtd->scaffIndex[dtd->scaffLevel - 1]].type
            = XML_CTYPE_MIXED;
        if (parser->m_elementDeclHandler)
          handleDefault = XML_FALSE;
      }
      break;

    case XML_ROLE_CONTENT_ELEMENT:
      quant = XML_CQUANT_NONE;
      goto elementContent;
    case XML_ROLE_CONTENT_ELEMENT_OPT:
      quant = XML_CQUANT_OPT;
      goto elementContent;
    case XML_ROLE_CONTENT_ELEMENT_REP:
      quant = XML_CQUANT_REP;
      goto elementContent;
    case XML_ROLE_CONTENT_ELEMENT_PLUS:
      quant = XML_CQUANT_PLUS;
    elementContent:
      if (dtd->in_eldecl) {
        ELEMENT_TYPE *el;
        const XML_Char *name;
        int nameLen;
        const char *nxt
            = (quant == XML_CQUANT_NONE ? next : next - enc->minBytesPerChar);
        int myindex = nextScaffoldPart(parser);
        if (myindex < 0)
          return XML_ERROR_NO_MEMORY;
        dtd->scaffold[myindex].type = XML_CTYPE_NAME;
        dtd->scaffold[myindex].quant = quant;
        el = getElementType(parser, enc, s, nxt);
        if (! el)
          return XML_ERROR_NO_MEMORY;
        name = el->name;
        dtd->scaffold[myindex].name = name;
        nameLen = 0;
        for (; name[nameLen++];)
          ;
        dtd->contentStringLen += nameLen;
        if (parser->m_elementDeclHandler)
          handleDefault = XML_FALSE;
      }
      break;

    case XML_ROLE_GROUP_CLOSE:
      quant = XML_CQUANT_NONE;
      goto closeGroup;
    case XML_ROLE_GROUP_CLOSE_OPT:
      quant = XML_CQUANT_OPT;
      goto closeGroup;
    case XML_ROLE_GROUP_CLOSE_REP:
      quant = XML_CQUANT_REP;
      goto closeGroup;
    case XML_ROLE_GROUP_CLOSE_PLUS:
      quant = XML_CQUANT_PLUS;
    closeGroup:
      if (dtd->in_eldecl) {
        if (parser->m_elementDeclHandler)
          handleDefault = XML_FALSE;
        dtd->scaffLevel--;
        dtd->scaffold[dtd->scaffIndex[dtd->scaffLevel]].quant = quant;
        if (dtd->scaffLevel == 0) {
          if (! handleDefault) {
            XML_Content *model = build_model(parser);
            if (! model)
              return XML_ERROR_NO_MEMORY;
            *eventEndPP = s;
            parser->m_elementDeclHandler(
                parser->m_handlerArg, parser->m_declElementType->name, model);
          }
          dtd->in_eldecl = XML_FALSE;
          dtd->contentStringLen = 0;
        }
      }
      break;
      /* End element declaration stuff */

    case XML_ROLE_PI:
      if (! reportProcessingInstruction(parser, enc, s, next))
        return XML_ERROR_NO_MEMORY;
      handleDefault = XML_FALSE;
      break;
    case XML_ROLE_COMMENT:
      if (! reportComment(parser, enc, s, next))
        return XML_ERROR_NO_MEMORY;
      handleDefault = XML_FALSE;
      break;
    case XML_ROLE_NONE:
      switch (tok) {
      case XML_TOK_BOM:
        handleDefault = XML_FALSE;
        break;
      }
      break;
    case XML_ROLE_DOCTYPE_NONE:
      if (parser->m_startDoctypeDeclHandler)
        handleDefault = XML_FALSE;
      break;
    case XML_ROLE_ENTITY_NONE:
      if (dtd->keepProcessing && parser->m_entityDeclHandler)
        handleDefault = XML_FALSE;
      break;
    case XML_ROLE_NOTATION_NONE:
      if (parser->m_notationDeclHandler)
        handleDefault = XML_FALSE;
      break;
    case XML_ROLE_ATTLIST_NONE:
      if (dtd->keepProcessing && parser->m_attlistDeclHandler)
        handleDefault = XML_FALSE;
      break;
    case XML_ROLE_ELEMENT_NONE:
      if (parser->m_elementDeclHandler)
        handleDefault = XML_FALSE;
      break;
    } /* end of big switch */

    if (handleDefault && parser->m_defaultHandler)
      reportDefault(parser, enc, s, next);

    switch (parser->m_parsingStatus.parsing) {
    case XML_SUSPENDED:
      *nextPtr = next;
      return XML_ERROR_NONE;
    case XML_FINISHED:
      return XML_ERROR_ABORTED;
    default:
      s = next;
      tok = XmlPrologTok(enc, s, end, &next);
    }
  }
  /* not reached */
}
