  write_header( FT_Error  error_code )
  {
    FT_Face      face;
    const char*  basename;


    error = FTC_Manager_LookupFace( handle->cache_manager,
                                    handle->scaler.face_id, &face );
    if ( error )
      PanicZ( "can't access font file" );

    if ( !status.header )
    {
      basename = ft_basename( handle->current_font->filepathname );

       switch ( error_code )
       {
       case FT_Err_Ok:
        sprintf( status.header_buffer, "%s %s (file `%s')", face->family_name,
                  face->style_name, basename );
         break;
       case FT_Err_Invalid_Pixel_Size:
        sprintf( status.header_buffer, "Invalid pixel size (file `%s')",
                  basename );
         break;
       case FT_Err_Invalid_PPem:
        sprintf( status.header_buffer, "Invalid ppem value (file `%s')",
                  basename );
         break;
       default:
        sprintf( status.header_buffer, "File `%s': error 0x%04x", basename,
             (FT_UShort)error_code );
         break;
       }
      status.header = status.header_buffer;
    }

    grWriteCellString( display->bitmap, 0, 0,
                       status.header, display->fore_color );

    sprintf( status.header_buffer, "at %g points, angle = %d",
             status.ptsize/64.0, status.angle );
    grWriteCellString( display->bitmap, 0, CELLSTRING_HEIGHT,
                       status.header_buffer, display->fore_color );

    grRefreshSurface( display->surface );
  }
