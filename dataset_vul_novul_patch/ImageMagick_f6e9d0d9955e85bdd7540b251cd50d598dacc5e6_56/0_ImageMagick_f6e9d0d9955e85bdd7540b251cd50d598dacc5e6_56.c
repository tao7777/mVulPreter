static Image *ReadSCTImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    magick[2];

  Image
    *image;

  MagickBooleanType
    status;

  MagickRealType
    height,
    width;

  Quantum
    pixel;

  register IndexPacket
    *indexes;

  register ssize_t
    i,
    x;

  register PixelPacket
    *q;

  ssize_t
    count,
    y;

  unsigned char
    buffer[768];

  size_t
    separations,
    separations_mask,
    units;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AcquireImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Read control block.
  */
  count=ReadBlob(image,80,buffer);
  (void) count;
  count=ReadBlob(image,2,(unsigned char *) magick);
  if ((LocaleNCompare((char *) magick,"CT",2) != 0) &&
      (LocaleNCompare((char *) magick,"LW",2) != 0) &&
      (LocaleNCompare((char *) magick,"BM",2) != 0) &&
      (LocaleNCompare((char *) magick,"PG",2) != 0) &&
      (LocaleNCompare((char *) magick,"TX",2) != 0))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  if ((LocaleNCompare((char *) magick,"LW",2) == 0) ||
      (LocaleNCompare((char *) magick,"BM",2) == 0) ||
      (LocaleNCompare((char *) magick,"PG",2) == 0) ||
      (LocaleNCompare((char *) magick,"TX",2) == 0))
    ThrowReaderException(CoderError,"OnlyContinuousTonePictureSupported");
  count=ReadBlob(image,174,buffer);
  count=ReadBlob(image,768,buffer);
  /*
    Read paramter block.
  */
  units=1UL*ReadBlobByte(image);
  if (units == 0)
    image->units=PixelsPerCentimeterResolution;
  separations=1UL*ReadBlobByte(image);
  separations_mask=ReadBlobMSBShort(image);
  count=ReadBlob(image,14,buffer);
  buffer[14]='\0';
  height=StringToDouble((char *) buffer,(char **) NULL);
  count=ReadBlob(image,14,buffer);
  width=StringToDouble((char *) buffer,(char **) NULL);
  count=ReadBlob(image,12,buffer);
  buffer[12]='\0';
  image->rows=StringToUnsignedLong((char *) buffer);
  count=ReadBlob(image,12,buffer);
  image->columns=StringToUnsignedLong((char *) buffer);
  count=ReadBlob(image,200,buffer);
  count=ReadBlob(image,768,buffer);
  if (separations_mask == 0x0f)
    SetImageColorspace(image,CMYKColorspace);
  image->x_resolution=1.0*image->columns/width;
  image->y_resolution=1.0*image->rows/height;
  if (image_info->ping != MagickFalse)
    {
       (void) CloseBlob(image);
       return(GetFirstImageInList(image));
     }
  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }
   /*
     Convert SCT raster image to pixel packets.
   */
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    for (i=0; i < (ssize_t) separations; i++)
    {
      q=GetAuthenticPixels(image,0,y,image->columns,1,exception);
      if (q == (PixelPacket *) NULL)
        break;
      indexes=GetAuthenticIndexQueue(image);
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        pixel=(Quantum) ScaleCharToQuantum((unsigned char) ReadBlobByte(image));
        if (image->colorspace == CMYKColorspace)
          pixel=(Quantum) (QuantumRange-pixel);
        switch (i)
        {
          case 0:
          {
            SetPixelRed(q,pixel);
            SetPixelGreen(q,pixel);
            SetPixelBlue(q,pixel);
            break;
          }
          case 1:
          {
            SetPixelGreen(q,pixel);
            break;
          }
          case 2:
          {
            SetPixelBlue(q,pixel);
            break;
          }
          case 3:
          {
            if (image->colorspace == CMYKColorspace)
              SetPixelBlack(indexes+x,pixel);
            break;
          }
        }
        q++;
      }
      if (SyncAuthenticPixels(image,exception) == MagickFalse)
        break;
      if ((image->columns % 2) != 0)
        (void) ReadBlobByte(image);  /* pad */
    }
    status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  if (EOFBlob(image) != MagickFalse)
    ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
      image->filename);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
