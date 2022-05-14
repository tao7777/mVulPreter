lexer_process_char_literal (parser_context_t *context_p, /**< context */
                            const uint8_t *char_p, /**< characters */
                            size_t length, /**< length of string */
                            uint8_t literal_type, /**< final literal type */
                            bool has_escape) /**< has escape sequences */
{
  parser_list_iterator_t literal_iterator;
  lexer_literal_t *literal_p;
  uint32_t literal_index = 0;

  JERRY_ASSERT (literal_type == LEXER_IDENT_LITERAL
                || literal_type == LEXER_STRING_LITERAL);

  JERRY_ASSERT (literal_type != LEXER_IDENT_LITERAL || length <= PARSER_MAXIMUM_IDENT_LENGTH);
  JERRY_ASSERT (literal_type != LEXER_STRING_LITERAL || length <= PARSER_MAXIMUM_STRING_LENGTH);

  parser_list_iterator_init (&context_p->literal_pool, &literal_iterator);

  while ((literal_p = (lexer_literal_t *) parser_list_iterator_next (&literal_iterator)) != NULL)
  {
    if (literal_p->type == literal_type
        && literal_p->prop.length == length
        && memcmp (literal_p->u.char_p, char_p, length) == 0)
    {
      context_p->lit_object.literal_p = literal_p;
      context_p->lit_object.index = (uint16_t) literal_index;
      literal_p->status_flags = (uint8_t) (literal_p->status_flags & ~LEXER_FLAG_UNUSED_IDENT);
      return;
    }

    literal_index++;
  }

  JERRY_ASSERT (literal_index == context_p->literal_count);

  if (literal_index >= PARSER_MAXIMUM_NUMBER_OF_LITERALS)
  {
     parser_raise_error (context_p, PARSER_ERR_LITERAL_LIMIT_REACHED);
   }
 
   literal_p = (lexer_literal_t *) parser_list_append (context_p, &context_p->literal_pool);
   literal_p->prop.length = (uint16_t) length;
   literal_p->type = literal_type;
  literal_p->status_flags = has_escape ? 0 : LEXER_FLAG_SOURCE_PTR;

  if (has_escape)
  {
    literal_p->u.char_p = (uint8_t *) jmem_heap_alloc_block (length);
    memcpy ((uint8_t *) literal_p->u.char_p, char_p, length);
  }
  else
  {
    literal_p->u.char_p = char_p;
  }

  context_p->lit_object.literal_p = literal_p;
  context_p->lit_object.index = (uint16_t) literal_index;
  context_p->literal_count++;
} /* lexer_process_char_literal */
