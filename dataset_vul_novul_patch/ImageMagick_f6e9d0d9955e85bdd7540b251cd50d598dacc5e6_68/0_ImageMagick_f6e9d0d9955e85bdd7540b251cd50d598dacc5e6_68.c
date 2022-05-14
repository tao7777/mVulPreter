static Image *ReadUYVYImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  MagickBooleanType
    status;

  register ssize_t
    x;

  register PixelPacket
    *q;

  ssize_t
    y;

  unsigned char
    u,
    v,
    y1,
    y2;

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
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionError,"MustSpecifyImageSize");
  if ((image->columns % 2) != 0)
    image->columns++;
  (void) CopyMagickString(image->filename,image_info->filename,MaxTextExtent);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    return((Image *) NULL);
  if (DiscardBlobBytes(image,image->offset) == MagickFalse)
    ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
      image->filename);
  image->depth=8;
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
     Accumulate UYVY, then unpack into two pixels.
   */
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (ssize_t) (image->columns >> 1); x++)
    {
      u=(unsigned char) ReadBlobByte(image);
      y1=(unsigned char) ReadBlobByte(image);
      v=(unsigned char) ReadBlobByte(image);
      y2=(unsigned char) ReadBlobByte(image);
      SetPixelRed(q,ScaleCharToQuantum(y1));
      SetPixelGreen(q,ScaleCharToQuantum(u));
      SetPixelBlue(q,ScaleCharToQuantum(v));
      q++;
      SetPixelRed(q,ScaleCharToQuantum(y2));
      SetPixelGreen(q,ScaleCharToQuantum(u));
      SetPixelBlue(q,ScaleCharToQuantum(v));
      q++;
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
    status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  SetImageColorspace(image,YCbCrColorspace);
  if (EOFBlob(image) != MagickFalse)
    ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
      image->filename);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
