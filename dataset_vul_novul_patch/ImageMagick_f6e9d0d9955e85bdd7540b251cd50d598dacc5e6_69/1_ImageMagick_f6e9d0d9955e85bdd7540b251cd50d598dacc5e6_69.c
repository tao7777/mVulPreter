static Image *ReadVICARImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    keyword[MaxTextExtent],
    value[MaxTextExtent];

  Image
    *image;

  int
    c;

  MagickBooleanType
    status,
    value_expected;

  QuantumInfo
    *quantum_info;

  QuantumType
    quantum_type;

  register PixelPacket
    *q;

  size_t
    length;

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
    Decode image header.
  */
  c=ReadBlobByte(image);
  count=1;
  if (c == EOF)
    {
      image=DestroyImage(image);
      return((Image *) NULL);
    }
  length=0;
  image->columns=0;
  image->rows=0;
  while (isgraph(c) && ((image->columns == 0) || (image->rows == 0)))
  {
    if (isalnum(c) == MagickFalse)
      {
        c=ReadBlobByte(image);
        count++;
      }
    else
      {
        register char
          *p;

        /*
          Determine a keyword and its value.
        */
        p=keyword;
        do
        {
          if ((size_t) (p-keyword) < (MaxTextExtent-1))
            *p++=c;
          c=ReadBlobByte(image);
          count++;
        } while (isalnum(c) || (c == '_'));
        *p='\0';
        value_expected=MagickFalse;
        while ((isspace((int) ((unsigned char) c)) != 0) || (c == '='))
        {
          if (c == '=')
            value_expected=MagickTrue;
          c=ReadBlobByte(image);
          count++;
        }
        if (value_expected == MagickFalse)
          continue;
        p=value;
        while (isalnum(c))
        {
          if ((size_t) (p-value) < (MaxTextExtent-1))
            *p++=c;
          c=ReadBlobByte(image);
          count++;
        }
        *p='\0';
        /*
          Assign a value to the specified keyword.
        */
        if (LocaleCompare(keyword,"Label_RECORDS") == 0)
          length=(ssize_t) StringToLong(value);
        if (LocaleCompare(keyword,"LBLSIZE") == 0)
          length=(ssize_t) StringToLong(value);
        if (LocaleCompare(keyword,"RECORD_BYTES") == 0)
          image->columns=StringToUnsignedLong(value);
        if (LocaleCompare(keyword,"NS") == 0)
          image->columns=StringToUnsignedLong(value);
        if (LocaleCompare(keyword,"LINES") == 0)
          image->rows=StringToUnsignedLong(value);
        if (LocaleCompare(keyword,"NL") == 0)
          image->rows=StringToUnsignedLong(value);
      }
    while (isspace((int) ((unsigned char) c)) != 0)
    {
      c=ReadBlobByte(image);
      count++;
    }
  }
  while (count < (ssize_t) length)
  {
    c=ReadBlobByte(image);
    count++;
  }
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(CorruptImageError,"NegativeOrZeroImageSize");
  image->depth=8;
  if (image_info->ping != MagickFalse)
    {
       (void) CloseBlob(image);
       return(GetFirstImageInList(image));
     }
   /*
     Read VICAR pixels.
   */
  (void) SetImageColorspace(image,GRAYColorspace);
  quantum_type=GrayQuantum;
  quantum_info=AcquireQuantumInfo(image_info,image);
  if (quantum_info == (QuantumInfo *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  pixels=GetQuantumPixels(quantum_info);
  length=GetQuantumExtent(image,quantum_info,quantum_type);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    count=ReadBlob(image,length,pixels);
    (void) ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
      quantum_type,pixels,exception);
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
