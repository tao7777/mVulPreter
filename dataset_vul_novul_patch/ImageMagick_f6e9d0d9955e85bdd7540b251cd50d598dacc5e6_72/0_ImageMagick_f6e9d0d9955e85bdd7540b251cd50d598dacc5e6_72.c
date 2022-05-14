static Image *ReadWBMPImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  int
    byte;

  MagickBooleanType
    status;

  register IndexPacket
    *indexes;

  register ssize_t
    x;

  register PixelPacket
    *q;

  ssize_t
    y;

  unsigned char
    bit;

  unsigned short
    header;

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
  if (ReadBlob(image,2,(unsigned char *) &header) == 0)
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  if (header != 0)
    ThrowReaderException(CoderError,"OnlyLevelZerofilesSupported");
  /*
    Initialize image structure.
  */
  if (WBMPReadInteger(image,&image->columns) == MagickFalse)
    ThrowReaderException(CorruptImageError,"CorruptWBMPimage");
  if (WBMPReadInteger(image,&image->rows) == MagickFalse)
    ThrowReaderException(CorruptImageError,"CorruptWBMPimage");
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  if (DiscardBlobBytes(image,image->offset) == MagickFalse)
    ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
      image->filename);
  if (AcquireImageColormap(image,2) == MagickFalse)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
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
     Convert bi-level image to pixel packets.
   */
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetAuthenticIndexQueue(image);
    bit=0;
    byte=0;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if (bit == 0)
        {
          byte=ReadBlobByte(image);
          if (byte == EOF)
            ThrowReaderException(CorruptImageError,"CorruptImage");
        }
      SetPixelIndex(indexes+x,(byte & (0x01 << (7-bit))) ? 1 : 0);
      bit++;
      if (bit == 8)
        bit=0;
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
    status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
                image->rows);
    if (status == MagickFalse)
      break;
  }
  (void) SyncImage(image);
  if (EOFBlob(image) != MagickFalse)
    ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
      image->filename);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
