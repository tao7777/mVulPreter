  cff_decoder_parse_charstrings( CFF_Decoder*  decoder,
                                 FT_Byte*      charstring_base,
                                 FT_ULong      charstring_len )
  {
    FT_Error           error;
    CFF_Decoder_Zone*  zone;
    FT_Byte*           ip;
    FT_Byte*           limit;
    CFF_Builder*       builder = &decoder->builder;
    FT_Pos             x, y;
    FT_Fixed           seed;
    FT_Fixed*          stack;
    FT_Int             charstring_type =
                         decoder->cff->top_font.font_dict.charstring_type;

    T2_Hints_Funcs     hinter;


    /* set default width */
    decoder->num_hints  = 0;
    decoder->read_width = 1;

    /* compute random seed from stack address of parameter */
    seed = (FT_Fixed)( ( (FT_PtrDist)(char*)&seed              ^
                         (FT_PtrDist)(char*)&decoder           ^
                         (FT_PtrDist)(char*)&charstring_base ) &
                         FT_ULONG_MAX ) ;
    seed = ( seed ^ ( seed >> 10 ) ^ ( seed >> 20 ) ) & 0xFFFFL;
    if ( seed == 0 )
      seed = 0x7384;

    /* initialize the decoder */
    decoder->top  = decoder->stack;
    decoder->zone = decoder->zones;
    zone          = decoder->zones;
    stack         = decoder->top;

    hinter = (T2_Hints_Funcs)builder->hints_funcs;

    builder->path_begun = 0;

    zone->base           = charstring_base;
    limit = zone->limit  = charstring_base + charstring_len;
    ip    = zone->cursor = zone->base;

    error = CFF_Err_Ok;

    x = builder->pos_x;
    y = builder->pos_y;

    /* begin hints recording session, if any */
    if ( hinter )
      hinter->open( hinter->hints );

    /* now execute loop */
    while ( ip < limit )
    {
      CFF_Operator  op;
      FT_Byte       v;


      /********************************************************************/
      /*                                                                  */
      /* Decode operator or operand                                       */
      /*                                                                  */
      v = *ip++;
      if ( v >= 32 || v == 28 )
      {
        FT_Int    shift = 16;
        FT_Int32  val;


        /* this is an operand, push it on the stack */
        if ( v == 28 )
        {
          if ( ip + 1 >= limit )
            goto Syntax_Error;
          val = (FT_Short)( ( (FT_Short)ip[0] << 8 ) | ip[1] );
          ip += 2;
        }
        else if ( v < 247 )
          val = (FT_Int32)v - 139;
        else if ( v < 251 )
        {
          if ( ip >= limit )
            goto Syntax_Error;
          val = ( (FT_Int32)v - 247 ) * 256 + *ip++ + 108;
        }
        else if ( v < 255 )
        {
          if ( ip >= limit )
            goto Syntax_Error;
          val = -( (FT_Int32)v - 251 ) * 256 - *ip++ - 108;
        }
        else
        {
          if ( ip + 3 >= limit )
            goto Syntax_Error;
          val = ( (FT_Int32)ip[0] << 24 ) |
                ( (FT_Int32)ip[1] << 16 ) |
                ( (FT_Int32)ip[2] <<  8 ) |
                            ip[3];
          ip    += 4;
          if ( charstring_type == 2 )
            shift = 0;
        }
        if ( decoder->top - stack >= CFF_MAX_OPERANDS )
          goto Stack_Overflow;

        val           <<= shift;
        *decoder->top++ = val;

#ifdef FT_DEBUG_LEVEL_TRACE
        if ( !( val & 0xFFFFL ) )
          FT_TRACE4(( " %ld", (FT_Int32)( val >> 16 ) ));
        else
          FT_TRACE4(( " %.2f", val / 65536.0 ));
#endif

      }
      else
      {
        /* The specification says that normally arguments are to be taken */
        /* from the bottom of the stack.  However, this seems not to be   */
        /* correct, at least for Acroread 7.0.8 on GNU/Linux: It pops the */
        /* arguments similar to a PS interpreter.                         */

        FT_Fixed*  args     = decoder->top;
        FT_Int     num_args = (FT_Int)( args - decoder->stack );
        FT_Int     req_args;


        /* find operator */
        op = cff_op_unknown;

        switch ( v )
        {
        case 1:
          op = cff_op_hstem;
          break;
        case 3:
          op = cff_op_vstem;
          break;
        case 4:
          op = cff_op_vmoveto;
          break;
        case 5:
          op = cff_op_rlineto;
          break;
        case 6:
          op = cff_op_hlineto;
          break;
        case 7:
          op = cff_op_vlineto;
          break;
        case 8:
          op = cff_op_rrcurveto;
          break;
        case 9:
          op = cff_op_closepath;
          break;
        case 10:
          op = cff_op_callsubr;
          break;
        case 11:
          op = cff_op_return;
          break;
        case 12:
          {
            if ( ip >= limit )
              goto Syntax_Error;
            v = *ip++;

            switch ( v )
            {
            case 0:
              op = cff_op_dotsection;
              break;
            case 1: /* this is actually the Type1 vstem3 operator */
              op = cff_op_vstem;
              break;
            case 2: /* this is actually the Type1 hstem3 operator */
              op = cff_op_hstem;
              break;
            case 3:
              op = cff_op_and;
              break;
            case 4:
              op = cff_op_or;
              break;
            case 5:
              op = cff_op_not;
              break;
            case 6:
              op = cff_op_seac;
              break;
            case 7:
              op = cff_op_sbw;
              break;
            case 8:
              op = cff_op_store;
              break;
            case 9:
              op = cff_op_abs;
              break;
            case 10:
              op = cff_op_add;
              break;
            case 11:
              op = cff_op_sub;
              break;
            case 12:
              op = cff_op_div;
              break;
            case 13:
              op = cff_op_load;
              break;
            case 14:
              op = cff_op_neg;
              break;
            case 15:
              op = cff_op_eq;
              break;
            case 16:
              op = cff_op_callothersubr;
              break;
            case 17:
              op = cff_op_pop;
              break;
            case 18:
              op = cff_op_drop;
              break;
            case 20:
              op = cff_op_put;
              break;
            case 21:
              op = cff_op_get;
              break;
            case 22:
              op = cff_op_ifelse;
              break;
            case 23:
              op = cff_op_random;
              break;
            case 24:
              op = cff_op_mul;
              break;
            case 26:
              op = cff_op_sqrt;
              break;
            case 27:
              op = cff_op_dup;
              break;
            case 28:
              op = cff_op_exch;
              break;
            case 29:
              op = cff_op_index;
              break;
            case 30:
              op = cff_op_roll;
              break;
            case 33:
              op = cff_op_setcurrentpoint;
              break;
            case 34:
              op = cff_op_hflex;
              break;
            case 35:
              op = cff_op_flex;
              break;
            case 36:
              op = cff_op_hflex1;
              break;
            case 37:
              op = cff_op_flex1;
              break;
            default:
              /* decrement ip for syntax error message */
              ip--;
            }
          }
          break;
        case 13:
          op = cff_op_hsbw;
          break;
        case 14:
          op = cff_op_endchar;
          break;
        case 16:
          op = cff_op_blend;
          break;
        case 18:
          op = cff_op_hstemhm;
          break;
        case 19:
          op = cff_op_hintmask;
          break;
        case 20:
          op = cff_op_cntrmask;
          break;
        case 21:
          op = cff_op_rmoveto;
          break;
        case 22:
          op = cff_op_hmoveto;
          break;
        case 23:
          op = cff_op_vstemhm;
          break;
        case 24:
          op = cff_op_rcurveline;
          break;
        case 25:
          op = cff_op_rlinecurve;
          break;
        case 26:
          op = cff_op_vvcurveto;
          break;
        case 27:
          op = cff_op_hhcurveto;
          break;
        case 29:
          op = cff_op_callgsubr;
          break;
        case 30:
          op = cff_op_vhcurveto;
          break;
        case 31:
          op = cff_op_hvcurveto;
          break;
        default:
          break;
        }

        if ( op == cff_op_unknown )
          goto Syntax_Error;

        /* check arguments */
        req_args = cff_argument_counts[op];
        if ( req_args & CFF_COUNT_CHECK_WIDTH )
        {
          if ( num_args > 0 && decoder->read_width )
          {
            /* If `nominal_width' is non-zero, the number is really a      */
            /* difference against `nominal_width'.  Else, the number here  */
            /* is truly a width, not a difference against `nominal_width'. */
            /* If the font does not set `nominal_width', then              */
            /* `nominal_width' defaults to zero, and so we can set         */
            /* `glyph_width' to `nominal_width' plus number on the stack   */
            /* -- for either case.                                         */

            FT_Int  set_width_ok;


            switch ( op )
            {
            case cff_op_hmoveto:
            case cff_op_vmoveto:
              set_width_ok = num_args & 2;
              break;

            case cff_op_hstem:
            case cff_op_vstem:
            case cff_op_hstemhm:
            case cff_op_vstemhm:
            case cff_op_rmoveto:
            case cff_op_hintmask:
            case cff_op_cntrmask:
              set_width_ok = num_args & 1;
              break;

            case cff_op_endchar:
              /* If there is a width specified for endchar, we either have */
              /* 1 argument or 5 arguments.  We like to argue.             */
              set_width_ok = ( num_args == 5 ) || ( num_args == 1 );
              break;

            default:
              set_width_ok = 0;
              break;
            }

            if ( set_width_ok )
            {
              decoder->glyph_width = decoder->nominal_width +
                                       ( stack[0] >> 16 );

              if ( decoder->width_only )
              {
                /* we only want the advance width; stop here */
                break;
              }

              /* Consumed an argument. */
              num_args--;
            }
          }

          decoder->read_width = 0;
          req_args            = 0;
        }

        req_args &= 0x000F;
        if ( num_args < req_args )
          goto Stack_Underflow;
        args     -= req_args;
        num_args -= req_args;

        /* At this point, `args' points to the first argument of the  */
        /* operand in case `req_args' isn't zero.  Otherwise, we have */
        /* to adjust `args' manually.                                 */

        /* Note that we only pop arguments from the stack which we    */
        /* really need and can digest so that we can continue in case */
        /* of superfluous stack elements.                             */

        switch ( op )
        {
        case cff_op_hstem:
        case cff_op_vstem:
        case cff_op_hstemhm:
        case cff_op_vstemhm:
          /* the number of arguments is always even here */
          FT_TRACE4((
              op == cff_op_hstem   ? " hstem\n"   :
            ( op == cff_op_vstem   ? " vstem\n"   :
            ( op == cff_op_hstemhm ? " hstemhm\n" : " vstemhm\n" ) ) ));

          if ( hinter )
            hinter->stems( hinter->hints,
                           ( op == cff_op_hstem || op == cff_op_hstemhm ),
                           num_args / 2,
                           args - ( num_args & ~1 ) );

          decoder->num_hints += num_args / 2;
          args = stack;
          break;

        case cff_op_hintmask:
        case cff_op_cntrmask:
          FT_TRACE4(( op == cff_op_hintmask ? " hintmask" : " cntrmask" ));

          /* implement vstem when needed --                        */
          /* the specification doesn't say it, but this also works */
          /* with the 'cntrmask' operator                          */
          /*                                                       */
          if ( num_args > 0 )
          {
            if ( hinter )
              hinter->stems( hinter->hints,
                             0,
                             num_args / 2,
                             args - ( num_args & ~1 ) );

            decoder->num_hints += num_args / 2;
          }

          if ( hinter )
          {
            if ( op == cff_op_hintmask )
              hinter->hintmask( hinter->hints,
                                builder->current->n_points,
                                decoder->num_hints,
                                ip );
            else
              hinter->counter( hinter->hints,
                               decoder->num_hints,
                               ip );
          }

#ifdef FT_DEBUG_LEVEL_TRACE
          {
            FT_UInt maskbyte;


            FT_TRACE4(( " (maskbytes: " ));

            for ( maskbyte = 0;
                  maskbyte < (FT_UInt)(( decoder->num_hints + 7 ) >> 3);
                  maskbyte++, ip++ )
              FT_TRACE4(( "0x%02X", *ip ));

            FT_TRACE4(( ")\n" ));
          }
#else
          ip += ( decoder->num_hints + 7 ) >> 3;
#endif
          if ( ip >= limit )
            goto Syntax_Error;
          args = stack;
          break;

        case cff_op_rmoveto:
          FT_TRACE4(( " rmoveto\n" ));

          cff_builder_close_contour( builder );
          builder->path_begun = 0;
          x   += args[-2];
          y   += args[-1];
          args = stack;
          break;

        case cff_op_vmoveto:
          FT_TRACE4(( " vmoveto\n" ));

          cff_builder_close_contour( builder );
          builder->path_begun = 0;
          y   += args[-1];
          args = stack;
          break;

        case cff_op_hmoveto:
          FT_TRACE4(( " hmoveto\n" ));

          cff_builder_close_contour( builder );
          builder->path_begun = 0;
          x   += args[-1];
          args = stack;
          break;

        case cff_op_rlineto:
          FT_TRACE4(( " rlineto\n" ));

          if ( cff_builder_start_point ( builder, x, y ) ||
               check_points( builder, num_args / 2 )     )
            goto Fail;

          if ( num_args < 2 )
            goto Stack_Underflow;

          args -= num_args & ~1;
          while ( args < decoder->top )
          {
            x += args[0];
            y += args[1];
            cff_builder_add_point( builder, x, y, 1 );
            args += 2;
          }
          args = stack;
          break;

        case cff_op_hlineto:
        case cff_op_vlineto:
          {
            FT_Int  phase = ( op == cff_op_hlineto );


            FT_TRACE4(( op == cff_op_hlineto ? " hlineto\n"
                                             : " vlineto\n" ));

            if ( num_args < 1 )
              goto Stack_Underflow;

            if ( cff_builder_start_point ( builder, x, y ) ||
                 check_points( builder, num_args )         )
              goto Fail;

            args = stack;
            while ( args < decoder->top )
            {
              if ( phase )
                x += args[0];
              else
                y += args[0];

              if ( cff_builder_add_point1( builder, x, y ) )
                goto Fail;

              args++;
              phase ^= 1;
            }
            args = stack;
          }
          break;

        case cff_op_rrcurveto:
          {
            FT_Int  nargs;


            FT_TRACE4(( " rrcurveto\n" ));

            if ( num_args < 6 )
              goto Stack_Underflow;

            nargs = num_args - num_args % 6;

            if ( cff_builder_start_point ( builder, x, y ) ||
                 check_points( builder, nargs / 2 )     )
              goto Fail;

            args -= nargs;
            while ( args < decoder->top )
            {
              x += args[0];
              y += args[1];
              cff_builder_add_point( builder, x, y, 0 );
              x += args[2];
              y += args[3];
              cff_builder_add_point( builder, x, y, 0 );
              x += args[4];
              y += args[5];
              cff_builder_add_point( builder, x, y, 1 );
              args += 6;
            }
            args = stack;
          }
          break;

        case cff_op_vvcurveto:
          {
            FT_Int  nargs;


            FT_TRACE4(( " vvcurveto\n" ));

            if ( num_args < 4 )
              goto Stack_Underflow;

            /* if num_args isn't of the form 4n or 4n+1, */
            /* we reduce it to 4n+1                      */

            nargs = num_args - num_args % 4;
            if ( num_args - nargs > 0 )
              nargs += 1;

            if ( cff_builder_start_point( builder, x, y ) )
              goto Fail;

            args -= nargs;

            if ( nargs & 1 )
            {
              x += args[0];
              args++;
              nargs--;
            }

            if ( check_points( builder, 3 * ( nargs / 4 ) ) )
              goto Fail;

            while ( args < decoder->top )
            {
              y += args[0];
              cff_builder_add_point( builder, x, y, 0 );
              x += args[1];
              y += args[2];
              cff_builder_add_point( builder, x, y, 0 );
              y += args[3];
              cff_builder_add_point( builder, x, y, 1 );
              args += 4;
            }
            args = stack;
          }
          break;

        case cff_op_hhcurveto:
          {
            FT_Int  nargs;


            FT_TRACE4(( " hhcurveto\n" ));

            if ( num_args < 4 )
              goto Stack_Underflow;

            /* if num_args isn't of the form 4n or 4n+1, */
            /* we reduce it to 4n+1                      */

            nargs = num_args - num_args % 4;
            if ( num_args - nargs > 0 )
              nargs += 1;

            if ( cff_builder_start_point( builder, x, y ) )
              goto Fail;

            args -= nargs;
            if ( nargs & 1 )
            {
              y += args[0];
              args++;
              nargs--;
            }

            if ( check_points( builder, 3 * ( nargs / 4 ) ) )
              goto Fail;

            while ( args < decoder->top )
            {
              x += args[0];
              cff_builder_add_point( builder, x, y, 0 );
              x += args[1];
              y += args[2];
              cff_builder_add_point( builder, x, y, 0 );
              x += args[3];
              cff_builder_add_point( builder, x, y, 1 );
              args += 4;
            }
            args = stack;
          }
          break;

        case cff_op_vhcurveto:
        case cff_op_hvcurveto:
          {
            FT_Int  phase;
            FT_Int  nargs;


            FT_TRACE4(( op == cff_op_vhcurveto ? " vhcurveto\n"
                                               : " hvcurveto\n" ));

            if ( cff_builder_start_point( builder, x, y ) )
              goto Fail;

            if ( num_args < 4 )
              goto Stack_Underflow;

            /* if num_args isn't of the form 8n, 8n+1, 8n+4, or 8n+5, */
            /* we reduce it to the largest one which fits             */

            nargs = num_args - num_args % 4;
            if ( num_args - nargs > 0 )
              nargs += 1;

            args -= nargs;
            if ( check_points( builder, ( nargs / 4 ) * 3 ) )
              goto Stack_Underflow;

            phase = ( op == cff_op_hvcurveto );

            while ( nargs >= 4 )
            {
              nargs -= 4;
              if ( phase )
              {
                x += args[0];
                cff_builder_add_point( builder, x, y, 0 );
                x += args[1];
                y += args[2];
                cff_builder_add_point( builder, x, y, 0 );
                y += args[3];
                if ( nargs == 1 )
                  x += args[4];
                cff_builder_add_point( builder, x, y, 1 );
              }
              else
              {
                y += args[0];
                cff_builder_add_point( builder, x, y, 0 );
                x += args[1];
                y += args[2];
                cff_builder_add_point( builder, x, y, 0 );
                x += args[3];
                if ( nargs == 1 )
                  y += args[4];
                cff_builder_add_point( builder, x, y, 1 );
              }
              args  += 4;
              phase ^= 1;
            }
            args = stack;
          }
          break;

        case cff_op_rlinecurve:
          {
            FT_Int  num_lines;
            FT_Int  nargs;


            FT_TRACE4(( " rlinecurve\n" ));

            if ( num_args < 8 )
              goto Stack_Underflow;

            nargs     = num_args & ~1;
            num_lines = ( nargs - 6 ) / 2;

            if ( cff_builder_start_point( builder, x, y ) ||
                 check_points( builder, num_lines + 3 )   )
              goto Fail;

            args -= nargs;

            /* first, add the line segments */
            while ( num_lines > 0 )
            {
              x += args[0];
              y += args[1];
              cff_builder_add_point( builder, x, y, 1 );
              args += 2;
              num_lines--;
            }

            /* then the curve */
            x += args[0];
            y += args[1];
            cff_builder_add_point( builder, x, y, 0 );
            x += args[2];
            y += args[3];
            cff_builder_add_point( builder, x, y, 0 );
            x += args[4];
            y += args[5];
            cff_builder_add_point( builder, x, y, 1 );
            args = stack;
          }
          break;

        case cff_op_rcurveline:
          {
            FT_Int  num_curves;
            FT_Int  nargs;


            FT_TRACE4(( " rcurveline\n" ));

            if ( num_args < 8 )
              goto Stack_Underflow;

            nargs      = num_args - 2;
            nargs      = nargs - nargs % 6 + 2;
            num_curves = ( nargs - 2 ) / 6;

            if ( cff_builder_start_point ( builder, x, y ) ||
                 check_points( builder, num_curves * 3 + 2 ) )
              goto Fail;

            args -= nargs;

            /* first, add the curves */
            while ( num_curves > 0 )
            {
              x += args[0];
              y += args[1];
              cff_builder_add_point( builder, x, y, 0 );
              x += args[2];
              y += args[3];
              cff_builder_add_point( builder, x, y, 0 );
              x += args[4];
              y += args[5];
              cff_builder_add_point( builder, x, y, 1 );
              args += 6;
              num_curves--;
            }

            /* then the final line */
            x += args[0];
            y += args[1];
            cff_builder_add_point( builder, x, y, 1 );
            args = stack;
          }
          break;

        case cff_op_hflex1:
          {
            FT_Pos start_y;


            FT_TRACE4(( " hflex1\n" ));

            /* adding five more points: 4 control points, 1 on-curve point */
            /* -- make sure we have enough space for the start point if it */
            /* needs to be added                                           */
            if ( cff_builder_start_point( builder, x, y ) ||
                 check_points( builder, 6 )               )
              goto Fail;

            /* record the starting point's y position for later use */
            start_y = y;

            /* first control point */
            x += args[0];
            y += args[1];
            cff_builder_add_point( builder, x, y, 0 );

            /* second control point */
            x += args[2];
            y += args[3];
            cff_builder_add_point( builder, x, y, 0 );

            /* join point; on curve, with y-value the same as the last */
            /* control point's y-value                                 */
            x += args[4];
            cff_builder_add_point( builder, x, y, 1 );

            /* third control point, with y-value the same as the join */
            /* point's y-value                                        */
            x += args[5];
            cff_builder_add_point( builder, x, y, 0 );

            /* fourth control point */
            x += args[6];
            y += args[7];
            cff_builder_add_point( builder, x, y, 0 );

            /* ending point, with y-value the same as the start   */
            x += args[8];
            y  = start_y;
            cff_builder_add_point( builder, x, y, 1 );

            args = stack;
            break;
          }

        case cff_op_hflex:
          {
            FT_Pos start_y;


            FT_TRACE4(( " hflex\n" ));

            /* adding six more points; 4 control points, 2 on-curve points */
            if ( cff_builder_start_point( builder, x, y ) ||
                 check_points( builder, 6 )               )
              goto Fail;

            /* record the starting point's y-position for later use */
            start_y = y;

            /* first control point */
            x += args[0];
            cff_builder_add_point( builder, x, y, 0 );

            /* second control point */
            x += args[1];
            y += args[2];
            cff_builder_add_point( builder, x, y, 0 );

            /* join point; on curve, with y-value the same as the last */
            /* control point's y-value                                 */
            x += args[3];
            cff_builder_add_point( builder, x, y, 1 );

            /* third control point, with y-value the same as the join */
            /* point's y-value                                        */
            x += args[4];
            cff_builder_add_point( builder, x, y, 0 );

            /* fourth control point */
            x += args[5];
            y  = start_y;
            cff_builder_add_point( builder, x, y, 0 );

            /* ending point, with y-value the same as the start point's */
            /* y-value -- we don't add this point, though               */
            x += args[6];
            cff_builder_add_point( builder, x, y, 1 );

            args = stack;
            break;
          }

        case cff_op_flex1:
          {
            FT_Pos     start_x, start_y; /* record start x, y values for */
                                         /* alter use                    */
            FT_Fixed   dx = 0, dy = 0;   /* used in horizontal/vertical  */
                                         /* algorithm below              */
            FT_Int     horizontal, count;
            FT_Fixed*  temp;


            FT_TRACE4(( " flex1\n" ));

            /* adding six more points; 4 control points, 2 on-curve points */
            if ( cff_builder_start_point( builder, x, y ) ||
                 check_points( builder, 6 )               )
              goto Fail;

            /* record the starting point's x, y position for later use */
            start_x = x;
            start_y = y;

            /* XXX: figure out whether this is supposed to be a horizontal */
            /*      or vertical flex; the Type 2 specification is vague... */

            temp = args;

            /* grab up to the last argument */
            for ( count = 5; count > 0; count-- )
            {
              dx += temp[0];
              dy += temp[1];
              temp += 2;
            }

            if ( dx < 0 )
              dx = -dx;
            if ( dy < 0 )
              dy = -dy;

            /* strange test, but here it is... */
            horizontal = ( dx > dy );

            for ( count = 5; count > 0; count-- )
            {
              x += args[0];
              y += args[1];
              cff_builder_add_point( builder, x, y,
                                     (FT_Bool)( count == 3 ) );
              args += 2;
            }

            /* is last operand an x- or y-delta? */
            if ( horizontal )
            {
              x += args[0];
              y  = start_y;
            }
            else
            {
              x  = start_x;
              y += args[0];
            }

            cff_builder_add_point( builder, x, y, 1 );

            args = stack;
            break;
           }

        case cff_op_flex:
          {
            FT_UInt  count;


            FT_TRACE4(( " flex\n" ));

            if ( cff_builder_start_point( builder, x, y ) ||
                 check_points( builder, 6 )               )
              goto Fail;

            for ( count = 6; count > 0; count-- )
            {
              x += args[0];
              y += args[1];
              cff_builder_add_point( builder, x, y,
                                     (FT_Bool)( count == 4 || count == 1 ) );
              args += 2;
            }

            args = stack;
          }
          break;

        case cff_op_seac:
            FT_TRACE4(( " seac\n" ));

            error = cff_operator_seac( decoder,
                                       args[0], args[1], args[2],
                                       (FT_Int)( args[3] >> 16 ),
                                       (FT_Int)( args[4] >> 16 ) );

            /* add current outline to the glyph slot */
            FT_GlyphLoader_Add( builder->loader );

            /* return now! */
            FT_TRACE4(( "\n" ));
            return error;

        case cff_op_endchar:
          FT_TRACE4(( " endchar\n" ));

          /* We are going to emulate the seac operator. */
          if ( num_args >= 4 )
          {
            /* Save glyph width so that the subglyphs don't overwrite it. */
            FT_Pos  glyph_width = decoder->glyph_width;

            error = cff_operator_seac( decoder,
                                       0L, args[-4], args[-3],
                                       (FT_Int)( args[-2] >> 16 ),
                                       (FT_Int)( args[-1] >> 16 ) );

            decoder->glyph_width = glyph_width;
          }
          else
          {
            if ( !error )
              error = CFF_Err_Ok;

            cff_builder_close_contour( builder );

            /* close hints recording session */
            if ( hinter )
            {
              if ( hinter->close( hinter->hints,
                                  builder->current->n_points ) )
                goto Syntax_Error;

              /* apply hints to the loaded glyph outline now */
              hinter->apply( hinter->hints,
                             builder->current,
                             (PSH_Globals)builder->hints_globals,
                             decoder->hint_mode );
            }

            /* add current outline to the glyph slot */
            FT_GlyphLoader_Add( builder->loader );
          }

          /* return now! */
          FT_TRACE4(( "\n" ));
          return error;

        case cff_op_abs:
          FT_TRACE4(( " abs\n" ));

          if ( args[0] < 0 )
            args[0] = -args[0];
          args++;
          break;

        case cff_op_add:
          FT_TRACE4(( " add\n" ));

          args[0] += args[1];
          args++;
          break;

        case cff_op_sub:
          FT_TRACE4(( " sub\n" ));

          args[0] -= args[1];
          args++;
          break;

        case cff_op_div:
          FT_TRACE4(( " div\n" ));

          args[0] = FT_DivFix( args[0], args[1] );
          args++;
          break;

        case cff_op_neg:
          FT_TRACE4(( " neg\n" ));

          args[0] = -args[0];
          args++;
          break;

        case cff_op_random:
          {
            FT_Fixed  Rand;


            FT_TRACE4(( " rand\n" ));

            Rand = seed;
            if ( Rand >= 0x8000L )
              Rand++;

            args[0] = Rand;
            seed    = FT_MulFix( seed, 0x10000L - seed );
            if ( seed == 0 )
              seed += 0x2873;
            args++;
          }
          break;

        case cff_op_mul:
          FT_TRACE4(( " mul\n" ));

          args[0] = FT_MulFix( args[0], args[1] );
          args++;
          break;

        case cff_op_sqrt:
          FT_TRACE4(( " sqrt\n" ));

          if ( args[0] > 0 )
          {
            FT_Int    count = 9;
            FT_Fixed  root  = args[0];
            FT_Fixed  new_root;


            for (;;)
            {
              new_root = ( root + FT_DivFix( args[0], root ) + 1 ) >> 1;
              if ( new_root == root || count <= 0 )
                break;
              root = new_root;
            }
            args[0] = new_root;
          }
          else
            args[0] = 0;
          args++;
          break;

        case cff_op_drop:
          /* nothing */
          FT_TRACE4(( " drop\n" ));

          break;

        case cff_op_exch:
          {
            FT_Fixed  tmp;


            FT_TRACE4(( " exch\n" ));

            tmp     = args[0];
            args[0] = args[1];
            args[1] = tmp;
            args   += 2;
          }
          break;

        case cff_op_index:
          {
            FT_Int  idx = (FT_Int)( args[0] >> 16 );


            FT_TRACE4(( " index\n" ));

            if ( idx < 0 )
              idx = 0;
            else if ( idx > num_args - 2 )
              idx = num_args - 2;
            args[0] = args[-( idx + 1 )];
            args++;
          }
          break;

        case cff_op_roll:
          {
            FT_Int  count = (FT_Int)( args[0] >> 16 );
            FT_Int  idx   = (FT_Int)( args[1] >> 16 );


            FT_TRACE4(( " roll\n" ));

            if ( count <= 0 )
              count = 1;

            args -= count;
            if ( args < stack )
              goto Stack_Underflow;

            if ( idx >= 0 )
            {
              while ( idx > 0 )
              {
                FT_Fixed  tmp = args[count - 1];
                FT_Int    i;


                for ( i = count - 2; i >= 0; i-- )
                  args[i + 1] = args[i];
                args[0] = tmp;
                idx--;
              }
            }
            else
            {
              while ( idx < 0 )
              {
                FT_Fixed  tmp = args[0];
                FT_Int    i;


                for ( i = 0; i < count - 1; i++ )
                  args[i] = args[i + 1];
                args[count - 1] = tmp;
                idx++;
              }
            }
            args += count;
          }
          break;

        case cff_op_dup:
          FT_TRACE4(( " dup\n" ));

          args[1] = args[0];
          args += 2;
          break;

        case cff_op_put:
          {
            FT_Fixed  val = args[0];
            FT_Int    idx = (FT_Int)( args[1] >> 16 );


            FT_TRACE4(( " put\n" ));

            if ( idx >= 0 && idx < CFF_MAX_TRANS_ELEMENTS )
              decoder->buildchar[idx] = val;
          }
          break;

        case cff_op_get:
          {
            FT_Int    idx = (FT_Int)( args[0] >> 16 );
            FT_Fixed  val = 0;


            FT_TRACE4(( " get\n" ));

            if ( idx >= 0 && idx < CFF_MAX_TRANS_ELEMENTS )
              val = decoder->buildchar[idx];

            args[0] = val;
            args++;
          }
          break;

        case cff_op_store:
          FT_TRACE4(( " store\n"));

          goto Unimplemented;

        case cff_op_load:
          FT_TRACE4(( " load\n" ));

          goto Unimplemented;

        case cff_op_dotsection:
          /* this operator is deprecated and ignored by the parser */
          FT_TRACE4(( " dotsection\n" ));
          break;

        case cff_op_closepath:
          /* this is an invalid Type 2 operator; however, there        */
          /* exist fonts which are incorrectly converted from probably */
          /* Type 1 to CFF, and some parsers seem to accept it         */

          FT_TRACE4(( " closepath (invalid op)\n" ));

          args = stack;
          break;

        case cff_op_hsbw:
          /* this is an invalid Type 2 operator; however, there        */
          /* exist fonts which are incorrectly converted from probably */
          /* Type 1 to CFF, and some parsers seem to accept it         */

          FT_TRACE4(( " hsbw (invalid op)\n" ));

          decoder->glyph_width = decoder->nominal_width + ( args[1] >> 16 );

          decoder->builder.left_bearing.x = args[0];
          decoder->builder.left_bearing.y = 0;

          x    = decoder->builder.pos_x + args[0];
          y    = decoder->builder.pos_y;
          args = stack;
          break;

        case cff_op_sbw:
          /* this is an invalid Type 2 operator; however, there        */
          /* exist fonts which are incorrectly converted from probably */
          /* Type 1 to CFF, and some parsers seem to accept it         */

          FT_TRACE4(( " sbw (invalid op)\n" ));

          decoder->glyph_width = decoder->nominal_width + ( args[2] >> 16 );

          decoder->builder.left_bearing.x = args[0];
          decoder->builder.left_bearing.y = args[1];

          x    = decoder->builder.pos_x + args[0];
          y    = decoder->builder.pos_y + args[1];
          args = stack;
          break;

        case cff_op_setcurrentpoint:
          /* this is an invalid Type 2 operator; however, there        */
          /* exist fonts which are incorrectly converted from probably */
          /* Type 1 to CFF, and some parsers seem to accept it         */

          FT_TRACE4(( " setcurrentpoint (invalid op)\n" ));

          x    = decoder->builder.pos_x + args[0];
          y    = decoder->builder.pos_y + args[1];
          args = stack;
          break;

        case cff_op_callothersubr:
          /* this is an invalid Type 2 operator; however, there        */
          /* exist fonts which are incorrectly converted from probably */
          /* Type 1 to CFF, and some parsers seem to accept it         */

          FT_TRACE4(( " callothersubr (invalid op)\n" ));

          /* subsequent `pop' operands should add the arguments,       */
           /* this is the implementation described for `unknown' other  */
           /* subroutines in the Type1 spec.                            */
           args -= 2 + ( args[-2] >> 16 );
          if ( args < stack )
            goto Stack_Underflow;
           break;
 
         case cff_op_pop:
          /* Type 1 to CFF, and some parsers seem to accept it         */

          FT_TRACE4(( " pop (invalid op)\n" ));

          args++;
          break;

        case cff_op_and:
          {
            FT_Fixed  cond = args[0] && args[1];


            FT_TRACE4(( " and\n" ));

            args[0] = cond ? 0x10000L : 0;
            args++;
          }
          break;

        case cff_op_or:
          {
            FT_Fixed  cond = args[0] || args[1];


            FT_TRACE4(( " or\n" ));

            args[0] = cond ? 0x10000L : 0;
            args++;
          }
          break;

        case cff_op_eq:
          {
            FT_Fixed  cond = !args[0];


            FT_TRACE4(( " eq\n" ));

            args[0] = cond ? 0x10000L : 0;
            args++;
          }
          break;

        case cff_op_ifelse:
          {
            FT_Fixed  cond = ( args[2] <= args[3] );


            FT_TRACE4(( " ifelse\n" ));

            if ( !cond )
              args[0] = args[1];
            args++;
          }
          break;

        case cff_op_callsubr:
          {
            FT_UInt  idx = (FT_UInt)( ( args[0] >> 16 ) +
                                      decoder->locals_bias );


            FT_TRACE4(( " callsubr(%d)\n", idx ));

            if ( idx >= decoder->num_locals )
            {
              FT_ERROR(( "cff_decoder_parse_charstrings:"
                         " invalid local subr index\n" ));
              goto Syntax_Error;
            }

            if ( zone - decoder->zones >= CFF_MAX_SUBRS_CALLS )
            {
              FT_ERROR(( "cff_decoder_parse_charstrings:"
                         " too many nested subrs\n" ));
              goto Syntax_Error;
            }

            zone->cursor = ip;  /* save current instruction pointer */

            zone++;
            zone->base   = decoder->locals[idx];
            zone->limit  = decoder->locals[idx + 1];
            zone->cursor = zone->base;

            if ( !zone->base || zone->limit == zone->base )
            {
              FT_ERROR(( "cff_decoder_parse_charstrings:"
                         " invoking empty subrs\n" ));
              goto Syntax_Error;
            }

            decoder->zone = zone;
            ip            = zone->base;
            limit         = zone->limit;
          }
          break;

        case cff_op_callgsubr:
          {
            FT_UInt  idx = (FT_UInt)( ( args[0] >> 16 ) +
                                      decoder->globals_bias );


            FT_TRACE4(( " callgsubr(%d)\n", idx ));

            if ( idx >= decoder->num_globals )
            {
              FT_ERROR(( "cff_decoder_parse_charstrings:"
                         " invalid global subr index\n" ));
              goto Syntax_Error;
            }

            if ( zone - decoder->zones >= CFF_MAX_SUBRS_CALLS )
            {
              FT_ERROR(( "cff_decoder_parse_charstrings:"
                         " too many nested subrs\n" ));
              goto Syntax_Error;
            }

            zone->cursor = ip;  /* save current instruction pointer */

            zone++;
            zone->base   = decoder->globals[idx];
            zone->limit  = decoder->globals[idx + 1];
            zone->cursor = zone->base;

            if ( !zone->base || zone->limit == zone->base )
            {
              FT_ERROR(( "cff_decoder_parse_charstrings:"
                         " invoking empty subrs\n" ));
              goto Syntax_Error;
            }

            decoder->zone = zone;
            ip            = zone->base;
            limit         = zone->limit;
          }
          break;

        case cff_op_return:
          FT_TRACE4(( " return\n" ));

          if ( decoder->zone <= decoder->zones )
          {
            FT_ERROR(( "cff_decoder_parse_charstrings:"
                       " unexpected return\n" ));
            goto Syntax_Error;
          }

          decoder->zone--;
          zone  = decoder->zone;
          ip    = zone->cursor;
          limit = zone->limit;
          break;

        default:
        Unimplemented:
          FT_ERROR(( "Unimplemented opcode: %d", ip[-1] ));

          if ( ip[-1] == 12 )
            FT_ERROR(( " %d", ip[0] ));
          FT_ERROR(( "\n" ));

          return CFF_Err_Unimplemented_Feature;
        }

      decoder->top = args;

      } /* general operator processing */

    } /* while ip < limit */

    FT_TRACE4(( "..end..\n\n" ));

  Fail:
    return error;

  Syntax_Error:
    FT_TRACE4(( "cff_decoder_parse_charstrings: syntax error\n" ));
    return CFF_Err_Invalid_File_Format;

  Stack_Underflow:
    FT_TRACE4(( "cff_decoder_parse_charstrings: stack underflow\n" ));
    return CFF_Err_Too_Few_Arguments;

  Stack_Overflow:
    FT_TRACE4(( "cff_decoder_parse_charstrings: stack overflow\n" ));
    return CFF_Err_Stack_Overflow;
  }
