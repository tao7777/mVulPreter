static Image *ReadPIXImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  IndexPacket
    index;

  MagickBooleanType
    status;

  Quantum
    blue,
    green,
    red;

  register IndexPacket
    *indexes;

  register ssize_t
    x;

  register PixelPacket
    *q;

  size_t
    bits_per_pixel,
    height,
    length,
    width;

  ssize_t
    y;

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
    Read PIX image.
  */
  width=ReadBlobMSBShort(image);
  height=ReadBlobMSBShort(image);
  (void) ReadBlobMSBShort(image);  /* x-offset */
  (void) ReadBlobMSBShort(image);  /* y-offset */
  bits_per_pixel=ReadBlobMSBShort(image);
  if ((width == 0UL) || (height == 0UL) || ((bits_per_pixel != 8) &&
      (bits_per_pixel != 24)))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  do
  {
    /*
      Initialize image structure.
    */
    image->columns=width;
    image->rows=height;
    if (bits_per_pixel == 8)
      if (AcquireImageColormap(image,256) == MagickFalse)
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
     if ((image_info->ping != MagickFalse) && (image_info->number_scenes != 0))
       if (image->scene >= (image_info->scene+image_info->number_scenes-1))
         break;
    status=SetImageExtent(image,image->columns,image->rows);
    if (status == MagickFalse)
      {
        InheritException(exception,&image->exception);
        return(DestroyImageList(image));
      }
     /*
       Convert PIX raster image to pixel packets.
     */
    red=(Quantum) 0;
    green=(Quantum) 0;
    blue=(Quantum) 0;
    index=(IndexPacket) 0;
    length=0;
    for (y=0; y < (ssize_t) image->rows; y++)
    {
      q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
      if (q == (PixelPacket *) NULL)
        break;
      indexes=GetAuthenticIndexQueue(image);
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        if (length == 0)
          {
            length=(size_t) ReadBlobByte(image);
            if (bits_per_pixel == 8)
              index=ScaleCharToQuantum((unsigned char) ReadBlobByte(image));
            else
              {
                blue=ScaleCharToQuantum((unsigned char) ReadBlobByte(image));
                green=ScaleCharToQuantum((unsigned char) ReadBlobByte(image));
                red=ScaleCharToQuantum((unsigned char) ReadBlobByte(image));
              }
          }
        if (image->storage_class == PseudoClass)
          SetPixelIndex(indexes+x,index);
        SetPixelBlue(q,blue);
        SetPixelGreen(q,green);
        SetPixelRed(q,red);
        length--;
        q++;
      }
      if (SyncAuthenticPixels(image,exception) == MagickFalse)
        break;
      if (image->previous == (Image *) NULL)
        {
          status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
            image->rows);
          if (status == MagickFalse)
            break;
        }
    }
    if (image->storage_class == PseudoClass)
      (void) SyncImage(image);
    if (EOFBlob(image) != MagickFalse)
      {
        ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
          image->filename);
        break;
      }
    /*
      Proceed to next image.
    */
    if (image_info->number_scenes != 0)
      if (image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    width=ReadBlobMSBLong(image);
    height=ReadBlobMSBLong(image);
    (void) ReadBlobMSBShort(image);
    (void) ReadBlobMSBShort(image);
    bits_per_pixel=ReadBlobMSBShort(image);
    status=(width != 0UL) && (height == 0UL) && ((bits_per_pixel == 8) ||
      (bits_per_pixel == 24)) ? MagickTrue : MagickFalse;
    if (status != MagickFalse)
      {
        /*
          Allocate next image structure.
        */
        AcquireNextImage(image_info,image);
        if (GetNextImageInList(image) == (Image *) NULL)
          {
            image=DestroyImageList(image);
            return((Image *) NULL);
          }
        image=SyncNextImageInList(image);
        status=SetImageProgress(image,LoadImagesTag,TellBlob(image),
          GetBlobSize(image));
        if (status == MagickFalse)
          break;
      }
  } while (status != MagickFalse);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
