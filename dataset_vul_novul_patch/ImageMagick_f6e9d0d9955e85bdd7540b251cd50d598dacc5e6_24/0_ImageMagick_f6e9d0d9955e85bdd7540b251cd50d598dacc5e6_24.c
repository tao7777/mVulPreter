static Image *ReadHRZImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  MagickBooleanType
    status;

  register ssize_t
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

  ssize_t
    count,
    y;

  size_t
    length;

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
    Convert HRZ raster image to pixel packets.
  */
   image->columns=256;
   image->rows=240;
   image->depth=8;
  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }
   pixels=(unsigned char *) AcquireQuantumMemory(image->columns,3*
     sizeof(*pixels));
   if (pixels == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  length=(size_t) (3*image->columns);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    count=ReadBlob(image,length,pixels);
    if ((size_t) count != length)
      ThrowReaderException(CorruptImageError,"UnableToReadImageData");
    p=pixels;
    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      SetPixelRed(q,ScaleCharToQuantum(4**p++));
      SetPixelGreen(q,ScaleCharToQuantum(4**p++));
      SetPixelBlue(q,ScaleCharToQuantum(4**p++));
      SetPixelOpacity(q,OpaqueOpacity);
      q++;
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
    if (SetImageProgress(image,LoadImageTag,y,image->rows) == MagickFalse)
      break;
  }
  pixels=(unsigned char *) RelinquishMagickMemory(pixels);
  if (EOFBlob(image) != MagickFalse)
    ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
      image->filename);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
