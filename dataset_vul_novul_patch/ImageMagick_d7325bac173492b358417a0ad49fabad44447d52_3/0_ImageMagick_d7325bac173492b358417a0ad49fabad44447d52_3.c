 static MagickBooleanType ReadUncompressedRGB(Image *image, DDSInfo *dds_info,
  ExceptionInfo *exception)
{
  PixelPacket
    *q;

  ssize_t
    x, y;

  unsigned short
    color;

  if (dds_info->pixelformat.rgb_bitcount == 8)
    (void) SetImageType(image,GrayscaleType);
  else if (dds_info->pixelformat.rgb_bitcount == 16 && !IsBitMask(
    dds_info->pixelformat,0xf800,0x07e0,0x001f,0x0000))
    ThrowBinaryException(CorruptImageError,"ImageTypeNotSupported",
      image->filename);

  for (y = 0; y < (ssize_t) dds_info->height; y++)
  {
    q = QueueAuthenticPixels(image, 0, y, dds_info->width, 1,exception);

    if (q == (PixelPacket *) NULL)
      return MagickFalse;

    for (x = 0; x < (ssize_t) dds_info->width; x++)
    {
      if (dds_info->pixelformat.rgb_bitcount == 8)
        SetPixelGray(q,ScaleCharToQuantum(ReadBlobByte(image)));
      else if (dds_info->pixelformat.rgb_bitcount == 16)
        {
           color=ReadBlobShort(image);
           SetPixelRed(q,ScaleCharToQuantum((unsigned char)
             (((color >> 11)/31.0)*255)));
           SetPixelGreen(q,ScaleCharToQuantum((unsigned char)
             ((((unsigned short)(color << 5) >> 10)/63.0)*255)));
           SetPixelBlue(q,ScaleCharToQuantum((unsigned char)
             ((((unsigned short)(color << 11) >> 11)/31.0)*255)));
        }
      else
        {
          SetPixelBlue(q,ScaleCharToQuantum((unsigned char)
            ReadBlobByte(image)));
          SetPixelGreen(q,ScaleCharToQuantum((unsigned char)
            ReadBlobByte(image)));
          SetPixelRed(q,ScaleCharToQuantum((unsigned char)
            ReadBlobByte(image)));
          if (dds_info->pixelformat.rgb_bitcount == 32)
            (void) ReadBlobByte(image);
        }
      SetPixelAlpha(q,QuantumRange);
      q++;
    }
 
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
       return MagickFalse;
   }
 
  return(SkipRGBMipmaps(image,dds_info,3,exception));
 }
