  T42_Open_Face( T42_Face  face )
  {
    T42_LoaderRec  loader;
    T42_Parser     parser;
    T1_Font        type1 = &face->type1;
    FT_Memory      memory = face->root.memory;
    FT_Error       error;

    PSAux_Service  psaux  = (PSAux_Service)face->psaux;


    t42_loader_init( &loader, face );

    parser = &loader.parser;

     if ( FT_ALLOC( face->ttf_data, 12 ) )
       goto Exit;
 
     error = t42_parser_init( parser,
                              face->root.stream,
                              memory,
    if ( error )
      goto Exit;

    if ( type1->font_type != 42 )
    {
      FT_ERROR(( "T42_Open_Face: cannot handle FontType %d\n",
                 type1->font_type ));
      error = FT_THROW( Unknown_File_Format );
      goto Exit;
    }

    /* now, propagate the charstrings and glyphnames tables */
    /* to the Type1 data                                    */
    type1->num_glyphs = loader.num_glyphs;

    if ( !loader.charstrings.init )
    {
      FT_ERROR(( "T42_Open_Face: no charstrings array in face\n" ));
      error = FT_THROW( Invalid_File_Format );
    }

    loader.charstrings.init  = 0;
    type1->charstrings_block = loader.charstrings.block;
    type1->charstrings       = loader.charstrings.elements;
    type1->charstrings_len   = loader.charstrings.lengths;

    /* we copy the glyph names `block' and `elements' fields; */
    /* the `lengths' field must be released later             */
    type1->glyph_names_block    = loader.glyph_names.block;
    type1->glyph_names          = (FT_String**)loader.glyph_names.elements;
    loader.glyph_names.block    = 0;
    loader.glyph_names.elements = 0;

    /* we must now build type1.encoding when we have a custom array */
    if ( type1->encoding_type == T1_ENCODING_TYPE_ARRAY )
    {
      FT_Int    charcode, idx, min_char, max_char;
      FT_Byte*  glyph_name;


      /* OK, we do the following: for each element in the encoding   */
      /* table, look up the index of the glyph having the same name  */
      /* as defined in the CharStrings array.                        */
      /* The index is then stored in type1.encoding.char_index, and  */
      /* the name in type1.encoding.char_name                        */

      min_char = 0;
      max_char = 0;

      charcode = 0;
      for ( ; charcode < loader.encoding_table.max_elems; charcode++ )
      {
        FT_Byte*  char_name;


        type1->encoding.char_index[charcode] = 0;
        type1->encoding.char_name [charcode] = (char *)".notdef";

        char_name = loader.encoding_table.elements[charcode];
        if ( char_name )
          for ( idx = 0; idx < type1->num_glyphs; idx++ )
          {
            glyph_name = (FT_Byte*)type1->glyph_names[idx];
            if ( ft_strcmp( (const char*)char_name,
                            (const char*)glyph_name ) == 0 )
            {
              type1->encoding.char_index[charcode] = (FT_UShort)idx;
              type1->encoding.char_name [charcode] = (char*)glyph_name;

              /* Change min/max encoded char only if glyph name is */
              /* not /.notdef                                      */
              if ( ft_strcmp( (const char*)".notdef",
                              (const char*)glyph_name ) != 0 )
              {
                if ( charcode < min_char )
                  min_char = charcode;
                if ( charcode >= max_char )
                  max_char = charcode + 1;
              }
              break;
            }
          }
      }

      type1->encoding.code_first = min_char;
      type1->encoding.code_last  = max_char;
      type1->encoding.num_chars  = loader.num_chars;
    }

  Exit:
    t42_loader_done( &loader );
    return error;
  }
