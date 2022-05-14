static MagickBooleanType WriteMAPImage(const ImageInfo *image_info,Image *image)
{
  MagickBooleanType
    status;

  register const IndexPacket
    *indexes;

  register const PixelPacket
    *p;

  register ssize_t
    i,
    x;

  register unsigned char
    *q;

  size_t
    depth,
    packet_size;

  ssize_t
    y;

  unsigned char
    *colormap,
    *pixels;

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
  (void) TransformImageColorspace(image,sRGBColorspace);
  /*
    Allocate colormap.
  */
  if (IsPaletteImage(image,&image->exception) == MagickFalse)
    (void) SetImageType(image,PaletteType);
  depth=GetImageQuantumDepth(image,MagickTrue);
  packet_size=(size_t) (depth/8);
  pixels=(unsigned char *) AcquireQuantumMemory(image->columns,packet_size*
    sizeof(*pixels));
  packet_size=(size_t) (image->colors > 256 ? 6UL : 3UL);
  colormap=(unsigned char *) AcquireQuantumMemory(image->colors,packet_size*
     sizeof(*colormap));
   if ((pixels == (unsigned char *) NULL) ||
       (colormap == (unsigned char *) NULL))
    {
      if (colormap != (unsigned char *) NULL)
        colormap=(unsigned char *) RelinquishMagickMemory(colormap);
      if (pixels != (unsigned char *) NULL)
        pixels=(unsigned char *) RelinquishMagickMemory(pixels);
      ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
    }
   /*
     Write colormap to file.
   */
   q=colormap;
   if (image->colors <= 256)
     for (i=0; i < (ssize_t) image->colors; i++)
     {
      *q++=(unsigned char) ScaleQuantumToChar(image->colormap[i].red);
      *q++=(unsigned char) ScaleQuantumToChar(image->colormap[i].green);
      *q++=(unsigned char) ScaleQuantumToChar(image->colormap[i].blue);
    }
  else
    for (i=0; i < (ssize_t) image->colors; i++)
    {
       *q++=(unsigned char) (ScaleQuantumToShort(image->colormap[i].red) >> 8);
       *q++=(unsigned char) (ScaleQuantumToShort(image->colormap[i].red) & 0xff);
       *q++=(unsigned char) (ScaleQuantumToShort(image->colormap[i].green) >> 8);
      *q++=(unsigned char) (ScaleQuantumToShort(image->colormap[i].green) &
        0xff);
       *q++=(unsigned char) (ScaleQuantumToShort(image->colormap[i].blue) >> 8);
      *q++=(unsigned char) (ScaleQuantumToShort(image->colormap[i].blue) &
        0xff);
     }
   (void) WriteBlob(image,packet_size*image->colors,colormap);
   colormap=(unsigned char *) RelinquishMagickMemory(colormap);
  /*
    Write image pixels to file.
  */
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=GetVirtualIndexQueue(image);
    q=pixels;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if (image->colors > 256)
        *q++=(unsigned char) ((size_t) GetPixelIndex(indexes+x) >> 8);
      *q++=(unsigned char) GetPixelIndex(indexes+x);
    }
    (void) WriteBlob(image,(size_t) (q-pixels),pixels);
  }
  pixels=(unsigned char *) RelinquishMagickMemory(pixels);
  (void) CloseBlob(image);
  return(status);
}
