static MagickBooleanType ReadDXT1(Image *image, DDSInfo *dds_info,
  ExceptionInfo *exception)
{
  DDSColors
    colors;

  PixelPacket
    *q;

  register ssize_t
    i,
    x;

  size_t
    bits;

  ssize_t
    j,
    y;

  unsigned char
    code;

  unsigned short
    c0,
    c1;

  for (y = 0; y < (ssize_t) dds_info->height; y += 4)
  {
     for (x = 0; x < (ssize_t) dds_info->width; x += 4)
     {
       /* Get 4x4 patch of pixels to write on */
      q = QueueAuthenticPixels(image, x, y, MagickMin(4, dds_info->width - x),
        MagickMin(4, dds_info->height - y),exception);
 
       if (q == (PixelPacket *) NULL)
         return MagickFalse;

      /* Read 8 bytes of data from the image */
      c0 = ReadBlobLSBShort(image);
      c1 = ReadBlobLSBShort(image);
      bits = ReadBlobLSBLong(image);

      CalculateColors(c0, c1, &colors, MagickFalse);

      /* Write the pixels */
      for (j = 0; j < 4; j++)
      {
        for (i = 0; i < 4; i++)
        {
          if ((x + i) < (ssize_t) dds_info->width && (y + j) < (ssize_t) dds_info->height)
            {
              code = (unsigned char) ((bits >> ((j*4+i)*2)) & 0x3);
              SetPixelRed(q,ScaleCharToQuantum(colors.r[code]));
              SetPixelGreen(q,ScaleCharToQuantum(colors.g[code]));
              SetPixelBlue(q,ScaleCharToQuantum(colors.b[code]));
              SetPixelOpacity(q,ScaleCharToQuantum(colors.a[code]));
              if (colors.a[code] && image->matte == MagickFalse)
                /* Correct matte */
                image->matte = MagickTrue;
              q++;
            }
        }
      }

      if (SyncAuthenticPixels(image,exception) == MagickFalse)
        return MagickFalse;
     }
   }
 
  return(SkipDXTMipmaps(image,dds_info,8,exception));
 }
