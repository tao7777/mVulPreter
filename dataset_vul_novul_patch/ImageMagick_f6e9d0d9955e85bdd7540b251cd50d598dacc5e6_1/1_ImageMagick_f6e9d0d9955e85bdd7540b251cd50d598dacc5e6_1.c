static Image *ReadAVSImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  MagickBooleanType
    status;

  register PixelPacket
    *q;

  register ssize_t
    x;

  register unsigned char
    *p;

  size_t
    height,
    width;

  ssize_t
    count,
    y;

  unsigned char
    *pixels;

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
    Read AVS X image.
  */
  width=ReadBlobMSBLong(image);
  height=ReadBlobMSBLong(image);
  if (EOFBlob(image) != MagickFalse)
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  if ((width == 0UL) || (height == 0UL))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  do
  {
    ssize_t
      length;

    /*
      Convert AVS raster image to pixel packets.
    */
    image->columns=width;
    image->rows=height;
    image->depth=8;
     if ((image_info->ping != MagickFalse) && (image_info->number_scenes != 0))
       if (image->scene >= (image_info->scene+image_info->number_scenes-1))
         break;
     pixels=(unsigned char *) AcquireQuantumMemory(image->columns,
       4*sizeof(*pixels));
     if (pixels == (unsigned char *) NULL)
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    length=(size_t) 4*image->columns;
    for (y=0; y < (ssize_t) image->rows; y++)
    {
      count=ReadBlob(image,length,pixels);
      if (count != length)
        ThrowReaderException(CorruptImageError,"UnableToReadImageData");
      p=pixels;
      q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
      if (q == (PixelPacket *) NULL)
        break;
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        SetPixelAlpha(q,ScaleCharToQuantum(*p++));
        SetPixelRed(q,ScaleCharToQuantum(*p++));
        SetPixelGreen(q,ScaleCharToQuantum(*p++));
        SetPixelBlue(q,ScaleCharToQuantum(*p++));
        if (q->opacity != OpaqueOpacity)
          image->matte=MagickTrue;
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
    pixels=(unsigned char *) RelinquishMagickMemory(pixels);
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
    if ((width != 0UL) && (height != 0UL))
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
  } while ((width != 0UL) && (height != 0UL));
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
