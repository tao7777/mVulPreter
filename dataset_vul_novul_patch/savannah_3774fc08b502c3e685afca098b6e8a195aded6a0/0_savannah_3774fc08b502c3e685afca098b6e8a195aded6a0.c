  ps_parser_to_token( PS_Parser  parser,
                      T1_Token   token )
  {
    FT_Byte*  cur;
    FT_Byte*  limit;
    FT_Int    embed;


    token->type  = T1_TOKEN_TYPE_NONE;
    token->start = NULL;
    token->limit = NULL;

    /* first of all, skip leading whitespace */
    ps_parser_skip_spaces( parser );

    cur   = parser->cursor;
    limit = parser->limit;

    if ( cur >= limit )
      return;

    switch ( *cur )
    {
      /************* check for literal string *****************/
    case '(':
      token->type  = T1_TOKEN_TYPE_STRING;
      token->start = cur;

      if ( skip_literal_string( &cur, limit ) == FT_Err_Ok )
        token->limit = cur;
      break;

      /************* check for programs/array *****************/
    case '{':
      token->type  = T1_TOKEN_TYPE_ARRAY;
      token->start = cur;

      if ( skip_procedure( &cur, limit ) == FT_Err_Ok )
        token->limit = cur;
      break;

      /************* check for table/array ********************/
      /* XXX: in theory we should also look for "<<"          */
      /*      since this is semantically equivalent to "[";   */
      /*      in practice it doesn't matter (?)               */
    case '[':
      token->type  = T1_TOKEN_TYPE_ARRAY;
      embed        = 1;
      token->start = cur++;

      /* we need this to catch `[ ]' */
      parser->cursor = cur;
      ps_parser_skip_spaces( parser );
      cur = parser->cursor;

      while ( cur < limit && !parser->error )
      {
        /* XXX: this is wrong because it does not      */
        /*      skip comments, procedures, and strings */
        if ( *cur == '[' )
          embed++;
        else if ( *cur == ']' )
        {
          embed--;
          if ( embed <= 0 )
          {
            token->limit = ++cur;
            break;
          }
        }

        parser->cursor = cur;
        ps_parser_skip_PS_token( parser );
        /* we need this to catch `[XXX ]' */
        ps_parser_skip_spaces  ( parser );
        cur = parser->cursor;
      }
      break;

      /* ************ otherwise, it is any token **************/
    default:
      token->start = cur;
      token->type  = ( *cur == '/' ) ? T1_TOKEN_TYPE_KEY : T1_TOKEN_TYPE_ANY;
      ps_parser_skip_PS_token( parser );
      cur = parser->cursor;
      if ( !parser->error )
        token->limit = cur;
    }

    if ( !token->limit )
    {
      token->start = NULL;
      token->type  = T1_TOKEN_TYPE_NONE;
    }

    parser->cursor = cur;
  }


  /* NB: `tokens' can be NULL if we only want to count */
  /* the number of array elements                      */

  FT_LOCAL_DEF( void )
  ps_parser_to_token_array( PS_Parser  parser,
                            T1_Token   tokens,
                            FT_UInt    max_tokens,
                            FT_Int*    pnum_tokens )
  {
    T1_TokenRec  master;


    *pnum_tokens = -1;

    /* this also handles leading whitespace */
    ps_parser_to_token( parser, &master );

    if ( master.type == T1_TOKEN_TYPE_ARRAY )
    {
      FT_Byte*  old_cursor = parser->cursor;
      FT_Byte*  old_limit  = parser->limit;
      T1_Token  cur        = tokens;
      T1_Token  limit      = cur + max_tokens;


      /* don't include outermost delimiters */
      parser->cursor = master.start + 1;
      parser->limit  = master.limit - 1;

      while ( parser->cursor < parser->limit )
      {
        T1_TokenRec  token;


        ps_parser_to_token( parser, &token );
        if ( !token.type )
          break;

        if ( tokens && cur < limit )
          *cur = token;

        cur++;
      }

      *pnum_tokens = (FT_Int)( cur - tokens );

      parser->cursor = old_cursor;
      parser->limit  = old_limit;
    }
  }


  /* first character must be a delimiter or a part of a number */
  /* NB: `coords' can be NULL if we just want to skip the      */
  /*     array; in this case we ignore `max_coords'            */

  static FT_Int
  ps_tocoordarray( FT_Byte*  *acur,
                   FT_Byte*   limit,
                   FT_Int     max_coords,
                   FT_Short*  coords )
  {
    FT_Byte*  cur   = *acur;
    FT_Int    count = 0;
    FT_Byte   c, ender;


    if ( cur >= limit )
      goto Exit;

    /* check for the beginning of an array; otherwise, only one number */
    /* will be read                                                    */
    c     = *cur;
    ender = 0;

    if ( c == '[' )
      ender = ']';
    else if ( c == '{' )
      ender = '}';

    if ( ender )
      cur++;

    /* now, read the coordinates */
    while ( cur < limit )
    {
      FT_Short  dummy;
      FT_Byte*  old_cur;


      /* skip whitespace in front of data */
      skip_spaces( &cur, limit );
      if ( cur >= limit )
        goto Exit;

      if ( *cur == ender )
      {
        cur++;
        break;
      }

      old_cur = cur;

      if ( coords && count >= max_coords )
        break;

      /* call PS_Conv_ToFixed() even if coords == NULL */
      /* to properly parse number at `cur'             */
      *( coords ? &coords[count] : &dummy ) =
        (FT_Short)( PS_Conv_ToFixed( &cur, limit, 0 ) >> 16 );

      if ( old_cur == cur )
      {
        count = -1;
        goto Exit;
      }
      else
        count++;

      if ( !ender )
        break;
    }

  Exit:
    *acur = cur;
    return count;
  }


  /* first character must be a delimiter or a part of a number */
  /* NB: `values' can be NULL if we just want to skip the      */
  /*     array; in this case we ignore `max_values'            */
  /*                                                           */
  /* return number of successfully parsed values               */

  static FT_Int
  ps_tofixedarray( FT_Byte*  *acur,
                   FT_Byte*   limit,
                   FT_Int     max_values,
                   FT_Fixed*  values,
                   FT_Int     power_ten )
  {
    FT_Byte*  cur   = *acur;
    FT_Int    count = 0;
    FT_Byte   c, ender;


    if ( cur >= limit )
      goto Exit;

    /* Check for the beginning of an array.  Otherwise, only one number */
    /* will be read.                                                    */
    c     = *cur;
    ender = 0;

    if ( c == '[' )
      ender = ']';
    else if ( c == '{' )
      ender = '}';

    if ( ender )
      cur++;

    /* now, read the values */
    while ( cur < limit )
    {
      FT_Fixed  dummy;
      FT_Byte*  old_cur;


      /* skip whitespace in front of data */
      skip_spaces( &cur, limit );
      if ( cur >= limit )
        goto Exit;

      if ( *cur == ender )
      {
        cur++;
        break;
      }

      old_cur = cur;

      if ( values && count >= max_values )
        break;

      /* call PS_Conv_ToFixed() even if coords == NULL */
      /* to properly parse number at `cur'             */
      *( values ? &values[count] : &dummy ) =
        PS_Conv_ToFixed( &cur, limit, power_ten );

      if ( old_cur == cur )
      {
        count = -1;
        goto Exit;
      }
      else
        count++;

      if ( !ender )
        break;
    }

  Exit:
    *acur = cur;
    return count;
  }


#if 0

  static FT_String*
  ps_tostring( FT_Byte**  cursor,
               FT_Byte*   limit,
               FT_Memory  memory )
  {
    FT_Byte*    cur = *cursor;
    FT_UInt     len = 0;
    FT_Int      count;
    FT_String*  result;
    FT_Error    error;


    /* XXX: some stupid fonts have a `Notice' or `Copyright' string     */
    /*      that simply doesn't begin with an opening parenthesis, even */
    /*      though they have a closing one!  E.g. "amuncial.pfb"        */
    /*                                                                  */
    /*      We must deal with these ill-fated cases there.  Note that   */
    /*      these fonts didn't work with the old Type 1 driver as the   */
    /*      notice/copyright was not recognized as a valid string token */
    /*      and made the old token parser commit errors.                */

    while ( cur < limit && ( *cur == ' ' || *cur == '\t' ) )
      cur++;
    if ( cur + 1 >= limit )
      return 0;

    if ( *cur == '(' )
      cur++;  /* skip the opening parenthesis, if there is one */

    *cursor = cur;
    count   = 0;

    /* then, count its length */
    for ( ; cur < limit; cur++ )
    {
      if ( *cur == '(' )
        count++;

      else if ( *cur == ')' )
      {
        count--;
        if ( count < 0 )
          break;
      }
    }

    len = (FT_UInt)( cur - *cursor );
    if ( cur >= limit || FT_ALLOC( result, len + 1 ) )
      return 0;

    /* now copy the string */
    FT_MEM_COPY( result, *cursor, len );
    result[len] = '\0';
    *cursor = cur;
    return result;
  }

#endif /* 0 */


  static int
  ps_tobool( FT_Byte*  *acur,
             FT_Byte*   limit )
  {
    FT_Byte*  cur    = *acur;
    FT_Bool   result = 0;


    /* return 1 if we find `true', 0 otherwise */
    if ( cur + 3 < limit &&
         cur[0] == 't'   &&
         cur[1] == 'r'   &&
         cur[2] == 'u'   &&
         cur[3] == 'e'   )
    {
      result = 1;
      cur   += 5;
    }
    else if ( cur + 4 < limit &&
              cur[0] == 'f'   &&
              cur[1] == 'a'   &&
              cur[2] == 'l'   &&
              cur[3] == 's'   &&
              cur[4] == 'e'   )
    {
      result = 0;
      cur   += 6;
    }

    *acur = cur;
    return result;
  }


  /* load a simple field (i.e. non-table) into the current list of objects */

  FT_LOCAL_DEF( FT_Error )
  ps_parser_load_field( PS_Parser       parser,
                        const T1_Field  field,
                        void**          objects,
                        FT_UInt         max_objects,
                        FT_ULong*       pflags )
  {
    T1_TokenRec   token;
    FT_Byte*      cur;
    FT_Byte*      limit;
    FT_UInt       count;
    FT_UInt       idx;
    FT_Error      error;
    T1_FieldType  type;


    /* this also skips leading whitespace */
    ps_parser_to_token( parser, &token );
    if ( !token.type )
      goto Fail;

    count = 1;
    idx   = 0;
    cur   = token.start;
    limit = token.limit;

    type = field->type;

    /* we must detect arrays in /FontBBox */
    if ( type == T1_FIELD_TYPE_BBOX )
    {
      T1_TokenRec  token2;
      FT_Byte*     old_cur   = parser->cursor;
      FT_Byte*     old_limit = parser->limit;


      /* don't include delimiters */
      parser->cursor = token.start + 1;
      parser->limit  = token.limit - 1;

      ps_parser_to_token( parser, &token2 );
      parser->cursor = old_cur;
      parser->limit  = old_limit;

      if ( token2.type == T1_TOKEN_TYPE_ARRAY )
      {
        type = T1_FIELD_TYPE_MM_BBOX;
        goto FieldArray;
      }
    }
    else if ( token.type == T1_TOKEN_TYPE_ARRAY )
    {
      count = max_objects;

    FieldArray:
      /* if this is an array and we have no blend, an error occurs */
      if ( max_objects == 0 )
        goto Fail;

      idx = 1;

      /* don't include delimiters */
      cur++;
      limit--;
    }

    for ( ; count > 0; count--, idx++ )
    {
      FT_Byte*    q      = (FT_Byte*)objects[idx] + field->offset;
      FT_Long     val;
      FT_String*  string = NULL;


      skip_spaces( &cur, limit );

      switch ( type )
      {
      case T1_FIELD_TYPE_BOOL:
        val = ps_tobool( &cur, limit );
        goto Store_Integer;

      case T1_FIELD_TYPE_FIXED:
        val = PS_Conv_ToFixed( &cur, limit, 0 );
        goto Store_Integer;

      case T1_FIELD_TYPE_FIXED_1000:
        val = PS_Conv_ToFixed( &cur, limit, 3 );
        goto Store_Integer;

      case T1_FIELD_TYPE_INTEGER:
        val = PS_Conv_ToInt( &cur, limit );
        /* fall through */

      Store_Integer:
        switch ( field->size )
        {
        case (8 / FT_CHAR_BIT):
          *(FT_Byte*)q = (FT_Byte)val;
          break;

        case (16 / FT_CHAR_BIT):
          *(FT_UShort*)q = (FT_UShort)val;
          break;

        case (32 / FT_CHAR_BIT):
          *(FT_UInt32*)q = (FT_UInt32)val;
          break;

        default:                /* for 64-bit systems */
          *(FT_Long*)q = val;
        }
        break;

      case T1_FIELD_TYPE_STRING:
      case T1_FIELD_TYPE_KEY:
        {
          FT_Memory  memory = parser->memory;
          FT_UInt    len    = (FT_UInt)( limit - cur );


          if ( cur >= limit )
            break;

          /* we allow both a string or a name   */
          /* for cases like /FontName (foo) def */
          if ( token.type == T1_TOKEN_TYPE_KEY )
          {
            /* don't include leading `/' */
            len--;
            cur++;
          }
          else if ( token.type == T1_TOKEN_TYPE_STRING )
          {
            /* don't include delimiting parentheses    */
            /* XXX we don't handle <<...>> here        */
            /* XXX should we convert octal escapes?    */
            /*     if so, what encoding should we use? */
            cur++;
            len -= 2;
          }
          else
          {
            FT_ERROR(( "ps_parser_load_field:"
                       " expected a name or string\n"
                       "                     "
                       " but found token of type %d instead\n",
                       token.type ));
            error = FT_THROW( Invalid_File_Format );
            goto Exit;
          }

          /* for this to work (FT_String**)q must have been */
          /* initialized to NULL                            */
          if ( *(FT_String**)q )
          {
            FT_TRACE0(( "ps_parser_load_field: overwriting field %s\n",
                        field->ident ));
            FT_FREE( *(FT_String**)q );
            *(FT_String**)q = NULL;
          }

          if ( FT_ALLOC( string, len + 1 ) )
            goto Exit;

          FT_MEM_COPY( string, cur, len );
          string[len] = 0;

          *(FT_String**)q = string;
        }
        break;

      case T1_FIELD_TYPE_BBOX:
        {
          FT_Fixed  temp[4];
          FT_BBox*  bbox = (FT_BBox*)q;
          FT_Int    result;


          result = ps_tofixedarray( &cur, limit, 4, temp, 0 );

          if ( result < 4 )
          {
            FT_ERROR(( "ps_parser_load_field:"
                       " expected four integers in bounding box\n" ));
            error = FT_THROW( Invalid_File_Format );
            goto Exit;
          }

          bbox->xMin = FT_RoundFix( temp[0] );
          bbox->yMin = FT_RoundFix( temp[1] );
          bbox->xMax = FT_RoundFix( temp[2] );
          bbox->yMax = FT_RoundFix( temp[3] );
        }
        break;

      case T1_FIELD_TYPE_MM_BBOX:
        {
          FT_Memory  memory = parser->memory;
          FT_Fixed*  temp   = NULL;
          FT_Int     result;
          FT_UInt    i;


          if ( FT_NEW_ARRAY( temp, max_objects * 4 ) )
            goto Exit;

          for ( i = 0; i < 4; i++ )
          {
            result = ps_tofixedarray( &cur, limit, (FT_Int)max_objects,
                                      temp + i * max_objects, 0 );
            if ( result < 0 || (FT_UInt)result < max_objects )
            {
              FT_ERROR(( "ps_parser_load_field:"
                         " expected %d integer%s in the %s subarray\n"
                         "                     "
                         " of /FontBBox in the /Blend dictionary\n",
                         max_objects, max_objects > 1 ? "s" : "",
                         i == 0 ? "first"
                                : ( i == 1 ? "second"
                                           : ( i == 2 ? "third"
                                                      : "fourth" ) ) ));
              error = FT_THROW( Invalid_File_Format );

              FT_FREE( temp );
              goto Exit;
            }

            skip_spaces( &cur, limit );
          }

          for ( i = 0; i < max_objects; i++ )
          {
            FT_BBox*  bbox = (FT_BBox*)objects[i];


            bbox->xMin = FT_RoundFix( temp[i                  ] );
            bbox->yMin = FT_RoundFix( temp[i +     max_objects] );
            bbox->xMax = FT_RoundFix( temp[i + 2 * max_objects] );
            bbox->yMax = FT_RoundFix( temp[i + 3 * max_objects] );
          }

          FT_FREE( temp );
        }
        break;

      default:
        /* an error occurred */
        goto Fail;
      }
    }

#if 0  /* obsolete -- keep for reference */
    if ( pflags )
      *pflags |= 1L << field->flag_bit;
#else
    FT_UNUSED( pflags );
#endif

    error = FT_Err_Ok;

  Exit:
    return error;

  Fail:
    error = FT_THROW( Invalid_File_Format );
    goto Exit;
  }


#define T1_MAX_TABLE_ELEMENTS  32


  FT_LOCAL_DEF( FT_Error )
  ps_parser_load_field_table( PS_Parser       parser,
                              const T1_Field  field,
                              void**          objects,
                              FT_UInt         max_objects,
                              FT_ULong*       pflags )
  {
    T1_TokenRec  elements[T1_MAX_TABLE_ELEMENTS];
    T1_Token     token;
    FT_Int       num_elements;
    FT_Error     error = FT_Err_Ok;
    FT_Byte*     old_cursor;
    FT_Byte*     old_limit;
    T1_FieldRec  fieldrec = *(T1_Field)field;


    fieldrec.type = T1_FIELD_TYPE_INTEGER;
    if ( field->type == T1_FIELD_TYPE_FIXED_ARRAY ||
         field->type == T1_FIELD_TYPE_BBOX        )
      fieldrec.type = T1_FIELD_TYPE_FIXED;

    ps_parser_to_token_array( parser, elements,
                              T1_MAX_TABLE_ELEMENTS, &num_elements );
    if ( num_elements < 0 )
    {
      error = FT_ERR( Ignore );
      goto Exit;
    }
    if ( (FT_UInt)num_elements > field->array_max )
      num_elements = (FT_Int)field->array_max;

    old_cursor = parser->cursor;
    old_limit  = parser->limit;

    /* we store the elements count if necessary;           */
    /* we further assume that `count_offset' can't be zero */
    if ( field->type != T1_FIELD_TYPE_BBOX && field->count_offset != 0 )
      *(FT_Byte*)( (FT_Byte*)objects[0] + field->count_offset ) =
        (FT_Byte)num_elements;

    /* we now load each element, adjusting the field.offset on each one */
    token = elements;
    for ( ; num_elements > 0; num_elements--, token++ )
    {
      parser->cursor = token->start;
      parser->limit  = token->limit;

      error = ps_parser_load_field( parser,
                                    &fieldrec,
                                    objects,
                                    max_objects,
                                    0 );
      if ( error )
        break;

      fieldrec.offset += fieldrec.size;
    }

#if 0  /* obsolete -- keep for reference */
    if ( pflags )
      *pflags |= 1L << field->flag_bit;
#else
    FT_UNUSED( pflags );
#endif

    parser->cursor = old_cursor;
    parser->limit  = old_limit;

  Exit:
    return error;
  }


  FT_LOCAL_DEF( FT_Long )
  ps_parser_to_int( PS_Parser  parser )
  {
    ps_parser_skip_spaces( parser );
    return PS_Conv_ToInt( &parser->cursor, parser->limit );
  }


  /* first character must be `<' if `delimiters' is non-zero */

  FT_LOCAL_DEF( FT_Error )
  ps_parser_to_bytes( PS_Parser  parser,
                      FT_Byte*   bytes,
                      FT_Offset  max_bytes,
                      FT_ULong*  pnum_bytes,
                      FT_Bool    delimiters )
  {
    FT_Error  error = FT_Err_Ok;
    FT_Byte*  cur;


    ps_parser_skip_spaces( parser );
    cur = parser->cursor;

    if ( cur >= parser->limit )
      goto Exit;

    if ( delimiters )
    {
      if ( *cur != '<' )
      {
        FT_ERROR(( "ps_parser_to_bytes: Missing starting delimiter `<'\n" ));
        error = FT_THROW( Invalid_File_Format );
        goto Exit;
      }

      cur++;
    }

    *pnum_bytes = PS_Conv_ASCIIHexDecode( &cur,
                                          parser->limit,
                                          bytes,
                                          max_bytes );

    if ( delimiters )
    {
      if ( cur < parser->limit && *cur != '>' )
      {
        FT_ERROR(( "ps_parser_to_bytes: Missing closing delimiter `>'\n" ));
        error = FT_THROW( Invalid_File_Format );
        goto Exit;
      }

      cur++;
    }

    parser->cursor = cur;

  Exit:
    return error;
  }


  FT_LOCAL_DEF( FT_Fixed )
  ps_parser_to_fixed( PS_Parser  parser,
                      FT_Int     power_ten )
  {
    ps_parser_skip_spaces( parser );
    return PS_Conv_ToFixed( &parser->cursor, parser->limit, power_ten );
  }


  FT_LOCAL_DEF( FT_Int )
  ps_parser_to_coord_array( PS_Parser  parser,
                            FT_Int     max_coords,
                            FT_Short*  coords )
  {
    ps_parser_skip_spaces( parser );
    return ps_tocoordarray( &parser->cursor, parser->limit,
                            max_coords, coords );
  }


  FT_LOCAL_DEF( FT_Int )
  ps_parser_to_fixed_array( PS_Parser  parser,
                            FT_Int     max_values,
                            FT_Fixed*  values,
                            FT_Int     power_ten )
  {
    ps_parser_skip_spaces( parser );
    return ps_tofixedarray( &parser->cursor, parser->limit,
                            max_values, values, power_ten );
  }


#if 0

  FT_LOCAL_DEF( FT_String* )
  T1_ToString( PS_Parser  parser )
  {
    return ps_tostring( &parser->cursor, parser->limit, parser->memory );
  }


  FT_LOCAL_DEF( FT_Bool )
  T1_ToBool( PS_Parser  parser )
  {
    return ps_tobool( &parser->cursor, parser->limit );
  }

#endif /* 0 */


  FT_LOCAL_DEF( void )
  ps_parser_init( PS_Parser  parser,
                  FT_Byte*   base,
                  FT_Byte*   limit,
                  FT_Memory  memory )
  {
    parser->error  = FT_Err_Ok;
    parser->base   = base;
    parser->limit  = limit;
    parser->cursor = base;
    parser->memory = memory;
    parser->funcs  = ps_parser_funcs;
  }


  FT_LOCAL_DEF( void )
  ps_parser_done( PS_Parser  parser )
  {
    FT_UNUSED( parser );
  }


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                            T1 BUILDER                         *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    t1_builder_init                                                    */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Initializes a given glyph builder.                                 */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    builder :: A pointer to the glyph builder to initialize.           */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face    :: The current face object.                                */
  /*                                                                       */
  /*    size    :: The current size object.                                */
  /*                                                                       */
  /*    glyph   :: The current glyph object.                               */
  /*                                                                       */
  /*    hinting :: Whether hinting should be applied.                      */
  /*                                                                       */
  FT_LOCAL_DEF( void )
  t1_builder_init( T1_Builder    builder,
                   FT_Face       face,
                   FT_Size       size,
                   FT_GlyphSlot  glyph,
                   FT_Bool       hinting )
  {
    builder->parse_state = T1_Parse_Start;
    builder->load_points = 1;

    builder->face   = face;
    builder->glyph  = glyph;
    builder->memory = face->memory;

    if ( glyph )
    {
      FT_GlyphLoader  loader = glyph->internal->loader;


      builder->loader  = loader;
      builder->base    = &loader->base.outline;
      builder->current = &loader->current.outline;
      FT_GlyphLoader_Rewind( loader );

      builder->hints_globals = size->internal;
      builder->hints_funcs   = NULL;

      if ( hinting )
        builder->hints_funcs = glyph->internal->glyph_hints;
    }

    builder->pos_x = 0;
    builder->pos_y = 0;

    builder->left_bearing.x = 0;
    builder->left_bearing.y = 0;
    builder->advance.x      = 0;
    builder->advance.y      = 0;

    builder->funcs = t1_builder_funcs;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    t1_builder_done                                                    */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Finalizes a given glyph builder.  Its contents can still be used   */
  /*    after the call, but the function saves important information       */
  /*    within the corresponding glyph slot.                               */
  /*                                                                       */
  /* <Input>                                                               */
  /*    builder :: A pointer to the glyph builder to finalize.             */
  /*                                                                       */
  FT_LOCAL_DEF( void )
  t1_builder_done( T1_Builder  builder )
  {
    FT_GlyphSlot  glyph = builder->glyph;


    if ( glyph )
      glyph->outline = *builder->base;
  }


  /* check that there is enough space for `count' more points */
  FT_LOCAL_DEF( FT_Error )
  t1_builder_check_points( T1_Builder  builder,
                           FT_Int      count )
  {
    return FT_GLYPHLOADER_CHECK_POINTS( builder->loader, count, 0 );
  }


  /* add a new point, do not check space */
  FT_LOCAL_DEF( void )
  t1_builder_add_point( T1_Builder  builder,
                        FT_Pos      x,
                        FT_Pos      y,
                        FT_Byte     flag )
  {
    FT_Outline*  outline = builder->current;


    if ( builder->load_points )
    {
      FT_Vector*  point   = outline->points + outline->n_points;
      FT_Byte*    control = (FT_Byte*)outline->tags + outline->n_points;


      point->x = FIXED_TO_INT( x );
      point->y = FIXED_TO_INT( y );
      *control = (FT_Byte)( flag ? FT_CURVE_TAG_ON : FT_CURVE_TAG_CUBIC );
    }
    outline->n_points++;
  }


  /* check space for a new on-curve point, then add it */
  FT_LOCAL_DEF( FT_Error )
  t1_builder_add_point1( T1_Builder  builder,
                         FT_Pos      x,
                         FT_Pos      y )
  {
    FT_Error  error;


    error = t1_builder_check_points( builder, 1 );
    if ( !error )
      t1_builder_add_point( builder, x, y, 1 );

    return error;
  }


  /* check space for a new contour, then add it */
  FT_LOCAL_DEF( FT_Error )
  t1_builder_add_contour( T1_Builder  builder )
  {
    FT_Outline*  outline = builder->current;
    FT_Error     error;


    /* this might happen in invalid fonts */
    if ( !outline )
    {
      FT_ERROR(( "t1_builder_add_contour: no outline to add points to\n" ));
      return FT_THROW( Invalid_File_Format );
    }

    if ( !builder->load_points )
    {
      outline->n_contours++;
      return FT_Err_Ok;
    }

    error = FT_GLYPHLOADER_CHECK_POINTS( builder->loader, 0, 1 );
    if ( !error )
    {
      if ( outline->n_contours > 0 )
        outline->contours[outline->n_contours - 1] =
          (short)( outline->n_points - 1 );

      outline->n_contours++;
    }

    return error;
  }


  /* if a path was begun, add its first on-curve point */
  FT_LOCAL_DEF( FT_Error )
  t1_builder_start_point( T1_Builder  builder,
                          FT_Pos      x,
                          FT_Pos      y )
  {
    FT_Error  error = FT_ERR( Invalid_File_Format );


    /* test whether we are building a new contour */

    if ( builder->parse_state == T1_Parse_Have_Path )
      error = FT_Err_Ok;
    else
    {
      builder->parse_state = T1_Parse_Have_Path;
      error = t1_builder_add_contour( builder );
      if ( !error )
        error = t1_builder_add_point1( builder, x, y );
    }

    return error;
  }


  /* close the current contour */
  FT_LOCAL_DEF( void )
  t1_builder_close_contour( T1_Builder  builder )
  {
    FT_Outline*  outline = builder->current;
    FT_Int       first;


    if ( !outline )
      return;

     first = outline->n_contours <= 1
             ? 0 : outline->contours[outline->n_contours - 2] + 1;
 
    /* in malformed fonts it can happen that a contour was started */
    /* but no points were added                                    */
    if ( outline->n_contours && first == outline->n_points )
    {
      outline->n_contours--;
      return;
    }

     /* We must not include the last point in the path if it */
     /* is located on the first point.                       */
     if ( outline->n_points > 1 )
      if ( p1->x == p2->x && p1->y == p2->y )
        if ( *control == FT_CURVE_TAG_ON )
          outline->n_points--;
    }

    if ( outline->n_contours > 0 )
    {
      /* Don't add contours only consisting of one point, i.e.,  */
      /* check whether the first and the last point is the same. */
      if ( first == outline->n_points - 1 )
      {
        outline->n_contours--;
        outline->n_points--;
      }
      else
        outline->contours[outline->n_contours - 1] =
          (short)( outline->n_points - 1 );
    }
  }
