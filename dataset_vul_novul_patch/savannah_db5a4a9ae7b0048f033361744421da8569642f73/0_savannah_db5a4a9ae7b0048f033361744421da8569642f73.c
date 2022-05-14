  ps_parser_skip_PS_token( PS_Parser  parser )
  {
    /* Note: PostScript allows any non-delimiting, non-whitespace        */
    /*       character in a name (PS Ref Manual, 3rd ed, p31).           */
    /*       PostScript delimiters are (, ), <, >, [, ], {, }, /, and %. */

    FT_Byte*  cur   = parser->cursor;
    FT_Byte*  limit = parser->limit;
    FT_Error  error = FT_Err_Ok;


    skip_spaces( &cur, limit );             /* this also skips comments */
    if ( cur >= limit )
      goto Exit;

    /* self-delimiting, single-character tokens */
    if ( *cur == '[' || *cur == ']' )
    {
      cur++;
      goto Exit;
    }

    /* skip balanced expressions (procedures and strings) */

    if ( *cur == '{' )                              /* {...} */
    {
      error = skip_procedure( &cur, limit );
      goto Exit;
    }

    if ( *cur == '(' )                              /* (...) */
    {
      error = skip_literal_string( &cur, limit );
      goto Exit;
    }

    if ( *cur == '<' )                              /* <...> */
    {
      if ( cur + 1 < limit && *(cur + 1) == '<' )   /* << */
      {
        cur++;
        cur++;
      }
      else
        error = skip_string( &cur, limit );

      goto Exit;
    }

    if ( *cur == '>' )
    {
      cur++;
      if ( cur >= limit || *cur != '>' )             /* >> */
      {
        FT_ERROR(( "ps_parser_skip_PS_token:"
                   " unexpected closing delimiter `>'\n" ));
        error = FT_THROW( Invalid_File_Format );
        goto Exit;
      }
      cur++;
      goto Exit;
    }

    if ( *cur == '/' )
      cur++;

    /* anything else */
    while ( cur < limit )
    {
      /* *cur might be invalid (e.g., ')' or '}'), but this   */
      /* is handled by the test `cur == parser->cursor' below */
      if ( IS_PS_DELIM( *cur ) )
        break;

      cur++;
    }

  Exit:
    if ( cur < limit && cur == parser->cursor )
    {
      FT_ERROR(( "ps_parser_skip_PS_token:"
                 " current token is `%c' which is self-delimiting\n"
                 "                        "
                 " but invalid at this point\n",
                 *cur ));

       error = FT_THROW( Invalid_File_Format );
     }
 
    if ( cur > limit )
      cur = limit;

     parser->error  = error;
     parser->cursor = cur;
   }
