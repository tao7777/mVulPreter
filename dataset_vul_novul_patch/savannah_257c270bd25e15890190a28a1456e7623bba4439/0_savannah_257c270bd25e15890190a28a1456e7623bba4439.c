  tt_sbit_decoder_init( TT_SBitDecoder       decoder,
                        TT_Face              face,
                        FT_ULong             strike_index,
                        TT_SBit_MetricsRec*  metrics )
  {
    FT_Error   error;
    FT_Stream  stream = face->root.stream;
    FT_ULong   ebdt_size;


    error = face->goto_table( face, TTAG_CBDT, stream, &ebdt_size );
    if ( error )
      error = face->goto_table( face, TTAG_EBDT, stream, &ebdt_size );
    if ( error )
      error = face->goto_table( face, TTAG_bdat, stream, &ebdt_size );
    if ( error )
      goto Exit;

    decoder->face    = face;
    decoder->stream  = stream;
    decoder->bitmap  = &face->root.glyph->bitmap;
    decoder->metrics = metrics;

    decoder->metrics_loaded   = 0;
    decoder->bitmap_allocated = 0;

    decoder->ebdt_start = FT_STREAM_POS();
    decoder->ebdt_size  = ebdt_size;

    decoder->eblc_base  = face->sbit_table;
    decoder->eblc_limit = face->sbit_table + face->sbit_table_size;

    /* now find the strike corresponding to the index */
    {
      FT_Byte*  p;


      if ( 8 + 48 * strike_index + 3 * 4 + 34 + 1 > face->sbit_table_size )
      {
        error = FT_THROW( Invalid_File_Format );
        goto Exit;
      }

      p = decoder->eblc_base + 8 + 48 * strike_index;

      decoder->strike_index_array = FT_NEXT_ULONG( p );
      p                          += 4;
      decoder->strike_index_count = FT_NEXT_ULONG( p );
       p                          += 34;
       decoder->bit_depth          = *p;
 
      /* decoder->strike_index_array +                               */
      /*   8 * decoder->strike_index_count > face->sbit_table_size ? */
      if ( decoder->strike_index_array > face->sbit_table_size           ||
           decoder->strike_index_count >
             ( face->sbit_table_size - decoder->strike_index_array ) / 8 )
         error = FT_THROW( Invalid_File_Format );
     }
  }
