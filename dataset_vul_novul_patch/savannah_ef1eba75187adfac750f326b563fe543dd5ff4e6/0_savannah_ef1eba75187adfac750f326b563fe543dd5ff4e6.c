  pcf_get_encodings( FT_Stream  stream,
                     PCF_Face   face )
  {
    FT_Error      error;
    FT_Memory     memory = FT_FACE( face )->memory;
    FT_ULong      format, size;
    int           firstCol, lastCol;
    int           firstRow, lastRow;
    int           nencoding, encodingOffset;
    int           i, j, k;
    PCF_Encoding  encoding = NULL;


    error = pcf_seek_to_table_type( stream,
                                    face->toc.tables,
                                    face->toc.count,
                                    PCF_BDF_ENCODINGS,
                                    &format,
                                    &size );
    if ( error )
      return error;

    error = FT_Stream_EnterFrame( stream, 14 );
    if ( error )
      return error;

    format = FT_GET_ULONG_LE();

    if ( PCF_BYTE_ORDER( format ) == MSBFirst )
    {
      firstCol          = FT_GET_SHORT();
      lastCol           = FT_GET_SHORT();
      firstRow          = FT_GET_SHORT();
      lastRow           = FT_GET_SHORT();
      face->defaultChar = FT_GET_SHORT();
    }
    else
    {
      firstCol          = FT_GET_SHORT_LE();
      lastCol           = FT_GET_SHORT_LE();
      firstRow          = FT_GET_SHORT_LE();
      lastRow           = FT_GET_SHORT_LE();
      face->defaultChar = FT_GET_SHORT_LE();
    }

    FT_Stream_ExitFrame( stream );

     if ( !PCF_FORMAT_MATCH( format, PCF_DEFAULT_FORMAT ) )
       return FT_THROW( Invalid_File_Format );
 
    /* sanity checks */
    if ( firstCol < 0       ||
         firstCol > lastCol ||
         lastCol  > 0xFF    ||
         firstRow < 0       ||
         firstRow > lastRow ||
         lastRow  > 0xFF    )
      return FT_THROW( Invalid_Table );

     FT_TRACE4(( "pdf_get_encodings:\n" ));
 
     FT_TRACE4(( "  firstCol %d, lastCol %d, firstRow %d, lastRow %d\n",
      goto Bail;

    k = 0;
    for ( i = firstRow; i <= lastRow; i++ )
    {
      for ( j = firstCol; j <= lastCol; j++ )
      {
        if ( PCF_BYTE_ORDER( format ) == MSBFirst )
          encodingOffset = FT_GET_SHORT();
        else
          encodingOffset = FT_GET_SHORT_LE();

        if ( encodingOffset != -1 )
        {
          encoding[k].enc   = i * 256 + j;
          encoding[k].glyph = (FT_Short)encodingOffset;

          FT_TRACE5(( "  code %d (0x%04X): idx %d\n",
                      encoding[k].enc, encoding[k].enc, encoding[k].glyph ));

          k++;
        }
      }
    }
    FT_Stream_ExitFrame( stream );

    if ( FT_RENEW_ARRAY( encoding, nencoding, k ) )
      goto Bail;

    face->nencodings = k;
    face->encodings  = encoding;

    return error;

  Bail:
    FT_FREE( encoding );
    return error;
  }
