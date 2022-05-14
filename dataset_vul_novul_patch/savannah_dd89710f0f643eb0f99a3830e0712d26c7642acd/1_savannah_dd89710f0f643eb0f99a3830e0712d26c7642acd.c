  parse_charstrings( T1_Face    face,
                     T1_Loader  loader )
  {
    T1_Parser      parser       = &loader->parser;
    PS_Table       code_table   = &loader->charstrings;
    PS_Table       name_table   = &loader->glyph_names;
    PS_Table       swap_table   = &loader->swap_table;
    FT_Memory      memory       = parser->root.memory;
    FT_Error       error;

    PSAux_Service  psaux        = (PSAux_Service)face->psaux;

    FT_Byte*       cur;
    FT_Byte*       limit        = parser->root.limit;
    FT_Int         n, num_glyphs;
    FT_UInt        notdef_index = 0;
    FT_Byte        notdef_found = 0;


    num_glyphs = (FT_Int)T1_ToInt( parser );
    if ( num_glyphs < 0 )
    {
      error = FT_THROW( Invalid_File_Format );
      goto Fail;
    }

    /* some fonts like Optima-Oblique not only define the /CharStrings */
    /* array but access it also                                        */
    if ( num_glyphs == 0 || parser->root.error )
      return;

    /* initialize tables, leaving space for addition of .notdef, */
    /* if necessary, and a few other glyphs to handle buggy      */
    /* fonts which have more glyphs than specified.              */

    /* for some non-standard fonts like `Optima' which provides  */
    /* different outlines depending on the resolution it is      */
    /* possible to get here twice                                */
    if ( !loader->num_glyphs )
    {
      error = psaux->ps_table_funcs->init(
                code_table, num_glyphs + 1 + TABLE_EXTEND, memory );
      if ( error )
        goto Fail;

      error = psaux->ps_table_funcs->init(
                name_table, num_glyphs + 1 + TABLE_EXTEND, memory );
      if ( error )
        goto Fail;

      /* Initialize table for swapping index notdef_index and */
      /* index 0 names and codes (if necessary).              */

      error = psaux->ps_table_funcs->init( swap_table, 4, memory );
      if ( error )
        goto Fail;
    }

    n = 0;

    for (;;)
    {
      FT_Long   size;
      FT_Byte*  base;


      /* the format is simple:        */
      /*   `/glyphname' + binary data */

      T1_Skip_Spaces( parser );

      cur = parser->root.cursor;
      if ( cur >= limit )
        break;

      /* we stop when we find a `def' or `end' keyword */
      if ( cur + 3 < limit && IS_PS_DELIM( cur[3] ) )
      {
        if ( cur[0] == 'd' &&
             cur[1] == 'e' &&
             cur[2] == 'f' )
        {
          /* There are fonts which have this: */
          /*                                  */
          /*   /CharStrings 118 dict def      */
          /*   Private begin                  */
          /*   CharStrings begin              */
          /*   ...                            */
          /*                                  */
          /* To catch this we ignore `def' if */
          /* no charstring has actually been  */
          /* seen.                            */
          if ( n )
            break;
        }

        if ( cur[0] == 'e' &&
             cur[1] == 'n' &&
             cur[2] == 'd' )
          break;
      }

      T1_Skip_PS_Token( parser );
      if ( parser->root.error )
        return;

      if ( *cur == '/' )
      {
         FT_PtrDist  len;
 
 
        if ( cur + 1 >= limit )
         {
           error = FT_THROW( Invalid_File_Format );
           goto Fail;
        }

        cur++;                              /* skip `/' */
        len = parser->root.cursor - cur;

        if ( !read_binary_data( parser, &size, &base, IS_INCREMENTAL ) )
          return;

        /* for some non-standard fonts like `Optima' which provides */
        /* different outlines depending on the resolution it is     */
        /* possible to get here twice                               */
        if ( loader->num_glyphs )
          continue;

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

        if ( face->type1.private_dict.lenIV >= 0 &&
             n < num_glyphs + TABLE_EXTEND       )
        {
          FT_Byte*  temp;


          if ( size <= face->type1.private_dict.lenIV )
          {
            error = FT_THROW( Invalid_File_Format );
            goto Fail;
          }

          /* t1_decrypt() shouldn't write to base -- make temporary copy */
          if ( FT_ALLOC( temp, size ) )
            goto Fail;
          FT_MEM_COPY( temp, base, size );
          psaux->t1_decrypt( temp, size, 4330 );
          size -= face->type1.private_dict.lenIV;
          error = T1_Add_Table( code_table, n,
                                temp + face->type1.private_dict.lenIV, size );
          FT_FREE( temp );
        }
        else
          error = T1_Add_Table( code_table, n, base, size );
        if ( error )
          goto Fail;

        n++;
      }
    }

    loader->num_glyphs = n;

    /* if /.notdef is found but does not occupy index 0, do our magic. */
    if ( notdef_found                                                 &&
         ft_strcmp( ".notdef", (const char*)name_table->elements[0] ) )
    {
      /* Swap glyph in index 0 with /.notdef glyph.  First, add index 0  */
      /* name and code entries to swap_table.  Then place notdef_index   */
      /* name and code entries into swap_table.  Then swap name and code */
      /* entries at indices notdef_index and 0 using values stored in    */
      /* swap_table.                                                     */

      /* Index 0 name */
      error = T1_Add_Table( swap_table, 0,
                            name_table->elements[0],
                            name_table->lengths [0] );
      if ( error )
        goto Fail;

      /* Index 0 code */
      error = T1_Add_Table( swap_table, 1,
                            code_table->elements[0],
                            code_table->lengths [0] );
      if ( error )
        goto Fail;

      /* Index notdef_index name */
      error = T1_Add_Table( swap_table, 2,
                            name_table->elements[notdef_index],
                            name_table->lengths [notdef_index] );
      if ( error )
        goto Fail;

      /* Index notdef_index code */
      error = T1_Add_Table( swap_table, 3,
                            code_table->elements[notdef_index],
                            code_table->lengths [notdef_index] );
      if ( error )
        goto Fail;

      error = T1_Add_Table( name_table, notdef_index,
                            swap_table->elements[0],
                            swap_table->lengths [0] );
      if ( error )
        goto Fail;

      error = T1_Add_Table( code_table, notdef_index,
                            swap_table->elements[1],
                            swap_table->lengths [1] );
      if ( error )
        goto Fail;

      error = T1_Add_Table( name_table, 0,
                            swap_table->elements[2],
                            swap_table->lengths [2] );
      if ( error )
        goto Fail;

      error = T1_Add_Table( code_table, 0,
                            swap_table->elements[3],
                            swap_table->lengths [3] );
      if ( error )
        goto Fail;

    }
    else if ( !notdef_found )
    {
      /* notdef_index is already 0, or /.notdef is undefined in   */
      /* charstrings dictionary.  Worry about /.notdef undefined. */
      /* We take index 0 and add it to the end of the table(s)    */
      /* and add our own /.notdef glyph to index 0.               */

      /* 0 333 hsbw endchar */
      FT_Byte  notdef_glyph[] = { 0x8B, 0xF7, 0xE1, 0x0D, 0x0E };
      char*    notdef_name    = (char *)".notdef";


      error = T1_Add_Table( swap_table, 0,
                            name_table->elements[0],
                            name_table->lengths [0] );
      if ( error )
        goto Fail;

      error = T1_Add_Table( swap_table, 1,
                            code_table->elements[0],
                            code_table->lengths [0] );
      if ( error )
        goto Fail;

      error = T1_Add_Table( name_table, 0, notdef_name, 8 );
      if ( error )
        goto Fail;

      error = T1_Add_Table( code_table, 0, notdef_glyph, 5 );

      if ( error )
        goto Fail;

      error = T1_Add_Table( name_table, n,
                            swap_table->elements[0],
                            swap_table->lengths [0] );
      if ( error )
        goto Fail;

      error = T1_Add_Table( code_table, n,
                            swap_table->elements[1],
                            swap_table->lengths [1] );
      if ( error )
        goto Fail;

      /* we added a glyph. */
      loader->num_glyphs += 1;
    }

    return;

  Fail:
    parser->root.error = error;
  }
