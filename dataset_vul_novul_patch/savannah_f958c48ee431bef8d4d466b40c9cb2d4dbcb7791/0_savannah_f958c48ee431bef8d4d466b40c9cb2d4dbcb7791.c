  t1_decoder_parse_charstrings( T1_Decoder  decoder,
                                FT_Byte*    charstring_base,
                                FT_UInt     charstring_len )
  {
    FT_Error         error;
    T1_Decoder_Zone  zone;
    FT_Byte*         ip;
    FT_Byte*         limit;
    T1_Builder       builder = &decoder->builder;
    FT_Pos           x, y, orig_x, orig_y;
    FT_Int           known_othersubr_result_cnt   = 0;
    FT_Int           unknown_othersubr_result_cnt = 0;
    FT_Bool          large_int;
    FT_Fixed         seed;

    T1_Hints_Funcs   hinter;

#ifdef FT_DEBUG_LEVEL_TRACE
    FT_Bool          bol = TRUE;
#endif


    /* compute random seed from stack address of parameter */
    seed = (FT_Fixed)( ( (FT_Offset)(char*)&seed            ^
                         (FT_Offset)(char*)&decoder         ^
                         (FT_Offset)(char*)&charstring_base ) &
                         FT_ULONG_MAX                         );
    seed = ( seed ^ ( seed >> 10 ) ^ ( seed >> 20 ) ) & 0xFFFFL;
    if ( seed == 0 )
      seed = 0x7384;

    /* First of all, initialize the decoder */
    decoder->top  = decoder->stack;
    decoder->zone = decoder->zones;
    zone          = decoder->zones;

    builder->parse_state = T1_Parse_Start;

    hinter = (T1_Hints_Funcs)builder->hints_funcs;

    /* a font that reads BuildCharArray without setting */
    /* its values first is buggy, but ...               */
    FT_ASSERT( ( decoder->len_buildchar == 0 ) ==
               ( decoder->buildchar == NULL )  );

    if ( decoder->buildchar && decoder->len_buildchar > 0 )
      FT_ARRAY_ZERO( decoder->buildchar, decoder->len_buildchar );

    FT_TRACE4(( "\n"
                "Start charstring\n" ));

    zone->base           = charstring_base;
    limit = zone->limit  = charstring_base + charstring_len;
    ip    = zone->cursor = zone->base;

    error = FT_Err_Ok;

    x = orig_x = builder->pos_x;
    y = orig_y = builder->pos_y;

    /* begin hints recording session, if any */
    if ( hinter )
      hinter->open( hinter->hints );

    large_int = FALSE;

    /* now, execute loop */
    while ( ip < limit )
    {
      FT_Long*     top   = decoder->top;
      T1_Operator  op    = op_none;
      FT_Int32     value = 0;


      FT_ASSERT( known_othersubr_result_cnt == 0   ||
                 unknown_othersubr_result_cnt == 0 );

#ifdef FT_DEBUG_LEVEL_TRACE
      if ( bol )
      {
        FT_TRACE5(( " (%d)", decoder->top - decoder->stack ));
        bol = FALSE;
      }
#endif

      /*********************************************************************/
      /*                                                                   */
      /* Decode operator or operand                                        */
      /*                                                                   */
      /*                                                                   */

      /* first of all, decompress operator or value */
      switch ( *ip++ )
      {
      case 1:
        op = op_hstem;
        break;

      case 3:
        op = op_vstem;
        break;
      case 4:
        op = op_vmoveto;
        break;
      case 5:
        op = op_rlineto;
        break;
      case 6:
        op = op_hlineto;
        break;
      case 7:
        op = op_vlineto;
        break;
      case 8:
        op = op_rrcurveto;
        break;
      case 9:
        op = op_closepath;
        break;
      case 10:
        op = op_callsubr;
        break;
      case 11:
        op = op_return;
        break;

      case 13:
        op = op_hsbw;
        break;
      case 14:
        op = op_endchar;
        break;

      case 15:          /* undocumented, obsolete operator */
        op = op_unknown15;
        break;

      case 21:
        op = op_rmoveto;
        break;
      case 22:
        op = op_hmoveto;
        break;

      case 30:
        op = op_vhcurveto;
        break;
      case 31:
        op = op_hvcurveto;
        break;

      case 12:
        if ( ip >= limit )
        {
          FT_ERROR(( "t1_decoder_parse_charstrings:"
                     " invalid escape (12+EOF)\n" ));
          goto Syntax_Error;
        }

        switch ( *ip++ )
        {
        case 0:
          op = op_dotsection;
          break;
        case 1:
          op = op_vstem3;
          break;
        case 2:
          op = op_hstem3;
          break;
        case 6:
          op = op_seac;
          break;
        case 7:
          op = op_sbw;
          break;
        case 12:
          op = op_div;
          break;
        case 16:
          op = op_callothersubr;
          break;
        case 17:
          op = op_pop;
          break;
        case 33:
          op = op_setcurrentpoint;
          break;

        default:
          FT_ERROR(( "t1_decoder_parse_charstrings:"
                     " invalid escape (12+%d)\n",
                     ip[-1] ));
          goto Syntax_Error;
        }
        break;

      case 255:    /* four bytes integer */
        if ( ip + 4 > limit )
        {
          FT_ERROR(( "t1_decoder_parse_charstrings:"
                     " unexpected EOF in integer\n" ));
          goto Syntax_Error;
        }

        value = (FT_Int32)( ( (FT_UInt32)ip[0] << 24 ) |
                            ( (FT_UInt32)ip[1] << 16 ) |
                            ( (FT_UInt32)ip[2] << 8  ) |
                              (FT_UInt32)ip[3]         );
        ip += 4;

        /* According to the specification, values > 32000 or < -32000 must */
        /* be followed by a `div' operator to make the result be in the    */
        /* range [-32000;32000].  We expect that the second argument of    */
        /* `div' is not a large number.  Additionally, we don't handle     */
        /* stuff like `<large1> <large2> <num> div <num> div' or           */
        /* <large1> <large2> <num> div div'.  This is probably not allowed */
        /* anyway.                                                         */
        if ( value > 32000 || value < -32000 )
        {
          if ( large_int )
          {
            FT_ERROR(( "t1_decoder_parse_charstrings:"
                       " no `div' after large integer\n" ));
          }
          else
            large_int = TRUE;
        }
        else
        {
          if ( !large_int )
            value = (FT_Int32)( (FT_UInt32)value << 16 );
        }

        break;

      default:
        if ( ip[-1] >= 32 )
        {
          if ( ip[-1] < 247 )
            value = (FT_Int32)ip[-1] - 139;
          else
          {
            if ( ++ip > limit )
            {
              FT_ERROR(( "t1_decoder_parse_charstrings:"
                         " unexpected EOF in integer\n" ));
              goto Syntax_Error;
            }

            if ( ip[-2] < 251 )
              value =    ( ( ip[-2] - 247 ) * 256 ) + ip[-1] + 108;
            else
              value = -( ( ( ip[-2] - 251 ) * 256 ) + ip[-1] + 108 );
          }

          if ( !large_int )
            value = (FT_Int32)( (FT_UInt32)value << 16 );
        }
        else
        {
          FT_ERROR(( "t1_decoder_parse_charstrings:"
                     " invalid byte (%d)\n", ip[-1] ));
          goto Syntax_Error;
        }
      }

      if ( unknown_othersubr_result_cnt > 0 )
      {
        switch ( op )
        {
        case op_callsubr:
        case op_return:
        case op_none:
        case op_pop:
          break;

        default:
          /* all operands have been transferred by previous pops */
          unknown_othersubr_result_cnt = 0;
          break;
        }
      }

      if ( large_int && !( op == op_none || op == op_div ) )
      {
        FT_ERROR(( "t1_decoder_parse_charstrings:"
                   " no `div' after large integer\n" ));

        large_int = FALSE;
      }

      /*********************************************************************/
      /*                                                                   */
      /*  Push value on stack, or process operator                         */
      /*                                                                   */
      /*                                                                   */
      if ( op == op_none )
      {
        if ( top - decoder->stack >= T1_MAX_CHARSTRINGS_OPERANDS )
        {
          FT_ERROR(( "t1_decoder_parse_charstrings: stack overflow\n" ));
          goto Syntax_Error;
        }

#ifdef FT_DEBUG_LEVEL_TRACE
        if ( large_int )
          FT_TRACE4(( " %d", value ));
        else
          FT_TRACE4(( " %d", value / 65536 ));
#endif

        *top++       = value;
        decoder->top = top;
      }
      else if ( op == op_callothersubr )  /* callothersubr */
      {
        FT_Int  subr_no;
        FT_Int  arg_cnt;


#ifdef FT_DEBUG_LEVEL_TRACE
        FT_TRACE4(( " callothersubr\n" ));
        bol = TRUE;
#endif

        if ( top - decoder->stack < 2 )
          goto Stack_Underflow;

        top -= 2;

        subr_no = Fix2Int( top[1] );
        arg_cnt = Fix2Int( top[0] );

        /***********************************************************/
        /*                                                         */
        /* remove all operands to callothersubr from the stack     */
        /*                                                         */
        /* for handled othersubrs, where we know the number of     */
        /* arguments, we increase the stack by the value of        */
        /* known_othersubr_result_cnt                              */
        /*                                                         */
        /* for unhandled othersubrs the following pops adjust the  */
        /* stack pointer as necessary                              */

        if ( arg_cnt > top - decoder->stack )
          goto Stack_Underflow;

        top -= arg_cnt;

        known_othersubr_result_cnt   = 0;
        unknown_othersubr_result_cnt = 0;

        /* XXX TODO: The checks to `arg_count == <whatever>'       */
        /* might not be correct; an othersubr expects a certain    */
        /* number of operands on the PostScript stack (as opposed  */
        /* to the T1 stack) but it doesn't have to put them there  */
        /* by itself; previous othersubrs might have left the      */
        /* operands there if they were not followed by an          */
        /* appropriate number of pops                              */
        /*                                                         */
        /* On the other hand, Adobe Reader 7.0.8 for Linux doesn't */
        /* accept a font that contains charstrings like            */
        /*                                                         */
        /*     100 200 2 20 callothersubr                          */
        /*     300 1 20 callothersubr pop                          */
        /*                                                         */
        /* Perhaps this is the reason why BuildCharArray exists.   */

        switch ( subr_no )
        {
        case 0:                     /* end flex feature */
          if ( arg_cnt != 3 )
            goto Unexpected_OtherSubr;

          if ( !decoder->flex_state           ||
               decoder->num_flex_vectors != 7 )
          {
            FT_ERROR(( "t1_decoder_parse_charstrings:"
                       " unexpected flex end\n" ));
            goto Syntax_Error;
          }

          /* the two `results' are popped by the following setcurrentpoint */
          top[0] = x;
          top[1] = y;
          known_othersubr_result_cnt = 2;
          break;

        case 1:                     /* start flex feature */
          if ( arg_cnt != 0 )
            goto Unexpected_OtherSubr;

          if ( FT_SET_ERROR( t1_builder_start_point( builder, x, y ) ) ||
               FT_SET_ERROR( t1_builder_check_points( builder, 6 ) )   )
            goto Fail;

          decoder->flex_state        = 1;
          decoder->num_flex_vectors  = 0;
          break;

        case 2:                     /* add flex vectors */
          {
            FT_Int  idx;


            if ( arg_cnt != 0 )
              goto Unexpected_OtherSubr;

            if ( !decoder->flex_state )
            {
              FT_ERROR(( "t1_decoder_parse_charstrings:"
                         " missing flex start\n" ));
              goto Syntax_Error;
            }

            /* note that we should not add a point for index 0; */
            /* this will move our current position to the flex  */
             /* point without adding any point to the outline    */
             idx = decoder->num_flex_vectors++;
             if ( idx > 0 && idx < 7 )
            {
              /* in malformed fonts it is possible to have other */
              /* opcodes in the middle of a flex (which don't    */
              /* increase `num_flex_vectors'); we thus have to   */
              /* check whether we can add a point                */
              if ( FT_SET_ERROR( t1_builder_check_points( builder, 1 ) ) )
                goto Syntax_Error;

               t1_builder_add_point( builder,
                                     x,
                                     y,
                                     (FT_Byte)( idx == 3 || idx == 6 ) );
            }
           }
           break;
          break;

        case 12:
        case 13:
          /* counter control hints, clear stack */
          top = decoder->stack;
          break;

        case 14:
        case 15:
        case 16:
        case 17:
        case 18:                    /* multiple masters */
          {
            PS_Blend  blend = decoder->blend;
            FT_UInt   num_points, nn, mm;
            FT_Long*  delta;
            FT_Long*  values;


            if ( !blend )
            {
              FT_ERROR(( "t1_decoder_parse_charstrings:"
                         " unexpected multiple masters operator\n" ));
              goto Syntax_Error;
            }

            num_points = (FT_UInt)subr_no - 13 + ( subr_no == 18 );
            if ( arg_cnt != (FT_Int)( num_points * blend->num_designs ) )
            {
              FT_ERROR(( "t1_decoder_parse_charstrings:"
                         " incorrect number of multiple masters arguments\n" ));
              goto Syntax_Error;
            }

            /* We want to compute                                    */
            /*                                                       */
            /*   a0*w0 + a1*w1 + ... + ak*wk                         */
            /*                                                       */
            /* but we only have a0, a1-a0, a2-a0, ..., ak-a0.        */
            /*                                                       */
            /* However, given that w0 + w1 + ... + wk == 1, we can   */
            /* rewrite it easily as                                  */
            /*                                                       */
            /*   a0 + (a1-a0)*w1 + (a2-a0)*w2 + ... + (ak-a0)*wk     */
            /*                                                       */
            /* where k == num_designs-1.                             */
            /*                                                       */
            /* I guess that's why it's written in this `compact'     */
            /* form.                                                 */
            /*                                                       */
            delta  = top + num_points;
            values = top;
            for ( nn = 0; nn < num_points; nn++ )
            {
              FT_Long  tmp = values[0];


              for ( mm = 1; mm < blend->num_designs; mm++ )
                tmp += FT_MulFix( *delta++, blend->weight_vector[mm] );

              *values++ = tmp;
            }

            known_othersubr_result_cnt = (FT_Int)num_points;
            break;
          }

        case 19:
          /* <idx> 1 19 callothersubr                             */
          /* => replace elements starting from index cvi( <idx> ) */
          /*    of BuildCharArray with WeightVector               */
          {
            FT_Int    idx;
            PS_Blend  blend = decoder->blend;


            if ( arg_cnt != 1 || !blend )
              goto Unexpected_OtherSubr;

            idx = Fix2Int( top[0] );

            if ( idx < 0                                                    ||
                 (FT_UInt)idx + blend->num_designs > decoder->len_buildchar )
              goto Unexpected_OtherSubr;

            ft_memcpy( &decoder->buildchar[idx],
                       blend->weight_vector,
                       blend->num_designs *
                         sizeof ( blend->weight_vector[0] ) );
          }
          break;

        case 20:
          /* <arg1> <arg2> 2 20 callothersubr pop   */
          /* ==> push <arg1> + <arg2> onto T1 stack */
          if ( arg_cnt != 2 )
            goto Unexpected_OtherSubr;

          top[0] += top[1]; /* XXX (over|under)flow */

          known_othersubr_result_cnt = 1;
          break;

        case 21:
          /* <arg1> <arg2> 2 21 callothersubr pop   */
          /* ==> push <arg1> - <arg2> onto T1 stack */
          if ( arg_cnt != 2 )
            goto Unexpected_OtherSubr;

          top[0] -= top[1]; /* XXX (over|under)flow */

          known_othersubr_result_cnt = 1;
          break;

        case 22:
          /* <arg1> <arg2> 2 22 callothersubr pop   */
          /* ==> push <arg1> * <arg2> onto T1 stack */
          if ( arg_cnt != 2 )
            goto Unexpected_OtherSubr;

          top[0] = FT_MulFix( top[0], top[1] );

          known_othersubr_result_cnt = 1;
          break;

        case 23:
          /* <arg1> <arg2> 2 23 callothersubr pop   */
          /* ==> push <arg1> / <arg2> onto T1 stack */
          if ( arg_cnt != 2 || top[1] == 0 )
            goto Unexpected_OtherSubr;

          top[0] = FT_DivFix( top[0], top[1] );

          known_othersubr_result_cnt = 1;
          break;

        case 24:
          /* <val> <idx> 2 24 callothersubr               */
          /* ==> set BuildCharArray[cvi( <idx> )] = <val> */
          {
            FT_Int    idx;
            PS_Blend  blend = decoder->blend;


            if ( arg_cnt != 2 || !blend )
              goto Unexpected_OtherSubr;

            idx = Fix2Int( top[1] );

            if ( idx < 0 || (FT_UInt) idx >= decoder->len_buildchar )
              goto Unexpected_OtherSubr;

            decoder->buildchar[idx] = top[0];
          }
          break;

        case 25:
          /* <idx> 1 25 callothersubr pop        */
          /* ==> push BuildCharArray[cvi( idx )] */
          /*     onto T1 stack                   */
          {
            FT_Int    idx;
            PS_Blend  blend = decoder->blend;


            if ( arg_cnt != 1 || !blend )
              goto Unexpected_OtherSubr;

            idx = Fix2Int( top[0] );

            if ( idx < 0 || (FT_UInt) idx >= decoder->len_buildchar )
              goto Unexpected_OtherSubr;

            top[0] = decoder->buildchar[idx];
          }

          known_othersubr_result_cnt = 1;
          break;

#if 0
        case 26:
          /* <val> mark <idx> ==> set BuildCharArray[cvi( <idx> )] = <val>, */
          /*                      leave mark on T1 stack                    */
          /* <val> <idx>      ==> set BuildCharArray[cvi( <idx> )] = <val>  */
          XXX which routine has left its mark on the (PostScript) stack?;
          break;
#endif

        case 27:
          /* <res1> <res2> <val1> <val2> 4 27 callothersubr pop */
          /* ==> push <res1> onto T1 stack if <val1> <= <val2>, */
          /*     otherwise push <res2>                          */
          if ( arg_cnt != 4 )
            goto Unexpected_OtherSubr;

          if ( top[2] > top[3] )
            top[0] = top[1];

          known_othersubr_result_cnt = 1;
          break;

        case 28:
          /* 0 28 callothersubr pop                               */
          /* => push random value from interval [0, 1) onto stack */
          if ( arg_cnt != 0 )
            goto Unexpected_OtherSubr;

          {
            FT_Fixed  Rand;


            Rand = seed;
            if ( Rand >= 0x8000L )
              Rand++;

            top[0] = Rand;

            seed = FT_MulFix( seed, 0x10000L - seed );
            if ( seed == 0 )
              seed += 0x2873;
          }

          known_othersubr_result_cnt = 1;
          break;

        default:
          if ( arg_cnt >= 0 && subr_no >= 0 )
          {
            FT_ERROR(( "t1_decoder_parse_charstrings:"
                       " unknown othersubr [%d %d], wish me luck\n",
                       arg_cnt, subr_no ));
            unknown_othersubr_result_cnt = arg_cnt;
            break;
          }
          /* fall through */

        Unexpected_OtherSubr:
          FT_ERROR(( "t1_decoder_parse_charstrings:"
                     " invalid othersubr [%d %d]\n", arg_cnt, subr_no ));
          goto Syntax_Error;
        }

        top += known_othersubr_result_cnt;

        decoder->top = top;
      }
      else  /* general operator */
      {
        FT_Int  num_args = t1_args_count[op];


        FT_ASSERT( num_args >= 0 );

        if ( top - decoder->stack < num_args )
          goto Stack_Underflow;

        /* XXX Operators usually take their operands from the        */
        /*     bottom of the stack, i.e., the operands are           */
        /*     decoder->stack[0], ..., decoder->stack[num_args - 1]; */
        /*     only div, callsubr, and callothersubr are different.  */
        /*     In practice it doesn't matter (?).                    */

#ifdef FT_DEBUG_LEVEL_TRACE

        switch ( op )
        {
        case op_callsubr:
        case op_div:
        case op_callothersubr:
        case op_pop:
        case op_return:
          break;

        default:
          if ( top - decoder->stack != num_args )
            FT_TRACE0(( "t1_decoder_parse_charstrings:"
                        " too much operands on the stack"
                        " (seen %d, expected %d)\n",
                        top - decoder->stack, num_args ));
            break;
        }

#endif /* FT_DEBUG_LEVEL_TRACE */

        top -= num_args;

        switch ( op )
        {
        case op_endchar:
          FT_TRACE4(( " endchar\n" ));

          t1_builder_close_contour( builder );

          /* close hints recording session */
          if ( hinter )
          {
            if ( hinter->close( hinter->hints,
                                (FT_UInt)builder->current->n_points ) )
              goto Syntax_Error;

            /* apply hints to the loaded glyph outline now */
            error = hinter->apply( hinter->hints,
                                   builder->current,
                                   (PSH_Globals)builder->hints_globals,
                                   decoder->hint_mode );
            if ( error )
              goto Fail;
          }

          /* add current outline to the glyph slot */
          FT_GlyphLoader_Add( builder->loader );

          /* the compiler should optimize away this empty loop but ... */

#ifdef FT_DEBUG_LEVEL_TRACE

          if ( decoder->len_buildchar > 0 )
          {
            FT_UInt  i;


            FT_TRACE4(( "BuildCharArray = [ " ));

            for ( i = 0; i < decoder->len_buildchar; i++ )
              FT_TRACE4(( "%d ", decoder->buildchar[i] ));

            FT_TRACE4(( "]\n" ));
          }

#endif /* FT_DEBUG_LEVEL_TRACE */

          FT_TRACE4(( "\n" ));

          /* return now! */
          return FT_Err_Ok;

        case op_hsbw:
          FT_TRACE4(( " hsbw" ));

          builder->parse_state = T1_Parse_Have_Width;

          builder->left_bearing.x += top[0];
          builder->advance.x       = top[1];
          builder->advance.y       = 0;

          orig_x = x = builder->pos_x + top[0];
          orig_y = y = builder->pos_y;

          FT_UNUSED( orig_y );

          /* the `metrics_only' indicates that we only want to compute */
          /* the glyph's metrics (lsb + advance width), not load the   */
          /* rest of it; so exit immediately                           */
          if ( builder->metrics_only )
            return FT_Err_Ok;

          break;

        case op_seac:
          return t1operator_seac( decoder,
                                  top[0],
                                  top[1],
                                  top[2],
                                  Fix2Int( top[3] ),
                                  Fix2Int( top[4] ) );

        case op_sbw:
          FT_TRACE4(( " sbw" ));

          builder->parse_state = T1_Parse_Have_Width;

          builder->left_bearing.x += top[0];
          builder->left_bearing.y += top[1];
          builder->advance.x       = top[2];
          builder->advance.y       = top[3];

          x = builder->pos_x + top[0];
          y = builder->pos_y + top[1];

          /* the `metrics_only' indicates that we only want to compute */
          /* the glyph's metrics (lsb + advance width), not load the   */
          /* rest of it; so exit immediately                           */
          if ( builder->metrics_only )
            return FT_Err_Ok;

          break;

        case op_closepath:
          FT_TRACE4(( " closepath" ));

          /* if there is no path, `closepath' is a no-op */
          if ( builder->parse_state == T1_Parse_Have_Path   ||
               builder->parse_state == T1_Parse_Have_Moveto )
            t1_builder_close_contour( builder );

          builder->parse_state = T1_Parse_Have_Width;
          break;

        case op_hlineto:
          FT_TRACE4(( " hlineto" ));

          if ( FT_SET_ERROR( t1_builder_start_point( builder, x, y ) ) )
            goto Fail;

          x += top[0];
          goto Add_Line;

        case op_hmoveto:
          FT_TRACE4(( " hmoveto" ));

          x += top[0];
          if ( !decoder->flex_state )
          {
            if ( builder->parse_state == T1_Parse_Start )
              goto Syntax_Error;
            builder->parse_state = T1_Parse_Have_Moveto;
          }
          break;

        case op_hvcurveto:
          FT_TRACE4(( " hvcurveto" ));

          if ( FT_SET_ERROR( t1_builder_start_point( builder, x, y ) ) ||
               FT_SET_ERROR( t1_builder_check_points( builder, 3 ) )   )
            goto Fail;

          x += top[0];
          t1_builder_add_point( builder, x, y, 0 );
          x += top[1];
          y += top[2];
          t1_builder_add_point( builder, x, y, 0 );
          y += top[3];
          t1_builder_add_point( builder, x, y, 1 );
          break;

        case op_rlineto:
          FT_TRACE4(( " rlineto" ));

          if ( FT_SET_ERROR( t1_builder_start_point( builder, x, y ) ) )
            goto Fail;

          x += top[0];
          y += top[1];

        Add_Line:
          if ( FT_SET_ERROR( t1_builder_add_point1( builder, x, y ) ) )
            goto Fail;
          break;

        case op_rmoveto:
          FT_TRACE4(( " rmoveto" ));

          x += top[0];
          y += top[1];
          if ( !decoder->flex_state )
          {
            if ( builder->parse_state == T1_Parse_Start )
              goto Syntax_Error;
            builder->parse_state = T1_Parse_Have_Moveto;
          }
          break;

        case op_rrcurveto:
          FT_TRACE4(( " rrcurveto" ));

          if ( FT_SET_ERROR( t1_builder_start_point( builder, x, y ) ) ||
               FT_SET_ERROR( t1_builder_check_points( builder, 3 ) )   )
            goto Fail;

          x += top[0];
          y += top[1];
          t1_builder_add_point( builder, x, y, 0 );

          x += top[2];
          y += top[3];
          t1_builder_add_point( builder, x, y, 0 );

          x += top[4];
          y += top[5];
          t1_builder_add_point( builder, x, y, 1 );
          break;

        case op_vhcurveto:
          FT_TRACE4(( " vhcurveto" ));

          if ( FT_SET_ERROR( t1_builder_start_point( builder, x, y ) ) ||
               FT_SET_ERROR( t1_builder_check_points( builder, 3 ) )   )
            goto Fail;

          y += top[0];
          t1_builder_add_point( builder, x, y, 0 );
          x += top[1];
          y += top[2];
          t1_builder_add_point( builder, x, y, 0 );
          x += top[3];
          t1_builder_add_point( builder, x, y, 1 );
          break;

        case op_vlineto:
          FT_TRACE4(( " vlineto" ));

          if ( FT_SET_ERROR( t1_builder_start_point( builder, x, y ) ) )
            goto Fail;

          y += top[0];
          goto Add_Line;

        case op_vmoveto:
          FT_TRACE4(( " vmoveto" ));

          y += top[0];
          if ( !decoder->flex_state )
          {
            if ( builder->parse_state == T1_Parse_Start )
              goto Syntax_Error;
            builder->parse_state = T1_Parse_Have_Moveto;
          }
          break;

        case op_div:
          FT_TRACE4(( " div" ));

          /* if `large_int' is set, we divide unscaled numbers; */
          /* otherwise, we divide numbers in 16.16 format --    */
          /* in both cases, it is the same operation            */
          *top = FT_DivFix( top[0], top[1] );
          top++;

          large_int = FALSE;
          break;

        case op_callsubr:
          {
            FT_Int  idx;


            FT_TRACE4(( " callsubr" ));

            idx = Fix2Int( top[0] );

            if ( decoder->subrs_hash )
            {
              size_t*  val = ft_hash_num_lookup( idx,
                                                 decoder->subrs_hash );


              if ( val )
                idx = *val;
              else
                idx = -1;
            }

            if ( idx < 0 || idx >= decoder->num_subrs )
            {
              FT_ERROR(( "t1_decoder_parse_charstrings:"
                         " invalid subrs index\n" ));
              goto Syntax_Error;
            }

            if ( zone - decoder->zones >= T1_MAX_SUBRS_CALLS )
            {
              FT_ERROR(( "t1_decoder_parse_charstrings:"
                         " too many nested subrs\n" ));
              goto Syntax_Error;
            }

            zone->cursor = ip;  /* save current instruction pointer */

            zone++;

            /* The Type 1 driver stores subroutines without the seed bytes. */
            /* The CID driver stores subroutines with seed bytes.  This     */
            /* case is taken care of when decoder->subrs_len == 0.          */
            zone->base = decoder->subrs[idx];

            if ( decoder->subrs_len )
              zone->limit = zone->base + decoder->subrs_len[idx];
            else
            {
              /* We are using subroutines from a CID font.  We must adjust */
              /* for the seed bytes.                                       */
              zone->base  += ( decoder->lenIV >= 0 ? decoder->lenIV : 0 );
              zone->limit  = decoder->subrs[idx + 1];
            }

            zone->cursor = zone->base;

            if ( !zone->base )
            {
              FT_ERROR(( "t1_decoder_parse_charstrings:"
                         " invoking empty subrs\n" ));
              goto Syntax_Error;
            }

            decoder->zone = zone;
            ip            = zone->base;
            limit         = zone->limit;
            break;
          }

        case op_pop:
          FT_TRACE4(( " pop" ));

          if ( known_othersubr_result_cnt > 0 )
          {
            known_othersubr_result_cnt--;
            /* ignore, we pushed the operands ourselves */
            break;
          }

          if ( unknown_othersubr_result_cnt == 0 )
          {
            FT_ERROR(( "t1_decoder_parse_charstrings:"
                       " no more operands for othersubr\n" ));
            goto Syntax_Error;
          }

          unknown_othersubr_result_cnt--;
          top++;   /* `push' the operand to callothersubr onto the stack */
          break;

        case op_return:
          FT_TRACE4(( " return" ));

          if ( zone <= decoder->zones )
          {
            FT_ERROR(( "t1_decoder_parse_charstrings:"
                       " unexpected return\n" ));
            goto Syntax_Error;
          }

          zone--;
          ip            = zone->cursor;
          limit         = zone->limit;
          decoder->zone = zone;
          break;

        case op_dotsection:
          FT_TRACE4(( " dotsection" ));

          break;

        case op_hstem:
          FT_TRACE4(( " hstem" ));

          /* record horizontal hint */
          if ( hinter )
          {
            /* top[0] += builder->left_bearing.y; */
            hinter->stem( hinter->hints, 1, top );
          }
          break;

        case op_hstem3:
          FT_TRACE4(( " hstem3" ));

          /* record horizontal counter-controlled hints */
          if ( hinter )
            hinter->stem3( hinter->hints, 1, top );
          break;

        case op_vstem:
          FT_TRACE4(( " vstem" ));

          /* record vertical hint */
          if ( hinter )
          {
            top[0] += orig_x;
            hinter->stem( hinter->hints, 0, top );
          }
          break;

        case op_vstem3:
          FT_TRACE4(( " vstem3" ));

          /* record vertical counter-controlled hints */
          if ( hinter )
          {
            FT_Pos  dx = orig_x;


            top[0] += dx;
            top[2] += dx;
            top[4] += dx;
            hinter->stem3( hinter->hints, 0, top );
          }
          break;

        case op_setcurrentpoint:
          FT_TRACE4(( " setcurrentpoint" ));

          /* From the T1 specification, section 6.4:                */
          /*                                                        */
          /*   The setcurrentpoint command is used only in          */
          /*   conjunction with results from OtherSubrs procedures. */

          /* known_othersubr_result_cnt != 0 is already handled     */
          /* above.                                                 */

          /* Note, however, that both Ghostscript and Adobe         */
          /* Distiller handle this situation by silently ignoring   */
          /* the inappropriate `setcurrentpoint' instruction.  So   */
          /* we do the same.                                        */
#if 0

          if ( decoder->flex_state != 1 )
          {
            FT_ERROR(( "t1_decoder_parse_charstrings:"
                       " unexpected `setcurrentpoint'\n" ));
            goto Syntax_Error;
          }
          else
            ...
#endif

          x = top[0];
          y = top[1];
          decoder->flex_state = 0;
          break;

        case op_unknown15:
          FT_TRACE4(( " opcode_15" ));
          /* nothing to do except to pop the two arguments */
          break;

        default:
          FT_ERROR(( "t1_decoder_parse_charstrings:"
                     " unhandled opcode %d\n", op ));
          goto Syntax_Error;
        }

        /* XXX Operators usually clear the operand stack;  */
        /*     only div, callsubr, callothersubr, pop, and */
        /*     return are different.                       */
        /*     In practice it doesn't matter (?).          */

        decoder->top = top;

#ifdef FT_DEBUG_LEVEL_TRACE
        FT_TRACE4(( "\n" ));
        bol = TRUE;
#endif

      } /* general operator processing */

    } /* while ip < limit */

    FT_TRACE4(( "..end..\n\n" ));

  Fail:
    return error;

  Syntax_Error:
    return FT_THROW( Syntax_Error );

  Stack_Underflow:
    return FT_THROW( Stack_Underflow );
  }
