  t42_parse_charstrings( T42_Face    face,
                         T42_Loader  loader )
  {
    T42_Parser     parser       = &loader->parser;
    PS_Table       code_table   = &loader->charstrings;
    PS_Table       name_table   = &loader->glyph_names;
    PS_Table       swap_table   = &loader->swap_table;
    FT_Memory      memory       = parser->root.memory;
    FT_Error       error;

    PSAux_Service  psaux        = (PSAux_Service)face->psaux;

    FT_Byte*       cur;
    FT_Byte*       limit        = parser->root.limit;
    FT_UInt        n;
    FT_UInt        notdef_index = 0;
    FT_Byte        notdef_found = 0;


    T1_Skip_Spaces( parser );

    if ( parser->root.cursor >= limit )
    {
      FT_ERROR(( "t42_parse_charstrings: out of bounds\n" ));
      error = FT_THROW( Invalid_File_Format );
      goto Fail;
    }

    if ( ft_isdigit( *parser->root.cursor ) )
    {
      loader->num_glyphs = (FT_UInt)T1_ToInt( parser );
      if ( parser->root.error )
        return;
    }
    else if ( *parser->root.cursor == '<' )
    {
      /* We have `<< ... >>'.  Count the number of `/' in the dictionary */
      /* to get its size.                                                */
      FT_UInt  count = 0;


      T1_Skip_PS_Token( parser );
      if ( parser->root.error )
        return;
      T1_Skip_Spaces( parser );
      cur = parser->root.cursor;

      while ( parser->root.cursor < limit )
      {
        if ( *parser->root.cursor == '/' )
          count++;
        else if ( *parser->root.cursor == '>' )
        {
          loader->num_glyphs  = count;
          parser->root.cursor = cur;        /* rewind */
          break;
        }
        T1_Skip_PS_Token( parser );
        if ( parser->root.error )
          return;
        T1_Skip_Spaces( parser );
      }
    }
    else
    {
      FT_ERROR(( "t42_parse_charstrings: invalid token\n" ));
      error = FT_THROW( Invalid_File_Format );
      goto Fail;
    }

    if ( parser->root.cursor >= limit )
    {
      FT_ERROR(( "t42_parse_charstrings: out of bounds\n" ));
      error = FT_THROW( Invalid_File_Format );
      goto Fail;
    }

    /* initialize tables */

    error = psaux->ps_table_funcs->init( code_table,
                                         loader->num_glyphs,
                                         memory );
    if ( error )
      goto Fail;

    error = psaux->ps_table_funcs->init( name_table,
                                         loader->num_glyphs,
                                         memory );
    if ( error )
      goto Fail;

    /* Initialize table for swapping index notdef_index and */
    /* index 0 names and codes (if necessary).              */

    error = psaux->ps_table_funcs->init( swap_table, 4, memory );
    if ( error )
      goto Fail;

    n = 0;

    for (;;)
    {
      /* The format is simple:                   */
      /*   `/glyphname' + index [+ def]          */

      T1_Skip_Spaces( parser );

      cur = parser->root.cursor;
      if ( cur >= limit )
        break;

      /* We stop when we find an `end' keyword or '>' */
      if ( *cur   == 'e'          &&
           cur + 3 < limit        &&
           cur[1] == 'n'          &&
           cur[2] == 'd'          &&
           t42_is_space( cur[3] ) )
        break;
      if ( *cur == '>' )
         break;
 
       T1_Skip_PS_Token( parser );
       if ( parser->root.error )
         return;
        {
          FT_ERROR(( "t42_parse_charstrings: out of bounds\n" ));
          error = FT_THROW( Invalid_File_Format );
          goto Fail;
        }

        cur++;                              /* skip `/' */
        len = parser->root.cursor - cur;

        error = T1_Add_Table( name_table, n, cur, len + 1 );
        if ( error )
          goto Fail;

        /* add a trailing zero to the name table */
        name_table->elements[n][len] = '\0';

        /* record index of /.notdef */
        if ( *cur == '.'                                              &&
             ft_strcmp( ".notdef",
                        (const char*)(name_table->elements[n]) ) == 0 )
        {
          notdef_index = n;
          notdef_found = 1;
        }

        T1_Skip_Spaces( parser );

        cur = parser->root.cursor;

        (void)T1_ToInt( parser );
        if ( parser->root.cursor >= limit )
        {
          FT_ERROR(( "t42_parse_charstrings: out of bounds\n" ));
          error = FT_THROW( Invalid_File_Format );
          goto Fail;
        }

        len = parser->root.cursor - cur;

        error = T1_Add_Table( code_table, n, cur, len + 1 );
        if ( error )
          goto Fail;

        code_table->elements[n][len] = '\0';

        n++;
        if ( n >= loader->num_glyphs )
          break;
      }
    }
