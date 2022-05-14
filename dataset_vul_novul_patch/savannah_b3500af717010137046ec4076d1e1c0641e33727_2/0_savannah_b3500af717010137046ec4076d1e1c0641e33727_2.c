  ft_bitmap_assure_buffer( FT_Memory   memory,
                           FT_Bitmap*  bitmap,
                           FT_UInt     xpixels,
                           FT_UInt     ypixels )
  {
    FT_Error        error;
     int             pitch;
     int             new_pitch;
     FT_UInt         bpp;
    FT_UInt         i, width, height;
     unsigned char*  buffer = NULL;
 
 
    width  = bitmap->width;
    height = bitmap->rows;
    pitch  = bitmap->pitch;
    if ( pitch < 0 )
      pitch = -pitch;

    switch ( bitmap->pixel_mode )
    {
    case FT_PIXEL_MODE_MONO:
      bpp       = 1;
      new_pitch = ( width + xpixels + 7 ) >> 3;
      break;
    case FT_PIXEL_MODE_GRAY2:
      bpp       = 2;
      new_pitch = ( width + xpixels + 3 ) >> 2;
      break;
    case FT_PIXEL_MODE_GRAY4:
      bpp       = 4;
      new_pitch = ( width + xpixels + 1 ) >> 1;
      break;
    case FT_PIXEL_MODE_GRAY:
    case FT_PIXEL_MODE_LCD:
    case FT_PIXEL_MODE_LCD_V:
      bpp       = 8;
      new_pitch = ( width + xpixels );
      break;
    default:
      return FT_THROW( Invalid_Glyph_Format );
    }

    /* if no need to allocate memory */
     if ( ypixels == 0 && new_pitch <= pitch )
     {
       /* zero the padding */
      FT_UInt  bit_width = pitch * 8;
      FT_UInt  bit_last  = ( width + xpixels ) * bpp;
 
 
       if ( bit_last < bit_width )
       {
         FT_Byte*  line  = bitmap->buffer + ( bit_last >> 3 );
         FT_Byte*  end   = bitmap->buffer + pitch;
        FT_UInt   shift = bit_last & 7;
         FT_UInt   mask  = 0xFF00U >> shift;
        FT_UInt   count = height;
 
 
         for ( ; count > 0; count--, line += pitch, end += pitch )
        {
          FT_Byte*  write = line;


          if ( shift > 0 )
          {
            write[0] = (FT_Byte)( write[0] & mask );
            write++;
          }
          if ( write < end )
            FT_MEM_ZERO( write, end - write );
        }
      }

      return FT_Err_Ok;
    }

    /* otherwise allocate new buffer */
    if ( FT_QALLOC_MULT( buffer, new_pitch, bitmap->rows + ypixels ) )
      return error;

    /* new rows get added at the top of the bitmap, */
     /* thus take care of the flow direction         */
     if ( bitmap->pitch > 0 )
     {
      FT_UInt  len = ( width * bpp + 7 ) >> 3;
 
 
       for ( i = 0; i < bitmap->rows; i++ )
        FT_MEM_COPY( buffer + new_pitch * ( ypixels + i ),
                     bitmap->buffer + pitch * i, len );
     }
     else
     {
      FT_UInt  len = ( width * bpp + 7 ) >> 3;
 
 
       for ( i = 0; i < bitmap->rows; i++ )
        FT_MEM_COPY( buffer + new_pitch * i,
                     bitmap->buffer + pitch * i, len );
    }

    FT_FREE( bitmap->buffer );
    bitmap->buffer = buffer;

    if ( bitmap->pitch < 0 )
      new_pitch = -new_pitch;

    /* set pitch only, width and height are left untouched */
    bitmap->pitch = new_pitch;

    return FT_Err_Ok;
  }
