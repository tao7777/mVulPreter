static Image *ReadARTImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  const unsigned char
    *pixels;

  Image
    *image;

  QuantumInfo
    *quantum_info;

  QuantumType
    quantum_type;

  MagickBooleanType
    status;

  size_t
    length;

  ssize_t
    count,
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
  image->depth=1;
  image->endian=MSBEndian;
  (void) ReadBlobLSBShort(image);
  image->columns=(size_t) ReadBlobLSBShort(image);
  (void) ReadBlobLSBShort(image);
  image->rows=(size_t) ReadBlobLSBShort(image);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  /*
    Initialize image colormap.
  */
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
  SetImageColorspace(image,GRAYColorspace);
  quantum_type=IndexQuantum;
  quantum_info=AcquireQuantumInfo(image_info,image);
  if (quantum_info == (QuantumInfo *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  length=GetQuantumExtent(image,quantum_info,quantum_type);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register PixelPacket
      *magick_restrict q;

    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
     pixels=(const unsigned char *) ReadBlobStream(image,length,
       GetQuantumPixels(quantum_info),&count);
     if (count != (ssize_t) length)
      {
        quantum_info=DestroyQuantumInfo(quantum_info);
        ThrowReaderException(CorruptImageError,"UnableToReadImageData");
      }
     (void) ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
       quantum_type,pixels,exception);
     (void) ReadBlobStream(image,(size_t) (-(ssize_t) length) & 0x01,
      GetQuantumPixels(quantum_info),&count);
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
    status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  SetQuantumImageType(image,quantum_type);
  quantum_info=DestroyQuantumInfo(quantum_info);
  if (EOFBlob(image) != MagickFalse)
    ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
      image->filename);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
