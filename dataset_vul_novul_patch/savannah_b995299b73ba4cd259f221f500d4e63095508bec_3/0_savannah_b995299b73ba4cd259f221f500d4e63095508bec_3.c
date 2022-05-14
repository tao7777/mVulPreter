  write_header( FT_Error  error_code )
  {
    FT_Face      face;
    const char*  basename;
    const char*  format;


    error = FTC_Manager_LookupFace( handle->cache_manager,
                                    handle->scaler.face_id, &face );
    if ( error )
      Fatal( "can't access font file" );

    if ( !status.header )
    {
      basename = ft_basename( handle->current_font->filepathname );

       switch ( error_code )
       {
       case FT_Err_Ok:
        sprintf( status.header_buffer, "%.50s %.50s (file `%.100s')",
                  face->family_name, face->style_name, basename );
         break;
       case FT_Err_Invalid_Pixel_Size:
        sprintf( status.header_buffer, "Invalid pixel size (file `%.100s')",
                  basename );
         break;
       case FT_Err_Invalid_PPem:
        sprintf( status.header_buffer, "Invalid ppem value (file `%.100s')",
                  basename );
         break;
       default:
        sprintf( status.header_buffer, "File `%.100s': error 0x%04x",
                  basename, (FT_UShort)error_code );
         break;
       }

      status.header = status.header_buffer;
    }

    grWriteCellString( display->bitmap, 0, 0,
                       status.header, display->fore_color );

    format = status.encoding != FT_ENCODING_NONE
             ? "at %g points, first char code = 0x%x"
             : "at %g points, first glyph index = %d";

    snprintf( status.header_buffer, 256, format,
              status.ptsize / 64.0, status.Num );

    if ( FT_HAS_GLYPH_NAMES( face ) )
    {
      char*  p;
      int    format_len, gindex, size;


      size = strlen( status.header_buffer );
      p    = status.header_buffer + size;
      size = 256 - size;

      format = ", name = ";
      format_len = strlen( format );

      if ( size >= format_len + 2 )
      {
        gindex = status.Num;
        if ( status.encoding != FT_ENCODING_NONE )
          gindex = FTDemo_Get_Index( handle, status.Num );

        strcpy( p, format );
        if ( FT_Get_Glyph_Name( face, gindex,
                                p + format_len, size - format_len ) )
          *p = '\0';
      }
    }

    status.header = status.header_buffer;
    grWriteCellString( display->bitmap, 0, HEADER_HEIGHT,
                       status.header_buffer, display->fore_color );

    if ( status.use_custom_lcd_filter )
    {
      int             fwi = status.fw_index;
      unsigned char  *fw  = status.filter_weights;


      sprintf( status.header_buffer,
               "%s0x%02X%s%s0x%02X%s%s0x%02X%s%s0x%02X%s%s0x%02X%s",
               fwi == 0 ? "[" : " ", fw[0], fwi == 0 ? "]" : " ",
               fwi == 1 ? "[" : " ", fw[1], fwi == 1 ? "]" : " ",
               fwi == 2 ? "[" : " ", fw[2], fwi == 2 ? "]" : " ",
               fwi == 3 ? "[" : " ", fw[3], fwi == 3 ? "]" : " ",
               fwi == 4 ? "[" : " ", fw[4], fwi == 4 ? "]" : " " );
      grWriteCellString( display->bitmap, 0, 2 * HEADER_HEIGHT,
                         status.header_buffer, display->fore_color );
    }

    grRefreshSurface( display->surface );
  }
