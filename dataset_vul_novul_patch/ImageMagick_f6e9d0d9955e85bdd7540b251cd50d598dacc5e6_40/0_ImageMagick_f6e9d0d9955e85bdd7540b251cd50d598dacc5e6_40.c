static Image *ReadOTBImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define GetBit(a,i) (((a) >> (i)) & 1L)

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
    bit,
    info,
    depth;

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
    Initialize image structure.
  */
  info=(unsigned char) ReadBlobByte(image);
  if (GetBit(info,4) == 0)
    {
      image->columns=(size_t) ReadBlobByte(image);
      image->rows=(size_t) ReadBlobByte(image);
    }
  else
    {
      image->columns=(size_t) ReadBlobMSBShort(image);
      image->rows=(size_t) ReadBlobMSBShort(image);
    }
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  depth=(unsigned char) ReadBlobByte(image);
  if (depth != 1)
    ThrowReaderException(CoderError,"OnlyLevelZerofilesSupported");
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
      SetPixelIndex(indexes+x,(byte & (0x01 << (7-bit))) ?
        0x00 : 0x01);
      bit++;
      if (bit == 8)
        bit=0;
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
  (void) SyncImage(image);
  if (EOFBlob(image) != MagickFalse)
    ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
      image->filename);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
