  parse_encoding( T1_Face    face,
                  T1_Loader  loader )
  {
    T1_Parser  parser = &loader->parser;
    FT_Byte*   cur;
    FT_Byte*   limit  = parser->root.limit;

    PSAux_Service  psaux = (PSAux_Service)face->psaux;


    T1_Skip_Spaces( parser );
    cur = parser->root.cursor;
    if ( cur >= limit )
    {
      FT_ERROR(( "parse_encoding: out of bounds\n" ));
      parser->root.error = FT_THROW( Invalid_File_Format );
      return;
    }

    /* if we have a number or `[', the encoding is an array, */
    /* and we must load it now                               */
    if ( ft_isdigit( *cur ) || *cur == '[' )
    {
      T1_Encoding  encode          = &face->type1.encoding;
      FT_Int       count, n;
      PS_Table     char_table      = &loader->encoding_table;
      FT_Memory    memory          = parser->root.memory;
      FT_Error     error;
      FT_Bool      only_immediates = 0;


      /* read the number of entries in the encoding; should be 256 */
      if ( *cur == '[' )
      {
        count           = 256;
        only_immediates = 1;
        parser->root.cursor++;
      }
      else
        count = (FT_Int)T1_ToInt( parser );

      T1_Skip_Spaces( parser );
      if ( parser->root.cursor >= limit )
        return;

      /* we use a T1_Table to store our charnames */
      loader->num_chars = encode->num_chars = count;
      if ( FT_NEW_ARRAY( encode->char_index, count )     ||
           FT_NEW_ARRAY( encode->char_name,  count )     ||
           FT_SET_ERROR( psaux->ps_table_funcs->init(
                           char_table, count, memory ) ) )
      {
        parser->root.error = error;
        return;
      }

      /* We need to `zero' out encoding_table.elements */
      for ( n = 0; n < count; n++ )
      {
        char*  notdef = (char *)".notdef";


        T1_Add_Table( char_table, n, notdef, 8 );
      }

      /* Now we need to read records of the form                */
      /*                                                        */
      /*   ... charcode /charname ...                           */
      /*                                                        */
      /* for each entry in our table.                           */
      /*                                                        */
      /* We simply look for a number followed by an immediate   */
      /* name.  Note that this ignores correctly the sequence   */
      /* that is often seen in type1 fonts:                     */
      /*                                                        */
      /*   0 1 255 { 1 index exch /.notdef put } for dup        */
      /*                                                        */
      /* used to clean the encoding array before anything else. */
      /*                                                        */
      /* Alternatively, if the array is directly given as       */
      /*                                                        */
      /*   /Encoding [ ... ]                                    */
      /*                                                        */
      /* we only read immediates.                               */

      n = 0;
      T1_Skip_Spaces( parser );

      while ( parser->root.cursor < limit )
      {
        cur = parser->root.cursor;

        /* we stop when we encounter a `def' or `]' */
        if ( *cur == 'd' && cur + 3 < limit )
        {
          if ( cur[1] == 'e'         &&
               cur[2] == 'f'         &&
               IS_PS_DELIM( cur[3] ) )
          {
            FT_TRACE6(( "encoding end\n" ));
            cur += 3;
            break;
          }
        }
        if ( *cur == ']' )
        {
          FT_TRACE6(( "encoding end\n" ));
          cur++;
          break;
        }

        /* check whether we've found an entry */
        if ( ft_isdigit( *cur ) || only_immediates )
        {
          FT_Int  charcode;


          if ( only_immediates )
            charcode = n;
          else
           {
             charcode = (FT_Int)T1_ToInt( parser );
             T1_Skip_Spaces( parser );

            /* protect against invalid charcode */
            if ( cur == parser->root.cursor )
            {
              parser->root.error = FT_THROW( Unknown_File_Format );
              return;
            }
           }
 
           cur = parser->root.cursor;

            parser->root.cursor = cur;
            T1_Skip_PS_Token( parser );
            if ( parser->root.cursor >= limit )
              return;
            if ( parser->root.error )
              return;

            len = parser->root.cursor - cur;

            parser->root.error = T1_Add_Table( char_table, charcode,
                                               cur, len + 1 );
            if ( parser->root.error )
              return;
            char_table->elements[charcode][len] = '\0';

            n++;
          }
          else if ( only_immediates )
          {
            /* Since the current position is not updated for           */
            /* immediates-only mode we would get an infinite loop if   */
            /* we don't do anything here.                              */
            /*                                                         */
            /* This encoding array is not valid according to the type1 */
            /* specification (it might be an encoding for a CID type1  */
            /* font, however), so we conclude that this font is NOT a  */
            /* type1 font.                                             */
            parser->root.error = FT_THROW( Unknown_File_Format );
            return;
          }
        }
        else
        {
          T1_Skip_PS_Token( parser );
          if ( parser->root.error )
            return;
        }

        T1_Skip_Spaces( parser );
      }

      face->type1.encoding_type = T1_ENCODING_TYPE_ARRAY;
      parser->root.cursor       = cur;
    }
