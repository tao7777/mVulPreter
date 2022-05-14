static Image *ReadWEBPImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  int
    webp_status;

  MagickBooleanType
    status;

  register unsigned char
    *p;

  size_t
    length;

  ssize_t
    count,
    y;

  unsigned char
    header[12],
    *stream;

  WebPDecoderConfig
    configure;

  WebPDecBuffer
    *restrict webp_image = &configure.output;

  WebPBitstreamFeatures
    *restrict features = &configure.input;

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
  if (WebPInitDecoderConfig(&configure) == 0)
    ThrowReaderException(ResourceLimitError,"UnableToDecodeImageFile");
  webp_image->colorspace=MODE_RGBA;
  count=ReadBlob(image,12,header);
  if (count != 12)
    ThrowReaderException(CorruptImageError,"InsufficientImageDataInFile");
  status=IsWEBP(header,count);
  if (status == MagickFalse)
    ThrowReaderException(CorruptImageError,"CorruptImage");
  length=(size_t) (ReadWebPLSBWord(header+4)+8);
  if (length < 12)
    ThrowReaderException(CorruptImageError,"CorruptImage");
  stream=(unsigned char *) AcquireQuantumMemory(length,sizeof(*stream));
  if (stream == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  (void) memcpy(stream,header,12);
  count=ReadBlob(image,length-12,stream+12);
  if (count != (ssize_t) (length-12))
    ThrowReaderException(CorruptImageError,"InsufficientImageDataInFile");
  webp_status=WebPGetFeatures(stream,length,features);
  if (webp_status == VP8_STATUS_OK)
    {
      image->columns=(size_t) features->width;
      image->rows=(size_t) features->height;
      image->depth=8;
      image->matte=features->has_alpha != 0 ? MagickTrue : MagickFalse;
      if (IsWEBPImageLossless(stream,length) != MagickFalse)
        image->quality=100;
      if (image_info->ping != MagickFalse)
        {
          stream=(unsigned char*) RelinquishMagickMemory(stream);
           (void) CloseBlob(image);
           return(GetFirstImageInList(image));
         }
       webp_status=WebPDecode(stream,length,&configure);
     }
   if (webp_status != VP8_STATUS_OK)
    {
      stream=(unsigned char*) RelinquishMagickMemory(stream);
      switch (webp_status)
      {
        case VP8_STATUS_OUT_OF_MEMORY:
        {
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
          break;
        }
        case VP8_STATUS_INVALID_PARAM:
        {
          ThrowReaderException(CorruptImageError,"invalid parameter");
          break;
        }
        case VP8_STATUS_BITSTREAM_ERROR:
        {
          ThrowReaderException(CorruptImageError,"CorruptImage");
          break;
        }
        case VP8_STATUS_UNSUPPORTED_FEATURE:
        {
          ThrowReaderException(CoderError,"DataEncodingSchemeIsNotSupported");
          break;
        }
        case VP8_STATUS_SUSPENDED:
        {
          ThrowReaderException(CorruptImageError,"decoder suspended");
          break;
        }
        case VP8_STATUS_USER_ABORT:
        {
          ThrowReaderException(CorruptImageError,"user abort");
          break;
        }
        case VP8_STATUS_NOT_ENOUGH_DATA:
        {
          ThrowReaderException(CorruptImageError,"InsufficientImageDataInFile");
          break;
        }
        default:
          ThrowReaderException(CorruptImageError,"CorruptImage");
      }
    }
  p=(unsigned char *) webp_image->u.RGBA.rgba;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register PixelPacket
      *q;

    register ssize_t
      x;

    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      SetPixelRed(q,ScaleCharToQuantum(*p++));
      SetPixelGreen(q,ScaleCharToQuantum(*p++));
      SetPixelBlue(q,ScaleCharToQuantum(*p++));
      SetPixelAlpha(q,ScaleCharToQuantum(*p++));
      q++;
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
    status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  WebPFreeDecBuffer(webp_image);
  stream=(unsigned char*) RelinquishMagickMemory(stream);
  return(image);
}
