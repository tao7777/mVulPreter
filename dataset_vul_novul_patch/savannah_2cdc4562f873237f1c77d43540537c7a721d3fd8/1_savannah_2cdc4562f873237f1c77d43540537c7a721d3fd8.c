  cf2_hintmap_build( CF2_HintMap   hintmap,
                     CF2_ArrStack  hStemHintArray,
                     CF2_ArrStack  vStemHintArray,
                     CF2_HintMask  hintMask,
                     CF2_Fixed     hintOrigin,
                     FT_Bool       initialMap )
  {
    FT_Byte*  maskPtr;

    CF2_Font         font = hintmap->font;
    CF2_HintMaskRec  tempHintMask;

    size_t   bitCount, i;
    FT_Byte  maskByte;


    /* check whether initial map is constructed */
    if ( !initialMap && !cf2_hintmap_isValid( hintmap->initialHintMap ) )
    {
      /* make recursive call with initialHintMap and temporary mask; */
      /* temporary mask will get all bits set, below */
      cf2_hintmask_init( &tempHintMask, hintMask->error );
      cf2_hintmap_build( hintmap->initialHintMap,
                         hStemHintArray,
                         vStemHintArray,
                         &tempHintMask,
                         hintOrigin,
                         TRUE );
    }

    if ( !cf2_hintmask_isValid( hintMask ) )
    {
      /* without a hint mask, assume all hints are active */
      cf2_hintmask_setAll( hintMask,
                           cf2_arrstack_size( hStemHintArray ) +
                             cf2_arrstack_size( vStemHintArray ) );
      if ( !cf2_hintmask_isValid( hintMask ) )
          return;                   /* too many stem hints */
    }

    /* begin by clearing the map */
    hintmap->count     = 0;
    hintmap->lastIndex = 0;

    /* make a copy of the hint mask so we can modify it */
    tempHintMask = *hintMask;
     maskPtr      = cf2_hintmask_getMaskPtr( &tempHintMask );
 
     /* use the hStem hints only, which are first in the mask */
    /* TODO: compare this to cffhintmaskGetBitCount */
     bitCount = cf2_arrstack_size( hStemHintArray );
 
     /* synthetic embox hints get highest priority */
     if ( font->blues.doEmBoxHints )
     {
      cf2_hint_initZero( &dummy );   /* invalid hint map element */

      /* ghost bottom */
      cf2_hintmap_insertHint( hintmap,
                              &font->blues.emBoxBottomEdge,
                              &dummy );
      /* ghost top */
      cf2_hintmap_insertHint( hintmap,
                              &dummy,
                              &font->blues.emBoxTopEdge );
    }

    /* insert hints captured by a blue zone or already locked (higher */
    /* priority)                                                      */
    for ( i = 0, maskByte = 0x80; i < bitCount; i++ )
    {
      if ( maskByte & *maskPtr )
      {
        /* expand StemHint into two `CF2_Hint' elements */
        CF2_HintRec  bottomHintEdge, topHintEdge;


        cf2_hint_init( &bottomHintEdge,
                       hStemHintArray,
                       i,
                       font,
                       hintOrigin,
                       hintmap->scale,
                       TRUE /* bottom */ );
        cf2_hint_init( &topHintEdge,
                       hStemHintArray,
                       i,
                       font,
                       hintOrigin,
                       hintmap->scale,
                       FALSE /* top */ );

        if ( cf2_hint_isLocked( &bottomHintEdge ) ||
             cf2_hint_isLocked( &topHintEdge )    ||
             cf2_blues_capture( &font->blues,
                                &bottomHintEdge,
                                &topHintEdge )   )
        {
          /* insert captured hint into map */
          cf2_hintmap_insertHint( hintmap, &bottomHintEdge, &topHintEdge );

          *maskPtr &= ~maskByte;      /* turn off the bit for this hint */
        }
      }

      if ( ( i & 7 ) == 7 )
      {
        /* move to next mask byte */
        maskPtr++;
        maskByte = 0x80;
      }
      else
        maskByte >>= 1;
    }

    /* initial hint map includes only captured hints plus maybe one at 0 */

    /*
     * TODO: There is a problem here because we are trying to build a
     *       single hint map containing all captured hints.  It is
     *       possible for there to be conflicts between captured hints,
     *       either because of darkening or because the hints are in
     *       separate hint zones (we are ignoring hint zones for the
     *       initial map).  An example of the latter is MinionPro-Regular
     *       v2.030 glyph 883 (Greek Capital Alpha with Psili) at 15ppem.
     *       A stem hint for the psili conflicts with the top edge hint
     *       for the base character.  The stem hint gets priority because
     *       of its sort order.  In glyph 884 (Greek Capital Alpha with
     *       Psili and Oxia), the top of the base character gets a stem
     *       hint, and the psili does not.  This creates different initial
     *       maps for the two glyphs resulting in different renderings of
     *       the base character.  Will probably defer this either as not
     *       worth the cost or as a font bug.  I don't think there is any
     *       good reason for an accent to be captured by an alignment
     *       zone.  -darnold 2/12/10
     */

    if ( initialMap )
    {
      /* Apply a heuristic that inserts a point for (0,0), unless it's     */
      /* already covered by a mapping.  This locks the baseline for glyphs */
      /* that have no baseline hints.                                      */

      if ( hintmap->count == 0                           ||
           hintmap->edge[0].csCoord > 0                  ||
           hintmap->edge[hintmap->count - 1].csCoord < 0 )
      {
        /* all edges are above 0 or all edges are below 0; */
        /* construct a locked edge hint at 0               */

        CF2_HintRec  edge, invalid;


        cf2_hint_initZero( &edge );

        edge.flags = CF2_GhostBottom |
                     CF2_Locked      |
                     CF2_Synthetic;
        edge.scale = hintmap->scale;

        cf2_hint_initZero( &invalid );
        cf2_hintmap_insertHint( hintmap, &edge, &invalid );
      }
    }
    else
    {
      /* insert remaining hints */

      maskPtr = cf2_hintmask_getMaskPtr( &tempHintMask );

      for ( i = 0, maskByte = 0x80; i < bitCount; i++ )
      {
        if ( maskByte & *maskPtr )
        {
          CF2_HintRec  bottomHintEdge, topHintEdge;


          cf2_hint_init( &bottomHintEdge,
                         hStemHintArray,
                         i,
                         font,
                         hintOrigin,
                         hintmap->scale,
                         TRUE /* bottom */ );
          cf2_hint_init( &topHintEdge,
                         hStemHintArray,
                         i,
                         font,
                         hintOrigin,
                         hintmap->scale,
                         FALSE /* top */ );

          cf2_hintmap_insertHint( hintmap, &bottomHintEdge, &topHintEdge );
        }

        if ( ( i & 7 ) == 7 )
        {
          /* move to next mask byte */
          maskPtr++;
          maskByte = 0x80;
        }
        else
          maskByte >>= 1;
      }
    }

    /*
     * Note: The following line is a convenient place to break when
     *       debugging hinting.  Examine `hintmap->edge' for the list of
     *       enabled hints, then step over the call to see the effect of
     *       adjustment.  We stop here first on the recursive call that
     *       creates the initial map, and then on each counter group and
     *       hint zone.
     */

    /* adjust positions of hint edges that are not locked to blue zones */
    cf2_hintmap_adjustHints( hintmap );

    /* save the position of all hints that were used in this hint map; */
    /* if we use them again, we'll locate them in the same position    */
    if ( !initialMap )
    {
      for ( i = 0; i < hintmap->count; i++ )
      {
        if ( !cf2_hint_isSynthetic( &hintmap->edge[i] ) )
        {
          /* Note: include both valid and invalid edges            */
          /* Note: top and bottom edges are copied back separately */
          CF2_StemHint  stemhint = (CF2_StemHint)
                          cf2_arrstack_getPointer( hStemHintArray,
                                                   hintmap->edge[i].index );


          if ( cf2_hint_isTop( &hintmap->edge[i] ) )
            stemhint->maxDS = hintmap->edge[i].dsCoord;
          else
            stemhint->minDS = hintmap->edge[i].dsCoord;

          stemhint->used = TRUE;
        }
      }
    }

    /* hint map is ready to use */
    hintmap->isValid = TRUE;

    /* remember this mask has been used */
    cf2_hintmask_setNew( hintMask, FALSE );
  }
