  cff_parser_run( CFF_Parser  parser,
                  FT_Byte*    start,
                  FT_Byte*    limit )
  {
    FT_Byte*    p       = start;
    FT_Error    error   = FT_Err_Ok;
    FT_Library  library = parser->library;
    FT_UNUSED( library );


    parser->top    = parser->stack;
    parser->start  = start;
    parser->limit  = limit;
    parser->cursor = start;

    while ( p < limit )
    {
      FT_UInt  v = *p;

      /* Opcode 31 is legacy MM T2 operator, not a number.      */
      /* Opcode 255 is reserved and should not appear in fonts; */
      /* it is used internally for CFF2 blends.                 */
      if ( v >= 27 && v != 31 && v != 255 )
      {
        /* it's a number; we will push its position on the stack */
        if ( (FT_UInt)( parser->top - parser->stack ) >= parser->stackSize )
          goto Stack_Overflow;

        *parser->top++ = p;

        /* now, skip it */
        if ( v == 30 )
        {
          /* skip real number */
          p++;
          for (;;)
          {
            /* An unterminated floating point number at the */
            /* end of a dictionary is invalid but harmless. */
            if ( p >= limit )
              goto Exit;
            v = p[0] >> 4;
            if ( v == 15 )
              break;
            v = p[0] & 0xF;
            if ( v == 15 )
              break;
            p++;
          }
        }
        else if ( v == 28 )
          p += 2;
        else if ( v == 29 )
          p += 4;
        else if ( v > 246 )
          p += 1;
      }
#ifdef CFF_CONFIG_OPTION_OLD_ENGINE
      else if ( v == 31 )
      {
        /* a Type 2 charstring */

        CFF_Decoder  decoder;
        CFF_FontRec  cff_rec;
        FT_Byte*     charstring_base;
        FT_ULong     charstring_len;

        FT_Fixed*  stack;
        FT_Byte*   q;


        charstring_base = ++p;

        /* search `endchar' operator */
        for (;;)
        {
          if ( p >= limit )
            goto Exit;
          if ( *p == 14 )
            break;
          p++;
        }

        charstring_len = (FT_ULong)( p - charstring_base ) + 1;

        /* construct CFF_Decoder object */
        FT_ZERO( &decoder );
        FT_ZERO( &cff_rec );

        cff_rec.top_font.font_dict.num_designs = parser->num_designs;
        cff_rec.top_font.font_dict.num_axes    = parser->num_axes;
        decoder.cff                            = &cff_rec;

        error = cff_decoder_parse_charstrings( &decoder,
                                               charstring_base,
                                               charstring_len,
                                               1 );

        /* Now copy the stack data in the temporary decoder object,    */
        /* converting it back to charstring number representations     */
        /* (this is ugly, I know).                                     */
        /*                                                             */
        /* We overwrite the original top DICT charstring under the     */
        /* assumption that the charstring representation of the result */
        /* of `cff_decoder_parse_charstrings' is shorter, which should */
        /* be always true.                                             */

        q     = charstring_base - 1;
        stack = decoder.stack;

        while ( stack < decoder.top )
        {
          FT_ULong  num;
          FT_Bool   neg;


          if ( (FT_UInt)( parser->top - parser->stack ) >= parser->stackSize )
            goto Stack_Overflow;

          *parser->top++ = q;

          if ( *stack < 0 )
          {
            num = (FT_ULong)-*stack;
            neg = 1;
          }
          else
          {
            num = (FT_ULong)*stack;
            neg = 0;
          }

          if ( num & 0xFFFFU )
          {
            if ( neg )
              num = (FT_ULong)-num;

            *q++ = 255;
            *q++ = ( num & 0xFF000000U ) >> 24;
            *q++ = ( num & 0x00FF0000U ) >> 16;
            *q++ = ( num & 0x0000FF00U ) >>  8;
            *q++ =   num & 0x000000FFU;
          }
          else
          {
            num >>= 16;

            if ( neg )
            {
              if ( num <= 107 )
                *q++ = (FT_Byte)( 139 - num );
              else if ( num <= 1131 )
              {
                *q++ = (FT_Byte)( ( ( num - 108 ) >> 8 ) + 251 );
                *q++ = (FT_Byte)( ( num - 108 ) & 0xFF );
              }
              else
              {
                num = (FT_ULong)-num;

                *q++ = 28;
                *q++ = (FT_Byte)( num >> 8 );
                *q++ = (FT_Byte)( num & 0xFF );
              }
            }
            else
            {
              if ( num <= 107 )
                *q++ = (FT_Byte)( num + 139 );
              else if ( num <= 1131 )
              {
                *q++ = (FT_Byte)( ( ( num - 108 ) >> 8 ) + 247 );
                *q++ = (FT_Byte)( ( num - 108 ) & 0xFF );
              }
              else
              {
                *q++ = 28;
                *q++ = (FT_Byte)( num >> 8 );
                *q++ = (FT_Byte)( num & 0xFF );
              }
            }
          }

          stack++;
        }
      }
#endif /* CFF_CONFIG_OPTION_OLD_ENGINE */
      else
      {
        /* This is not a number, hence it's an operator.  Compute its code */
         /* and look for it in our current list.                            */
 
         FT_UInt                   code;
        FT_UInt                   num_args;
         const CFF_Field_Handler*  field;
 
 
        if ( (FT_UInt)( parser->top - parser->stack ) >= parser->stackSize )
          goto Stack_Overflow;

        num_args     = (FT_UInt)( parser->top - parser->stack );
         *parser->top = p;
        code         = v;

         if ( v == 12 )
         {
           /* two byte operator */
          code = 0x100 | p[0];
        }
        code = code | parser->object_code;

        for ( field = CFF_FIELD_HANDLERS_GET; field->kind; field++ )
        {
          if ( field->code == (FT_Int)code )
          {
            /* we found our field's handler; read it */
            FT_Long   val;
            FT_Byte*  q = (FT_Byte*)parser->object + field->offset;


#ifdef FT_DEBUG_LEVEL_TRACE
            FT_TRACE4(( "  %s", field->id ));
#endif

            /* check that we have enough arguments -- except for */
            /* delta encoded arrays, which can be empty          */
            if ( field->kind != cff_kind_delta && num_args < 1 )
              goto Stack_Underflow;

            switch ( field->kind )
            {
            case cff_kind_bool:
            case cff_kind_string:
            case cff_kind_num:
              val = cff_parse_num( parser, parser->stack );
              goto Store_Number;

            case cff_kind_fixed:
              val = cff_parse_fixed( parser, parser->stack );
              goto Store_Number;

            case cff_kind_fixed_thousand:
              val = cff_parse_fixed_scaled( parser, parser->stack, 3 );

            Store_Number:
              switch ( field->size )
              {
              case (8 / FT_CHAR_BIT):
                *(FT_Byte*)q = (FT_Byte)val;
                break;

              case (16 / FT_CHAR_BIT):
                *(FT_Short*)q = (FT_Short)val;
                break;

              case (32 / FT_CHAR_BIT):
                *(FT_Int32*)q = (FT_Int)val;
                break;

              default:  /* for 64-bit systems */
                *(FT_Long*)q = val;
              }

#ifdef FT_DEBUG_LEVEL_TRACE
              switch ( field->kind )
              {
              case cff_kind_bool:
                FT_TRACE4(( " %s\n", val ? "true" : "false" ));
                break;

              case cff_kind_string:
                FT_TRACE4(( " %ld (SID)\n", val ));
                break;

              case cff_kind_num:
                FT_TRACE4(( " %ld\n", val ));
                break;

              case cff_kind_fixed:
                FT_TRACE4(( " %f\n", (double)val / 65536 ));
                break;

              case cff_kind_fixed_thousand:
                FT_TRACE4(( " %f\n", (double)val / 65536 / 1000 ));

              default:
                ; /* never reached */
              }
#endif

              break;

            case cff_kind_delta:
              {
                FT_Byte*   qcount = (FT_Byte*)parser->object +
                                      field->count_offset;

                FT_Byte**  data = parser->stack;


                if ( num_args > field->array_max )
                  num_args = field->array_max;

                FT_TRACE4(( " [" ));

                /* store count */
                *qcount = (FT_Byte)num_args;

                val = 0;
                while ( num_args > 0 )
                {
                  val += cff_parse_num( parser, data++ );
                  switch ( field->size )
                  {
                  case (8 / FT_CHAR_BIT):
                    *(FT_Byte*)q = (FT_Byte)val;
                    break;

                  case (16 / FT_CHAR_BIT):
                    *(FT_Short*)q = (FT_Short)val;
                    break;

                  case (32 / FT_CHAR_BIT):
                    *(FT_Int32*)q = (FT_Int)val;
                    break;

                  default:  /* for 64-bit systems */
                    *(FT_Long*)q = val;
                  }

                  FT_TRACE4(( " %ld", val ));

                  q += field->size;
                  num_args--;
                }

                FT_TRACE4(( "]\n" ));
              }
              break;

            default:  /* callback or blend */
              error = field->reader( parser );
              if ( error )
                goto Exit;
            }
            goto Found;
          }
        }

        /* this is an unknown operator, or it is unsupported; */
        /* we will ignore it for now.                         */

      Found:
        /* clear stack */
        /* TODO: could clear blend stack here,       */
        /*       but we don't have access to subFont */
        if ( field->kind != cff_kind_blend )
          parser->top = parser->stack;
      }
      p++;
    }

  Exit:
    return error;

  Stack_Overflow:
    error = FT_THROW( Invalid_Argument );
    goto Exit;

  Stack_Underflow:
    error = FT_THROW( Invalid_Argument );
    goto Exit;

  Syntax_Error:
    error = FT_THROW( Invalid_Argument );
    goto Exit;
  }
