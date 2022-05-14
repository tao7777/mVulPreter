  pcf_read_TOC( FT_Stream  stream,
                PCF_Face   face )
  {
    FT_Error   error;
    PCF_Toc    toc = &face->toc;
    PCF_Table  tables;

    FT_Memory  memory = FT_FACE( face )->memory;
    FT_UInt    n;


    if ( FT_STREAM_SEEK ( 0 )                          ||
         FT_STREAM_READ_FIELDS ( pcf_toc_header, toc ) )
      return FT_THROW( Cannot_Open_Resource );

    if ( toc->version != PCF_FILE_VERSION                 ||
         toc->count   >  FT_ARRAY_MAX( face->toc.tables ) ||
         toc->count   == 0                                )
      return FT_THROW( Invalid_File_Format );

    if ( FT_NEW_ARRAY( face->toc.tables, toc->count ) )
      return FT_THROW( Out_Of_Memory );

    tables = face->toc.tables;
    for ( n = 0; n < toc->count; n++ )
    {
      if ( FT_STREAM_READ_FIELDS( pcf_table_header, tables ) )
        goto Exit;
      tables++;
    }

    /* Sort tables and check for overlaps.  Because they are almost      */
    /* always ordered already, an in-place bubble sort with simultaneous */
    /* boundary checking seems appropriate.                              */
    tables = face->toc.tables;

    for ( n = 0; n < toc->count - 1; n++ )
    {
      FT_UInt  i, have_change;


      have_change = 0;

      for ( i = 0; i < toc->count - 1 - n; i++ )
      {
        PCF_TableRec  tmp;


        if ( tables[i].offset > tables[i + 1].offset )
        {
          tmp           = tables[i];
          tables[i]     = tables[i + 1];
          tables[i + 1] = tmp;

          have_change = 1;
        }

        if ( ( tables[i].size   > tables[i + 1].offset )                  ||
             ( tables[i].offset > tables[i + 1].offset - tables[i].size ) )
        {
          error = FT_THROW( Invalid_Offset );
          goto Exit;
        }
      }

      if ( !have_change )
         break;
     }
 
    /* we now check whether the `size' and `offset' values are reasonable: */
    /* `offset' + `size' must not exceed the stream size                   */
    tables = face->toc.tables;
    for ( n = 0; n < toc->count; n++ )
    {
      /* we need two checks to avoid overflow */
      if ( ( tables->size   > stream->size                ) ||
           ( tables->offset > stream->size - tables->size ) )
      {
        error = FT_THROW( Invalid_Table );
        goto Exit;
      }
      tables++;
    }

 #ifdef FT_DEBUG_LEVEL_TRACE
 
     {

        FT_TRACE4(( "  %d: type=%s, format=0x%X, "
                    "size=%ld (0x%lX), offset=%ld (0x%lX)\n",
                    i, name,
                    tables[i].format,
                    tables[i].size, tables[i].size,
                    tables[i].offset, tables[i].offset ));
      }
    }
