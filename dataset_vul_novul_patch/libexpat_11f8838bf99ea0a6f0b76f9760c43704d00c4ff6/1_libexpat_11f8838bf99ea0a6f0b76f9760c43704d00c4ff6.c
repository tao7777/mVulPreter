setElementTypePrefix(XML_Parser parser, ELEMENT_TYPE *elementType)
{
  DTD * const dtd = parser->m_dtd;  /* save one level of indirection */
  const XML_Char *name;
  for (name = elementType->name; *name; name++) {
    if (*name == XML_T(ASCII_COLON)) {
      PREFIX *prefix;
      const XML_Char *s;
      for (s = elementType->name; s != name; s++) {
        if (!poolAppendChar(&dtd->pool, *s))
          return 0;
      }
      if (!poolAppendChar(&dtd->pool, XML_T('\0')))
        return 0;
      prefix = (PREFIX *)lookup(parser, &dtd->prefixes, poolStart(&dtd->pool),
                                sizeof(PREFIX));
      if (!prefix)
        return 0;
      if (prefix->name == poolStart(&dtd->pool))
        poolFinish(&dtd->pool);
       else
         poolDiscard(&dtd->pool);
       elementType->prefix = prefix;
     }
   }
   return 1;
}
