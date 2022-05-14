  FT_Bitmap_Convert( FT_Library        library,
                     const FT_Bitmap  *source,
                     FT_Bitmap        *target,
                     FT_Int            alignment )
  {
    FT_Error   error = FT_Err_Ok;
    FT_Memory  memory;

    FT_Int  source_pitch, target_pitch;


    if ( !library )
      return FT_THROW( Invalid_Library_Handle );

    memory = library->memory;

    switch ( source->pixel_mode )
    {
    case FT_PIXEL_MODE_MONO:
    case FT_PIXEL_MODE_GRAY:
    case FT_PIXEL_MODE_GRAY2:
    case FT_PIXEL_MODE_GRAY4:
    case FT_PIXEL_MODE_LCD:
    case FT_PIXEL_MODE_LCD_V:
     case FT_PIXEL_MODE_LCD_V:
     case FT_PIXEL_MODE_BGRA:
       {
        FT_Int    pad, old_target_pitch;
        FT_ULong  old_size;
 
 
         old_target_pitch = target->pitch;
          old_target_pitch = -old_target_pitch;

        old_size = target->rows * old_target_pitch;

        target->pixel_mode = FT_PIXEL_MODE_GRAY;
        target->rows       = source->rows;
        target->width      = source->width;

        pad = 0;
        if ( alignment > 0 )
        {
          pad = source->width % alignment;
          if ( pad != 0 )
            pad = alignment - pad;
        }

        target_pitch = source->width + pad;

        if ( target_pitch > 0                                     &&
             (FT_ULong)target->rows > FT_ULONG_MAX / target_pitch )
          return FT_THROW( Invalid_Argument );

        if ( target->rows * target_pitch > old_size               &&
             FT_QREALLOC( target->buffer,
                          old_size, target->rows * target_pitch ) )
          return error;

        target->pitch = target->pitch < 0 ? -target_pitch : target_pitch;
      }
      break;

    default:
      error = FT_THROW( Invalid_Argument );
    }

    source_pitch = source->pitch;
    if ( source_pitch < 0 )
      source_pitch = -source_pitch;

    switch ( source->pixel_mode )
    {
    case FT_PIXEL_MODE_MONO:
      {
        FT_Byte*  s = source->buffer;
        FT_Byte*  t = target->buffer;
        FT_Int    i;


        target->num_grays = 2;

        for ( i = source->rows; i > 0; i-- )
        {
          FT_Byte*  ss = s;
          FT_Byte*  tt = t;
          FT_Int    j;


          /* get the full bytes */
          for ( j = source->width >> 3; j > 0; j-- )
          {
            FT_Int  val = ss[0]; /* avoid a byte->int cast on each line */


            tt[0] = (FT_Byte)( ( val & 0x80 ) >> 7 );
            tt[1] = (FT_Byte)( ( val & 0x40 ) >> 6 );
            tt[2] = (FT_Byte)( ( val & 0x20 ) >> 5 );
            tt[3] = (FT_Byte)( ( val & 0x10 ) >> 4 );
            tt[4] = (FT_Byte)( ( val & 0x08 ) >> 3 );
            tt[5] = (FT_Byte)( ( val & 0x04 ) >> 2 );
            tt[6] = (FT_Byte)( ( val & 0x02 ) >> 1 );
            tt[7] = (FT_Byte)(   val & 0x01 );

            tt += 8;
            ss += 1;
          }

          /* get remaining pixels (if any) */
          j = source->width & 7;
          if ( j > 0 )
          {
            FT_Int  val = *ss;


            for ( ; j > 0; j-- )
            {
              tt[0] = (FT_Byte)( ( val & 0x80 ) >> 7);
              val <<= 1;
              tt   += 1;
            }
          }

          s += source_pitch;
          t += target_pitch;
        }
      }
      break;


    case FT_PIXEL_MODE_GRAY:
    case FT_PIXEL_MODE_LCD:
    case FT_PIXEL_MODE_LCD_V:
      {
        FT_Int    width = source->width;
        FT_Byte*  s     = source->buffer;
        FT_Byte*  t     = target->buffer;
        FT_Int    i;


        target->num_grays = 256;

        for ( i = source->rows; i > 0; i-- )
        {
          FT_ARRAY_COPY( t, s, width );

          s += source_pitch;
          t += target_pitch;
        }
      }
      break;


    case FT_PIXEL_MODE_GRAY2:
      {
        FT_Byte*  s = source->buffer;
        FT_Byte*  t = target->buffer;
        FT_Int    i;


        target->num_grays = 4;

        for ( i = source->rows; i > 0; i-- )
        {
          FT_Byte*  ss = s;
          FT_Byte*  tt = t;
          FT_Int    j;


          /* get the full bytes */
          for ( j = source->width >> 2; j > 0; j-- )
          {
            FT_Int  val = ss[0];


            tt[0] = (FT_Byte)( ( val & 0xC0 ) >> 6 );
            tt[1] = (FT_Byte)( ( val & 0x30 ) >> 4 );
            tt[2] = (FT_Byte)( ( val & 0x0C ) >> 2 );
            tt[3] = (FT_Byte)( ( val & 0x03 ) );

            ss += 1;
            tt += 4;
          }

          j = source->width & 3;
          if ( j > 0 )
          {
            FT_Int  val = ss[0];


            for ( ; j > 0; j-- )
            {
              tt[0]  = (FT_Byte)( ( val & 0xC0 ) >> 6 );
              val  <<= 2;
              tt    += 1;
            }
          }

          s += source_pitch;
          t += target_pitch;
        }
      }
      break;


    case FT_PIXEL_MODE_GRAY4:
      {
        FT_Byte*  s = source->buffer;
        FT_Byte*  t = target->buffer;
        FT_Int    i;


        target->num_grays = 16;

        for ( i = source->rows; i > 0; i-- )
        {
          FT_Byte*  ss = s;
          FT_Byte*  tt = t;
          FT_Int    j;


          /* get the full bytes */
          for ( j = source->width >> 1; j > 0; j-- )
          {
            FT_Int  val = ss[0];


            tt[0] = (FT_Byte)( ( val & 0xF0 ) >> 4 );
            tt[1] = (FT_Byte)( ( val & 0x0F ) );

            ss += 1;
            tt += 2;
          }

          if ( source->width & 1 )
            tt[0] = (FT_Byte)( ( ss[0] & 0xF0 ) >> 4 );

          s += source_pitch;
          t += target_pitch;
        }
      }
      break;


    case FT_PIXEL_MODE_BGRA:
      {
        FT_Byte*  s = source->buffer;
        FT_Byte*  t = target->buffer;
        FT_Int    i;


        target->num_grays = 256;

        for ( i = source->rows; i > 0; i-- )
        {
          FT_Byte*  ss = s;
          FT_Byte*  tt = t;
          FT_Int    j;


          for ( j = source->width; j > 0; j-- )
          {
            tt[0] = ft_gray_for_premultiplied_srgb_bgra( ss );

            ss += 4;
            tt += 1;
          }

          s += source_pitch;
          t += target_pitch;
        }
      }
      break;

    default:
      ;
    }

    return error;
  }
