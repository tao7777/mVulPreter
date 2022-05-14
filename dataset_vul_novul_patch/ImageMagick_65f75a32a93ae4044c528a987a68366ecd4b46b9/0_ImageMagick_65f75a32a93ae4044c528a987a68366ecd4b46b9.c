static MagickBooleanType WriteTGAImage(const ImageInfo *image_info,Image *image)
{
  CompressionType
     compression;
 
   const char
    *comment,
     *value;
 
   const double
    midpoint = QuantumRange/2.0;

  MagickBooleanType
    status;

  QuantumAny
    range;

  register const IndexPacket
    *indexes;

  register const PixelPacket
    *p;

  register ssize_t
    x;

  register ssize_t
    i;

  register unsigned char
    *q;

  ssize_t
    count,
    y;

  TGAInfo
    tga_info;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFalse)
    return(status);
  /*
    Initialize TGA raster file header.
  */
  if ((image->columns > 65535L) || (image->rows > 65535L))
    ThrowWriterException(ImageError,"WidthOrHeightExceedsLimit");
  (void) TransformImageColorspace(image,sRGBColorspace);
  compression=image->compression;
  if (image_info->compression != UndefinedCompression)
     compression=image_info->compression;
   range=GetQuantumRange(5UL);
   tga_info.id_length=0;
  comment=GetImageProperty(image,"comment");
  if (comment != (const char *) NULL)
    tga_info.id_length=(unsigned char) MagickMin(strlen(comment),255);
   tga_info.colormap_type=0;
   tga_info.colormap_index=0;
   tga_info.colormap_length=0;
  tga_info.colormap_size=0;
  tga_info.x_origin=0;
  tga_info.y_origin=0;
  tga_info.width=(unsigned short) image->columns;
  tga_info.height=(unsigned short) image->rows;
  tga_info.bits_per_pixel=8;
  tga_info.attributes=0;
  if ((image_info->type != TrueColorType) &&
      (image_info->type != TrueColorMatteType) &&
      (image_info->type != PaletteType) &&
      (image->matte == MagickFalse) &&
      (SetImageGray(image,&image->exception) != MagickFalse))
    tga_info.image_type=compression == RLECompression ? TGARLEMonochrome :
      TGAMonochrome;
  else
    if ((image->storage_class == DirectClass) || (image->colors > 256))
      {
        /*
          Full color TGA raster.
        */
        tga_info.image_type=compression == RLECompression ? TGARLERGB : TGARGB;
        if (image_info->depth == 5)
          {
            tga_info.bits_per_pixel=16;
            if (image->matte != MagickFalse)
              tga_info.attributes=1;  /* # of alpha bits */
          }
        else
          {
            tga_info.bits_per_pixel=24;
            if (image->matte != MagickFalse)
              {
                tga_info.bits_per_pixel=32;
                tga_info.attributes=8;  /* # of alpha bits */
              }
          }
      }
    else
      {
        /*
          Colormapped TGA raster.
        */
        tga_info.image_type=compression == RLECompression ? TGARLEColormap :
          TGAColormap;
        tga_info.colormap_type=1;
        tga_info.colormap_length=(unsigned short) image->colors;
        if (image_info->depth == 5)
          tga_info.colormap_size=16;
        else
          tga_info.colormap_size=24;
      }
  value=GetImageArtifact(image,"tga:image-origin");
  if (value != (const char *) NULL)
    {
      OrientationType
        origin;

      origin=(OrientationType) ParseCommandOption(MagickOrientationOptions,
        MagickFalse,value);
      if (origin == BottomRightOrientation || origin == TopRightOrientation)
        tga_info.attributes|=(1UL << 4);
      if (origin == TopLeftOrientation || origin == TopRightOrientation)
        tga_info.attributes|=(1UL << 5);
    }
  /*
    Write TGA header.
  */
  (void) WriteBlobByte(image,tga_info.id_length);
  (void) WriteBlobByte(image,tga_info.colormap_type);
  (void) WriteBlobByte(image,(unsigned char) tga_info.image_type);
  (void) WriteBlobLSBShort(image,tga_info.colormap_index);
  (void) WriteBlobLSBShort(image,tga_info.colormap_length);
  (void) WriteBlobByte(image,tga_info.colormap_size);
  (void) WriteBlobLSBShort(image,tga_info.x_origin);
  (void) WriteBlobLSBShort(image,tga_info.y_origin);
  (void) WriteBlobLSBShort(image,tga_info.width);
  (void) WriteBlobLSBShort(image,tga_info.height);
   (void) WriteBlobByte(image,tga_info.bits_per_pixel);
   (void) WriteBlobByte(image,tga_info.attributes);
   if (tga_info.id_length != 0)
    (void) WriteBlob(image,tga_info.id_length,(unsigned char *) comment);
   if (tga_info.colormap_type != 0)
     {
       unsigned char
        green,
        *targa_colormap;

      /*
        Dump colormap to file (blue, green, red byte order).
      */
      targa_colormap=(unsigned char *) AcquireQuantumMemory((size_t)
        tga_info.colormap_length,(tga_info.colormap_size/8)*sizeof(
        *targa_colormap));
      if (targa_colormap == (unsigned char *) NULL)
        ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
      q=targa_colormap;
      for (i=0; i < (ssize_t) image->colors; i++)
      {
        if (image_info->depth == 5)
          {
            green=(unsigned char) ScaleQuantumToAny(image->colormap[i].green,
              range);
            *q++=((unsigned char) ScaleQuantumToAny(image->colormap[i].blue,
              range)) | ((green & 0x07) << 5);
            *q++=(((image->matte != MagickFalse) && (
              (double) image->colormap[i].opacity < midpoint)) ? 0x80 : 0) |
              ((unsigned char) ScaleQuantumToAny(image->colormap[i].red,
              range) << 2) | ((green & 0x18) >> 3);
          }
        else
          {
            *q++=ScaleQuantumToChar(image->colormap[i].blue);
            *q++=ScaleQuantumToChar(image->colormap[i].green);
            *q++=ScaleQuantumToChar(image->colormap[i].red);
          }
      }
      (void) WriteBlob(image,(size_t) ((tga_info.colormap_size/8)*
        tga_info.colormap_length),targa_colormap);
      targa_colormap=(unsigned char *) RelinquishMagickMemory(targa_colormap);
    }
  /*
    Convert MIFF to TGA raster pixels.
  */
  for (y=(ssize_t) (image->rows-1); y >= 0; y--)
  {
    p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=GetVirtualIndexQueue(image);
    if (compression == RLECompression)
      {
        x=0;
        count=0;
        while (x < (ssize_t) image->columns)
        {
          i=1;
          while ((i < 128) && (count + i < 128) &&
                 ((x + i) < (ssize_t) image->columns))
          {
            if (tga_info.image_type == TGARLEColormap)
              {
                if (GetPixelIndex(indexes+i) != GetPixelIndex(indexes+(i-1)))
                  break;
              }
            else
              if (tga_info.image_type == TGARLEMonochrome)
                {
                  if (GetPixelLuma(image,p+i) != GetPixelLuma(image,p+(i-1)))
                    break;
                }
              else
                {
                  if ((GetPixelBlue(p+i) != GetPixelBlue(p+(i-1))) ||
                      (GetPixelGreen(p+i) != GetPixelGreen(p+(i-1))) ||
                      (GetPixelRed(p+i) != GetPixelRed(p+(i-1))))
                    break;
                  if ((image->matte != MagickFalse) &&
                      (GetPixelAlpha(p+i) != GetPixelAlpha(p+(i-1))))
                    break;
                }
            i++;
          }
          if (i < 3)
            {
              count+=i;
              p+=i;
              indexes+=i;
            }
          if ((i >= 3) || (count == 128) ||
              ((x + i) == (ssize_t) image->columns))
            {
              if (count > 0)
                {
                  (void) WriteBlobByte(image,(unsigned char) (--count));
                  while (count >= 0)
                  {
                    WriteTGAPixel(image,tga_info.image_type,indexes-(count+1),
                      p-(count+1),range,midpoint);
                    count--;
                  }
                  count=0;
                }
            }
          if (i >= 3)
            {
              (void) WriteBlobByte(image,(unsigned char) ((i-1) | 0x80));
              WriteTGAPixel(image,tga_info.image_type,indexes,p,range,midpoint);
              p+=i;
              indexes+=i;
            }
          x+=i;
        }
      }
    else
      {
        for (x=0; x < (ssize_t) image->columns; x++)
          WriteTGAPixel(image,tga_info.image_type,indexes+x,p++,range,midpoint);
      }
    if (image->previous == (Image *) NULL)
      {
        status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
          image->rows);
        if (status == MagickFalse)
          break;
      }
  }
  (void) CloseBlob(image);
  return(MagickTrue);
}
