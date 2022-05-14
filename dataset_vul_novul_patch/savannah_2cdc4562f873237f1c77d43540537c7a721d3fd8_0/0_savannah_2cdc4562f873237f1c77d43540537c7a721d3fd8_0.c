  cf2_interpT2CharString( CF2_Font              font,
                          CF2_Buffer            buf,
                          CF2_OutlineCallbacks  callbacks,
                          const FT_Vector*      translation,
                          FT_Bool               doingSeac,
                          CF2_Fixed             curX,
                          CF2_Fixed             curY,
                          CF2_Fixed*            width )
  {
    /* lastError is used for errors that are immediately tested */
    FT_Error  lastError = FT_Err_Ok;

    /* pointer to parsed font object */
    CFF_Decoder*  decoder = font->decoder;

    FT_Error*  error  = &font->error;
    FT_Memory  memory = font->memory;

    CF2_Fixed  scaleY        = font->innerTransform.d;
    CF2_Fixed  nominalWidthX = cf2_getNominalWidthX( decoder );

    /* save this for hinting seac accents */
    CF2_Fixed  hintOriginY = curY;

    CF2_Stack  opStack = NULL;
    FT_Byte    op1;                       /* first opcode byte */

    /* instruction limit; 20,000,000 matches Avalon */
    FT_UInt32  instructionLimit = 20000000UL;

    CF2_ArrStackRec  subrStack;

    FT_Bool     haveWidth;
    CF2_Buffer  charstring = NULL;

    CF2_Int  charstringIndex = -1;       /* initialize to empty */

    /* TODO: placeholders for hint structures */

    /* objects used for hinting */
    CF2_ArrStackRec  hStemHintArray;
    CF2_ArrStackRec  vStemHintArray;

    CF2_HintMaskRec   hintMask;
    CF2_GlyphPathRec  glyphPath;


    /* initialize the remaining objects */
    cf2_arrstack_init( &subrStack,
                       memory,
                       error,
                       sizeof ( CF2_BufferRec ) );
    cf2_arrstack_init( &hStemHintArray,
                       memory,
                       error,
                       sizeof ( CF2_StemHintRec ) );
    cf2_arrstack_init( &vStemHintArray,
                       memory,
                       error,
                       sizeof ( CF2_StemHintRec ) );

    /* initialize CF2_StemHint arrays */
    cf2_hintmask_init( &hintMask, error );

    /* initialize path map to manage drawing operations */

    /* Note: last 4 params are used to handle `MoveToPermissive', which */
    /*       may need to call `hintMap.Build'                           */
    /* TODO: MoveToPermissive is gone; are these still needed?          */
    cf2_glyphpath_init( &glyphPath,
                        font,
                        callbacks,
                        scaleY,
                        /* hShift, */
                        &hStemHintArray,
                        &vStemHintArray,
                        &hintMask,
                        hintOriginY,
                        &font->blues,
                        translation );

    /*
     * Initialize state for width parsing.  From the CFF Spec:
     *
     *   The first stack-clearing operator, which must be one of hstem,
     *   hstemhm, vstem, vstemhm, cntrmask, hintmask, hmoveto, vmoveto,
     *   rmoveto, or endchar, takes an additional argument - the width (as
     *   described earlier), which may be expressed as zero or one numeric
     *   argument.
     *
     * What we implement here uses the first validly specified width, but
     * does not detect errors for specifying more than one width.
     *
     * If one of the above operators occurs without explicitly specifying
     * a width, we assume the default width.
     *
     */
    haveWidth = FALSE;
    *width    = cf2_getDefaultWidthX( decoder );

    /*
     * Note: at this point, all pointers to resources must be NULL
     * and all local objects must be initialized.
     * There must be no branches to exit: above this point.
     *
     */

    /* allocate an operand stack */
    opStack = cf2_stack_init( memory, error );
    if ( !opStack )
    {
      lastError = FT_THROW( Out_Of_Memory );
      goto exit;
    }

    /* initialize subroutine stack by placing top level charstring as */
    /* first element (max depth plus one for the charstring)          */
    /* Note: Caller owns and must finalize the first charstring.      */
    /*       Our copy of it does not change that requirement.         */
    cf2_arrstack_setCount( &subrStack, CF2_MAX_SUBR + 1 );

    charstring  = (CF2_Buffer)cf2_arrstack_getBuffer( &subrStack );
    *charstring = *buf;    /* structure copy */

    charstringIndex = 0;       /* entry is valid now */

    /* catch errors so far */
    if ( *error )
      goto exit;

    /* main interpreter loop */
    while ( 1 )
    {
      if ( cf2_buf_isEnd( charstring ) )
      {
        /* If we've reached the end of the charstring, simulate a */
        /* cf2_cmdRETURN or cf2_cmdENDCHAR.                       */
        if ( charstringIndex )
          op1 = cf2_cmdRETURN;  /* end of buffer for subroutine */
        else
          op1 = cf2_cmdENDCHAR; /* end of buffer for top level charstring */
      }
      else
        op1 = (FT_Byte)cf2_buf_readByte( charstring );

      /* check for errors once per loop */
      if ( *error )
        goto exit;

      instructionLimit--;
      if ( instructionLimit == 0 )
      {
        lastError = FT_THROW( Invalid_Glyph_Format );
        goto exit;
      }

      switch( op1 )
      {
      case cf2_cmdRESERVED_0:
      case cf2_cmdRESERVED_2:
      case cf2_cmdRESERVED_9:
      case cf2_cmdRESERVED_13:
      case cf2_cmdRESERVED_15:
      case cf2_cmdRESERVED_16:
      case cf2_cmdRESERVED_17:
        /* we may get here if we have a prior error */
        FT_TRACE4(( " unknown op (%d)\n", op1 ));
        break;

      case cf2_cmdHSTEMHM:
      case cf2_cmdHSTEM:
        FT_TRACE4(( op1 == cf2_cmdHSTEMHM ? " hstemhm\n" : " hstem\n" ));
 
         /* never add hints after the mask is computed */
         if ( cf2_hintmask_isValid( &hintMask ) )
        {
           FT_TRACE4(( "cf2_interpT2CharString:"
                       " invalid horizontal hint mask\n" ));
          break;
        }
 
         cf2_doStems( font,
                      opStack,
                     0 );

        if ( font->decoder->width_only )
            goto exit;

        break;

      case cf2_cmdVSTEMHM:
      case cf2_cmdVSTEM:
        FT_TRACE4(( op1 == cf2_cmdVSTEMHM ? " vstemhm\n" : " vstem\n" ));

        /* never add hints after the mask is computed */
        if ( cf2_hintmask_isValid( &hintMask ) )
 
         /* never add hints after the mask is computed */
         if ( cf2_hintmask_isValid( &hintMask ) )
        {
           FT_TRACE4(( "cf2_interpT2CharString:"
                       " invalid vertical hint mask\n" ));
          break;
        }
 
         cf2_doStems( font,
                      opStack,
            goto exit;

        break;

      case cf2_cmdVMOVETO:
        FT_TRACE4(( " vmoveto\n" ));

        if ( cf2_stack_count( opStack ) > 1 && !haveWidth )
          *width = cf2_stack_getReal( opStack, 0 ) + nominalWidthX;

        /* width is defined or default after this */
        haveWidth = TRUE;

        if ( font->decoder->width_only )
            goto exit;

        curY += cf2_stack_popFixed( opStack );

        cf2_glyphpath_moveTo( &glyphPath, curX, curY );

        break;

      case cf2_cmdRLINETO:
        {
          CF2_UInt  index;
          CF2_UInt  count = cf2_stack_count( opStack );


          FT_TRACE4(( " rlineto\n" ));

          for ( index = 0; index < count; index += 2 )
          {
            curX += cf2_stack_getReal( opStack, index + 0 );
            curY += cf2_stack_getReal( opStack, index + 1 );

            cf2_glyphpath_lineTo( &glyphPath, curX, curY );
          }

          cf2_stack_clear( opStack );
        }
        continue; /* no need to clear stack again */

      case cf2_cmdHLINETO:
      case cf2_cmdVLINETO:
        {
          CF2_UInt  index;
          CF2_UInt  count = cf2_stack_count( opStack );

          FT_Bool  isX = op1 == cf2_cmdHLINETO;


          FT_TRACE4(( isX ? " hlineto\n" : " vlineto\n" ));

          for ( index = 0; index < count; index++ )
          {
            CF2_Fixed  v = cf2_stack_getReal( opStack, index );


            if ( isX )
              curX += v;
            else
              curY += v;

            isX = !isX;

            cf2_glyphpath_lineTo( &glyphPath, curX, curY );
          }

          cf2_stack_clear( opStack );
        }
        continue;

      case cf2_cmdRCURVELINE:
      case cf2_cmdRRCURVETO:
        {
          CF2_UInt  count = cf2_stack_count( opStack );
          CF2_UInt  index = 0;


          FT_TRACE4(( op1 == cf2_cmdRCURVELINE ? " rcurveline\n"
                                               : " rrcurveto\n" ));

          while ( index + 6 <= count )
          {
            CF2_Fixed  x1 = cf2_stack_getReal( opStack, index + 0 ) + curX;
            CF2_Fixed  y1 = cf2_stack_getReal( opStack, index + 1 ) + curY;
            CF2_Fixed  x2 = cf2_stack_getReal( opStack, index + 2 ) + x1;
            CF2_Fixed  y2 = cf2_stack_getReal( opStack, index + 3 ) + y1;
            CF2_Fixed  x3 = cf2_stack_getReal( opStack, index + 4 ) + x2;
            CF2_Fixed  y3 = cf2_stack_getReal( opStack, index + 5 ) + y2;


            cf2_glyphpath_curveTo( &glyphPath, x1, y1, x2, y2, x3, y3 );

            curX   = x3;
            curY   = y3;
            index += 6;
          }

          if ( op1 == cf2_cmdRCURVELINE )
          {
            curX += cf2_stack_getReal( opStack, index + 0 );
            curY += cf2_stack_getReal( opStack, index + 1 );

            cf2_glyphpath_lineTo( &glyphPath, curX, curY );
          }

          cf2_stack_clear( opStack );
        }
        continue; /* no need to clear stack again */

      case cf2_cmdCALLGSUBR:
      case cf2_cmdCALLSUBR:
        {
          CF2_UInt  subrIndex;


          FT_TRACE4(( op1 == cf2_cmdCALLGSUBR ? " callgsubr"
                                              : " callsubr" ));

          if ( charstringIndex > CF2_MAX_SUBR )
          {
            /* max subr plus one for charstring */
            lastError = FT_THROW( Invalid_Glyph_Format );
            goto exit;                      /* overflow of stack */
          }

          /* push our current CFF charstring region on subrStack */
          charstring = (CF2_Buffer)
                         cf2_arrstack_getPointer( &subrStack,
                                                  charstringIndex + 1 );

          /* set up the new CFF region and pointer */
          subrIndex = cf2_stack_popInt( opStack );

          switch ( op1 )
          {
          case cf2_cmdCALLGSUBR:
            FT_TRACE4(( "(%d)\n", subrIndex + decoder->globals_bias ));

            if ( cf2_initGlobalRegionBuffer( decoder,
                                             subrIndex,
                                             charstring ) )
            {
              lastError = FT_THROW( Invalid_Glyph_Format );
              goto exit;  /* subroutine lookup or stream error */
            }
            break;

          default:
            /* cf2_cmdCALLSUBR */
            FT_TRACE4(( "(%d)\n", subrIndex + decoder->locals_bias ));

            if ( cf2_initLocalRegionBuffer( decoder,
                                            subrIndex,
                                            charstring ) )
            {
              lastError = FT_THROW( Invalid_Glyph_Format );
              goto exit;  /* subroutine lookup or stream error */
            }
          }

          charstringIndex += 1;       /* entry is valid now */
        }
        continue; /* do not clear the stack */

      case cf2_cmdRETURN:
        FT_TRACE4(( " return\n" ));

        if ( charstringIndex < 1 )
        {
          /* Note: cannot return from top charstring */
          lastError = FT_THROW( Invalid_Glyph_Format );
          goto exit;                      /* underflow of stack */
        }

        /* restore position in previous charstring */
        charstring = (CF2_Buffer)
                       cf2_arrstack_getPointer( &subrStack,
                                                --charstringIndex );
        continue;     /* do not clear the stack */

      case cf2_cmdESC:
        {
          FT_Byte  op2 = (FT_Byte)cf2_buf_readByte( charstring );


          switch ( op2 )
          {
          case cf2_escDOTSECTION:
            /* something about `flip type of locking' -- ignore it */
            FT_TRACE4(( " dotsection\n" ));

            break;

          /* TODO: should these operators be supported? */
          case cf2_escAND: /* in spec */
            FT_TRACE4(( " and\n" ));

            CF2_FIXME;
            break;

          case cf2_escOR: /* in spec */
            FT_TRACE4(( " or\n" ));

            CF2_FIXME;
            break;

          case cf2_escNOT: /* in spec */
            FT_TRACE4(( " not\n" ));

            CF2_FIXME;
            break;

          case cf2_escABS: /* in spec */
            FT_TRACE4(( " abs\n" ));

            CF2_FIXME;
            break;

          case cf2_escADD: /* in spec */
            FT_TRACE4(( " add\n" ));

            CF2_FIXME;
            break;

          case cf2_escSUB: /* in spec */
            FT_TRACE4(( " sub\n" ));

            CF2_FIXME;
            break;

          case cf2_escDIV: /* in spec */
            FT_TRACE4(( " div\n" ));

            CF2_FIXME;
            break;

          case cf2_escNEG: /* in spec */
            FT_TRACE4(( " neg\n" ));

            CF2_FIXME;
            break;

          case cf2_escEQ: /* in spec */
            FT_TRACE4(( " eq\n" ));

            CF2_FIXME;
            break;

          case cf2_escDROP: /* in spec */
            FT_TRACE4(( " drop\n" ));

            CF2_FIXME;
            break;

          case cf2_escPUT: /* in spec */
            FT_TRACE4(( " put\n" ));

            CF2_FIXME;
            break;

          case cf2_escGET: /* in spec */
            FT_TRACE4(( " get\n" ));

            CF2_FIXME;
            break;

          case cf2_escIFELSE: /* in spec */
            FT_TRACE4(( " ifelse\n" ));

            CF2_FIXME;
            break;

          case cf2_escRANDOM: /* in spec */
            FT_TRACE4(( " random\n" ));

            CF2_FIXME;
            break;

          case cf2_escMUL: /* in spec */
            FT_TRACE4(( " mul\n" ));

            CF2_FIXME;
            break;

          case cf2_escSQRT: /* in spec */
            FT_TRACE4(( " sqrt\n" ));

            CF2_FIXME;
            break;

          case cf2_escDUP: /* in spec */
            FT_TRACE4(( " dup\n" ));

            CF2_FIXME;
            break;

          case cf2_escEXCH: /* in spec */
            FT_TRACE4(( " exch\n" ));

            CF2_FIXME;
            break;

          case cf2_escINDEX: /* in spec */
            FT_TRACE4(( " index\n" ));

            CF2_FIXME;
            break;

          case cf2_escROLL: /* in spec */
            FT_TRACE4(( " roll\n" ));

            CF2_FIXME;
            break;

          case cf2_escHFLEX:
            {
              static const FT_Bool  readFromStack[12] =
              {
                TRUE /* dx1 */, FALSE /* dy1 */,
                TRUE /* dx2 */, TRUE  /* dy2 */,
                TRUE /* dx3 */, FALSE /* dy3 */,
                TRUE /* dx4 */, FALSE /* dy4 */,
                TRUE /* dx5 */, FALSE /* dy5 */,
                TRUE /* dx6 */, FALSE /* dy6 */
              };


              FT_TRACE4(( " hflex\n" ));

              cf2_doFlex( opStack,
                          &curX,
                          &curY,
                          &glyphPath,
                          readFromStack,
                          FALSE /* doConditionalLastRead */ );
            }
            continue;

          case cf2_escFLEX:
            {
              static const FT_Bool  readFromStack[12] =
              {
                TRUE /* dx1 */, TRUE /* dy1 */,
                TRUE /* dx2 */, TRUE /* dy2 */,
                TRUE /* dx3 */, TRUE /* dy3 */,
                TRUE /* dx4 */, TRUE /* dy4 */,
                TRUE /* dx5 */, TRUE /* dy5 */,
                TRUE /* dx6 */, TRUE /* dy6 */
              };


              FT_TRACE4(( " flex\n" ));

              cf2_doFlex( opStack,
                          &curX,
                          &curY,
                          &glyphPath,
                          readFromStack,
                          FALSE /* doConditionalLastRead */ );
            }
            break;      /* TODO: why is this not a continue? */

          case cf2_escHFLEX1:
            {
              static const FT_Bool  readFromStack[12] =
              {
                TRUE /* dx1 */, TRUE  /* dy1 */,
                TRUE /* dx2 */, TRUE  /* dy2 */,
                TRUE /* dx3 */, FALSE /* dy3 */,
                TRUE /* dx4 */, FALSE /* dy4 */,
                TRUE /* dx5 */, TRUE  /* dy5 */,
                TRUE /* dx6 */, FALSE /* dy6 */
              };


              FT_TRACE4(( " hflex1\n" ));

              cf2_doFlex( opStack,
                          &curX,
                          &curY,
                          &glyphPath,
                          readFromStack,
                          FALSE /* doConditionalLastRead */ );
            }
            continue;

          case cf2_escFLEX1:
            {
              static const FT_Bool  readFromStack[12] =
              {
                TRUE  /* dx1 */, TRUE  /* dy1 */,
                TRUE  /* dx2 */, TRUE  /* dy2 */,
                TRUE  /* dx3 */, TRUE  /* dy3 */,
                TRUE  /* dx4 */, TRUE  /* dy4 */,
                TRUE  /* dx5 */, TRUE  /* dy5 */,
                FALSE /* dx6 */, FALSE /* dy6 */
              };


              FT_TRACE4(( " flex1\n" ));

              cf2_doFlex( opStack,
                          &curX,
                          &curY,
                          &glyphPath,
                          readFromStack,
                          TRUE /* doConditionalLastRead */ );
            }
            continue;

          case cf2_escRESERVED_1:
          case cf2_escRESERVED_2:
          case cf2_escRESERVED_6:
          case cf2_escRESERVED_7:
          case cf2_escRESERVED_8:
          case cf2_escRESERVED_13:
          case cf2_escRESERVED_16:
          case cf2_escRESERVED_17:
          case cf2_escRESERVED_19:
          case cf2_escRESERVED_25:
          case cf2_escRESERVED_31:
          case cf2_escRESERVED_32:
          case cf2_escRESERVED_33:
          default:
            FT_TRACE4(( " unknown op (12, %d)\n", op2 ));

          }; /* end of switch statement checking `op2' */

        } /* case cf2_cmdESC */
        break;

      case cf2_cmdENDCHAR:
        FT_TRACE4(( " endchar\n" ));

        if ( cf2_stack_count( opStack ) == 1 ||
             cf2_stack_count( opStack ) == 5 )
        {
          if ( !haveWidth )
            *width = cf2_stack_getReal( opStack, 0 ) + nominalWidthX;
        }

        /* width is defined or default after this */
        haveWidth = TRUE;

        if ( font->decoder->width_only )
            goto exit;

        /* close path if still open */
        cf2_glyphpath_closeOpenPath( &glyphPath );

        if ( cf2_stack_count( opStack ) > 1 )
        {
          /* must be either 4 or 5 --                       */
          /* this is a (deprecated) implied `seac' operator */

          CF2_UInt       achar;
          CF2_UInt       bchar;
          CF2_BufferRec  component;
          CF2_Fixed      dummyWidth;   /* ignore component width */
          FT_Error       error2;


          if ( doingSeac )
          {
            lastError = FT_THROW( Invalid_Glyph_Format );
            goto exit;      /* nested seac */
          }

          achar = cf2_stack_popInt( opStack );
          bchar = cf2_stack_popInt( opStack );

          curY = cf2_stack_popFixed( opStack );
          curX = cf2_stack_popFixed( opStack );

          error2 = cf2_getSeacComponent( decoder, achar, &component );
          if ( error2 )
          {
             lastError = error2;      /* pass FreeType error through */
             goto exit;
          }
          cf2_interpT2CharString( font,
                                  &component,
                                  callbacks,
                                  translation,
                                  TRUE,
                                  curX,
                                  curY,
                                  &dummyWidth );
          cf2_freeSeacComponent( decoder, &component );

          error2 = cf2_getSeacComponent( decoder, bchar, &component );
          if ( error2 )
          {
            lastError = error2;      /* pass FreeType error through */
            goto exit;
          }
          cf2_interpT2CharString( font,
                                  &component,
                                  callbacks,
                                  translation,
                                  TRUE,
                                  0,
                                  0,
                                  &dummyWidth );
          cf2_freeSeacComponent( decoder, &component );
        }
        goto exit;

      case cf2_cmdCNTRMASK:
      case cf2_cmdHINTMASK:
        /* the final \n in the tracing message gets added in      */
        /* `cf2_hintmask_read' (which also traces the mask bytes) */
        FT_TRACE4(( op1 == cf2_cmdCNTRMASK ? " cntrmask" : " hintmask" ));

        /* if there are arguments on the stack, there this is an */
        /* implied cf2_cmdVSTEMHM                                */
        if ( cf2_stack_count( opStack ) != 0 )
         /* `cf2_hintmask_read' (which also traces the mask bytes) */
         FT_TRACE4(( op1 == cf2_cmdCNTRMASK ? " cntrmask" : " hintmask" ));
 
        /* never add hints after the mask is computed */
        if ( cf2_stack_count( opStack ) > 1    &&
             cf2_hintmask_isValid( &hintMask ) )
         {
          FT_TRACE4(( "cf2_interpT2CharString: invalid hint mask\n" ));
          break;
         }
 
        /* if there are arguments on the stack, there this is an */
        /* implied cf2_cmdVSTEMHM                                */
         cf2_doStems( font,
                      opStack,
                      &vStemHintArray,
        if ( op1 == cf2_cmdHINTMASK )
        {
          /* consume the hint mask bytes which follow the operator */
          cf2_hintmask_read( &hintMask,
                             charstring,
                             cf2_arrstack_size( &hStemHintArray ) +
                               cf2_arrstack_size( &vStemHintArray ) );
        }
        else
        {
          /*
           * Consume the counter mask bytes which follow the operator:
           * Build a temporary hint map, just to place and lock those
           * stems participating in the counter mask.  These are most
           * likely the dominant hstems, and are grouped together in a
           * few counter groups, not necessarily in correspondence
           * with the hint groups.  This reduces the chances of
           * conflicts between hstems that are initially placed in
           * separate hint groups and then brought together.  The
           * positions are copied back to `hStemHintArray', so we can
           * discard `counterMask' and `counterHintMap'.
           *
           */
          CF2_HintMapRec   counterHintMap;
          CF2_HintMaskRec  counterMask;


          cf2_hintmap_init( &counterHintMap,
                            font,
                            &glyphPath.initialHintMap,
                            &glyphPath.hintMoves,
                            scaleY );
          cf2_hintmask_init( &counterMask, error );

          cf2_hintmask_read( &counterMask,
                             charstring,
                             cf2_arrstack_size( &hStemHintArray ) +
                               cf2_arrstack_size( &vStemHintArray ) );
          cf2_hintmap_build( &counterHintMap,
                             &hStemHintArray,
                             &vStemHintArray,
                             &counterMask,
                             0,
                             FALSE );
        }
        break;

      case cf2_cmdRMOVETO:
        FT_TRACE4(( " rmoveto\n" ));

        if ( cf2_stack_count( opStack ) > 2 && !haveWidth )
          *width = cf2_stack_getReal( opStack, 0 ) + nominalWidthX;

        /* width is defined or default after this */
        haveWidth = TRUE;

        if ( font->decoder->width_only )
            goto exit;

        curY += cf2_stack_popFixed( opStack );
        curX += cf2_stack_popFixed( opStack );

        cf2_glyphpath_moveTo( &glyphPath, curX, curY );

        break;

      case cf2_cmdHMOVETO:
        FT_TRACE4(( " hmoveto\n" ));

        if ( cf2_stack_count( opStack ) > 1 && !haveWidth )
          *width = cf2_stack_getReal( opStack, 0 ) + nominalWidthX;

        /* width is defined or default after this */
        haveWidth = TRUE;

        if ( font->decoder->width_only )
            goto exit;

        curX += cf2_stack_popFixed( opStack );

        cf2_glyphpath_moveTo( &glyphPath, curX, curY );

        break;

      case cf2_cmdRLINECURVE:
        {
          CF2_UInt  count = cf2_stack_count( opStack );
          CF2_UInt  index = 0;


          FT_TRACE4(( " rlinecurve\n" ));

          while ( index + 6 < count )
          {
            curX += cf2_stack_getReal( opStack, index + 0 );
            curY += cf2_stack_getReal( opStack, index + 1 );

            cf2_glyphpath_lineTo( &glyphPath, curX, curY );
            index += 2;
          }

          while ( index < count )
          {
            CF2_Fixed  x1 = cf2_stack_getReal( opStack, index + 0 ) + curX;
            CF2_Fixed  y1 = cf2_stack_getReal( opStack, index + 1 ) + curY;
            CF2_Fixed  x2 = cf2_stack_getReal( opStack, index + 2 ) + x1;
            CF2_Fixed  y2 = cf2_stack_getReal( opStack, index + 3 ) + y1;
            CF2_Fixed  x3 = cf2_stack_getReal( opStack, index + 4 ) + x2;
            CF2_Fixed  y3 = cf2_stack_getReal( opStack, index + 5 ) + y2;


            cf2_glyphpath_curveTo( &glyphPath, x1, y1, x2, y2, x3, y3 );

            curX   = x3;
            curY   = y3;
            index += 6;
          }

          cf2_stack_clear( opStack );
        }
        continue; /* no need to clear stack again */

      case cf2_cmdVVCURVETO:
        {
          CF2_UInt  count = cf2_stack_count( opStack );
          CF2_UInt  index = 0;


          FT_TRACE4(( " vvcurveto\n" ));

          while ( index < count )
          {
            CF2_Fixed  x1, y1, x2, y2, x3, y3;


            if ( ( count - index ) & 1 )
            {
              x1 = cf2_stack_getReal( opStack, index ) + curX;

              ++index;
            }
            else
              x1 = curX;

            y1 = cf2_stack_getReal( opStack, index + 0 ) + curY;
            x2 = cf2_stack_getReal( opStack, index + 1 ) + x1;
            y2 = cf2_stack_getReal( opStack, index + 2 ) + y1;
            x3 = x2;
            y3 = cf2_stack_getReal( opStack, index + 3 ) + y2;

            cf2_glyphpath_curveTo( &glyphPath, x1, y1, x2, y2, x3, y3 );

            curX   = x3;
            curY   = y3;
            index += 4;
          }

          cf2_stack_clear( opStack );
        }
        continue; /* no need to clear stack again */

      case cf2_cmdHHCURVETO:
        {
          CF2_UInt  count = cf2_stack_count( opStack );
          CF2_UInt  index = 0;


          FT_TRACE4(( " hhcurveto\n" ));

          while ( index < count )
          {
            CF2_Fixed  x1, y1, x2, y2, x3, y3;


            if ( ( count - index ) & 1 )
            {
              y1 = cf2_stack_getReal( opStack, index ) + curY;

              ++index;
            }
            else
              y1 = curY;

            x1 = cf2_stack_getReal( opStack, index + 0 ) + curX;
            x2 = cf2_stack_getReal( opStack, index + 1 ) + x1;
            y2 = cf2_stack_getReal( opStack, index + 2 ) + y1;
            x3 = cf2_stack_getReal( opStack, index + 3 ) + x2;
            y3 = y2;

            cf2_glyphpath_curveTo( &glyphPath, x1, y1, x2, y2, x3, y3 );

            curX   = x3;
            curY   = y3;
            index += 4;
          }

          cf2_stack_clear( opStack );
        }
        continue; /* no need to clear stack again */

      case cf2_cmdVHCURVETO:
      case cf2_cmdHVCURVETO:
        {
          CF2_UInt  count = cf2_stack_count( opStack );
          CF2_UInt  index = 0;

          FT_Bool  alternate = op1 == cf2_cmdHVCURVETO;


          FT_TRACE4(( alternate ? " hvcurveto\n" : " vhcurveto\n" ));

          while ( index < count )
          {
            CF2_Fixed x1, x2, x3, y1, y2, y3;


            if ( alternate )
            {
              x1 = cf2_stack_getReal( opStack, index + 0 ) + curX;
              y1 = curY;
              x2 = cf2_stack_getReal( opStack, index + 1 ) + x1;
              y2 = cf2_stack_getReal( opStack, index + 2 ) + y1;
              y3 = cf2_stack_getReal( opStack, index + 3 ) + y2;

              if ( count - index == 5 )
              {
                x3 = cf2_stack_getReal( opStack, index + 4 ) + x2;

                ++index;
              }
              else
                x3 = x2;

              alternate = FALSE;
            }
            else
            {
              x1 = curX;
              y1 = cf2_stack_getReal( opStack, index + 0 ) + curY;
              x2 = cf2_stack_getReal( opStack, index + 1 ) + x1;
              y2 = cf2_stack_getReal( opStack, index + 2 ) + y1;
              x3 = cf2_stack_getReal( opStack, index + 3 ) + x2;

              if ( count - index == 5 )
              {
                y3 = cf2_stack_getReal( opStack, index + 4 ) + y2;

                ++index;
              }
              else
                y3 = y2;

              alternate = TRUE;
            }

            cf2_glyphpath_curveTo( &glyphPath, x1, y1, x2, y2, x3, y3 );

            curX   = x3;
            curY   = y3;
            index += 4;
          }

          cf2_stack_clear( opStack );
        }
        continue;     /* no need to clear stack again */

      case cf2_cmdEXTENDEDNMBR:
        {
          CF2_Int  v;


          v = (FT_Short)( ( cf2_buf_readByte( charstring ) << 8 ) |
                            cf2_buf_readByte( charstring )        );

          FT_TRACE4(( " %d", v ));

          cf2_stack_pushInt( opStack, v );
        }
        continue;

      default:
        /* numbers */
        {
          if ( /* op1 >= 32 && */ op1 <= 246 )
          {
            CF2_Int  v;


            v = op1 - 139;

            FT_TRACE4(( " %d", v ));

            /* -107 .. 107 */
            cf2_stack_pushInt( opStack, v );
          }

          else if ( /* op1 >= 247 && */ op1 <= 250 )
          {
            CF2_Int  v;


            v  = op1;
            v -= 247;
            v *= 256;
            v += cf2_buf_readByte( charstring );
            v += 108;

            FT_TRACE4(( " %d", v ));

            /* 108 .. 1131 */
            cf2_stack_pushInt( opStack, v );
          }

          else if ( /* op1 >= 251 && */ op1 <= 254 )
          {
            CF2_Int  v;


            v  = op1;
            v -= 251;
            v *= 256;
            v += cf2_buf_readByte( charstring );
            v  = -v - 108;

            FT_TRACE4(( " %d", v ));

            /* -1131 .. -108 */
            cf2_stack_pushInt( opStack, v );
          }

          else /* op1 == 255 */
          {
            CF2_Fixed  v;


            v = (CF2_Fixed)
                  ( ( (FT_UInt32)cf2_buf_readByte( charstring ) << 24 ) |
                    ( (FT_UInt32)cf2_buf_readByte( charstring ) << 16 ) |
                    ( (FT_UInt32)cf2_buf_readByte( charstring ) <<  8 ) |
                      (FT_UInt32)cf2_buf_readByte( charstring )         );

            FT_TRACE4(( " %.2f", v / 65536.0 ));

            cf2_stack_pushFixed( opStack, v );
          }
        }
        continue;   /* don't clear stack */

      } /* end of switch statement checking `op1' */

      cf2_stack_clear( opStack );

    } /* end of main interpreter loop */

    /* we get here if the charstring ends without cf2_cmdENDCHAR */
    FT_TRACE4(( "cf2_interpT2CharString:"
                "  charstring ends without ENDCHAR\n" ));

  exit:
    /* check whether last error seen is also the first one */
    cf2_setError( error, lastError );

    /* free resources from objects we've used */
    cf2_glyphpath_finalize( &glyphPath );
    cf2_arrstack_finalize( &vStemHintArray );
    cf2_arrstack_finalize( &hStemHintArray );
    cf2_arrstack_finalize( &subrStack );
    cf2_stack_free( opStack );

    FT_TRACE4(( "\n" ));

    return;
  }
