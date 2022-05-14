  tt_cmap14_validate( FT_Byte*      table,
                      FT_Validator  valid )
  {
    FT_Byte*  p;
    FT_ULong  length;
    FT_ULong  num_selectors;


    if ( table + 2 + 4 + 4 > valid->limit )
      FT_INVALID_TOO_SHORT;

    p             = table + 2;
    length        = TT_NEXT_ULONG( p );
    num_selectors = TT_NEXT_ULONG( p );

    if ( length > (FT_ULong)( valid->limit - table ) ||
         /* length < 10 + 11 * num_selectors ? */
         length < 10                                 ||
         ( length - 10 ) / 11 < num_selectors        )
      FT_INVALID_TOO_SHORT;

    /* check selectors, they must be in increasing order */
    {
      /* we start lastVarSel at 1 because a variant selector value of 0
       * isn't valid.
       */
      FT_ULong  n, lastVarSel = 1;


      for ( n = 0; n < num_selectors; n++ )
      {
        FT_ULong  varSel    = TT_NEXT_UINT24( p );
        FT_ULong  defOff    = TT_NEXT_ULONG( p );
        FT_ULong  nondefOff = TT_NEXT_ULONG( p );


        if ( defOff >= length || nondefOff >= length )
          FT_INVALID_TOO_SHORT;

        if ( varSel < lastVarSel )
          FT_INVALID_DATA;

        lastVarSel = varSel + 1;

        /* check the default table (these glyphs should be reached     */
         /* through the normal Unicode cmap, no GIDs, just check order) */
         if ( defOff != 0 )
         {
          FT_Byte*  defp     = table + defOff;
          FT_ULong  numRanges;
           FT_ULong  i;
          FT_ULong  lastBase = 0;
 
 
          if ( defp + 4 > valid->limit )
            FT_INVALID_TOO_SHORT;

          numRanges = TT_NEXT_ULONG( defp );

           /* defp + numRanges * 4 > valid->limit ? */
           if ( numRanges > (FT_ULong)( valid->limit - defp ) / 4 )
             FT_INVALID_TOO_SHORT;


            if ( base + cnt >= 0x110000UL )              /* end of Unicode */
              FT_INVALID_DATA;

            if ( base < lastBase )
              FT_INVALID_DATA;

            lastBase = base + cnt + 1U;
          }
        }

        /* and the non-default table (these glyphs are specified here) */
        if ( nondefOff != 0 )
        {
          FT_Byte*  ndp         = table + nondefOff;
          FT_ULong  numMappings = TT_NEXT_ULONG( ndp );
         /* and the non-default table (these glyphs are specified here) */
         if ( nondefOff != 0 )
         {
          FT_Byte*  ndp        = table + nondefOff;
          FT_ULong  numMappings;
          FT_ULong  i, lastUni = 0;


          if ( ndp + 4 > valid->limit )
            FT_INVALID_TOO_SHORT;
 
          numMappings = TT_NEXT_ULONG( ndp );
 
          /* numMappings * 5 > (FT_ULong)( valid->limit - ndp ) ? */
          if ( numMappings > ( (FT_ULong)( valid->limit - ndp ) ) / 5 )
             FT_INVALID_TOO_SHORT;
 
           for ( i = 0; i < numMappings; ++i )

            lastUni = uni + 1U;

            if ( valid->level >= FT_VALIDATE_TIGHT    &&
                 gid >= TT_VALID_GLYPH_COUNT( valid ) )
              FT_INVALID_GLYPH_ID;
          }
        }
      }
    }
