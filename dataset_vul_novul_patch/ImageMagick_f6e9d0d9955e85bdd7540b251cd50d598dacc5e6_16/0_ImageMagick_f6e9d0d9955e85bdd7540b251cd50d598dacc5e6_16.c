static Image *ReadEXRImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  const ImfHeader
    *hdr_info;

  Image
    *image;

  ImageInfo
    *read_info;

  ImfInputFile
    *file;

  ImfRgba
    *scanline;

  int
    max_x,
    max_y,
    min_x,
    min_y;

  MagickBooleanType
    status;

  register ssize_t
    x;

  register PixelPacket
    *q;

  ssize_t
    y;

  /*
    Open image.
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
  read_info=CloneImageInfo(image_info);
  if (IsPathAccessible(read_info->filename) == MagickFalse)
    {
      (void) AcquireUniqueFilename(read_info->filename);
      (void) ImageToFile(image,read_info->filename,exception);
    }
  file=ImfOpenInputFile(read_info->filename);
  if (file == (ImfInputFile *) NULL)
    {
      ThrowFileException(exception,BlobError,"UnableToOpenBlob",
        ImfErrorMessage());
      if (LocaleCompare(image_info->filename,read_info->filename) != 0)
        (void) RelinquishUniqueFileResource(read_info->filename);
      read_info=DestroyImageInfo(read_info);
      return((Image *) NULL);
    }
  hdr_info=ImfInputHeader(file);
  ImfHeaderDisplayWindow(hdr_info,&min_x,&min_y,&max_x,&max_y);
  image->columns=max_x-min_x+1UL;
  image->rows=max_y-min_y+1UL;
  image->matte=MagickTrue;
  SetImageColorspace(image,RGBColorspace);
  if (image_info->ping != MagickFalse)
    {
      (void) ImfCloseInputFile(file);
      if (LocaleCompare(image_info->filename,read_info->filename) != 0)
        (void) RelinquishUniqueFileResource(read_info->filename);
      read_info=DestroyImageInfo(read_info);
       (void) CloseBlob(image);
       return(GetFirstImageInList(image));
     }
  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }
   scanline=(ImfRgba *) AcquireQuantumMemory(image->columns,sizeof(*scanline));
   if (scanline == (ImfRgba *) NULL)
     {
      (void) ImfCloseInputFile(file);
      if (LocaleCompare(image_info->filename,read_info->filename) != 0)
        (void) RelinquishUniqueFileResource(read_info->filename);
      read_info=DestroyImageInfo(read_info);
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    }
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    ResetMagickMemory(scanline,0,image->columns*sizeof(*scanline));
    ImfInputSetFrameBuffer(file,scanline-min_x-image->columns*(min_y+y),1,
      image->columns);
    ImfInputReadPixels(file,min_y+y,min_y+y);
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      SetPixelRed(q,ClampToQuantum((MagickRealType) QuantumRange*
        ImfHalfToFloat(scanline[x].r)));
      SetPixelGreen(q,ClampToQuantum((MagickRealType) QuantumRange*
        ImfHalfToFloat(scanline[x].g)));
      SetPixelBlue(q,ClampToQuantum((MagickRealType) QuantumRange*
        ImfHalfToFloat(scanline[x].b)));
      SetPixelAlpha(q,ClampToQuantum((MagickRealType) QuantumRange*
        ImfHalfToFloat(scanline[x].a)));
      q++;
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
  }
  scanline=(ImfRgba *) RelinquishMagickMemory(scanline);
  (void) ImfCloseInputFile(file);
  if (LocaleCompare(image_info->filename,read_info->filename) != 0)
    (void) RelinquishUniqueFileResource(read_info->filename);
  read_info=DestroyImageInfo(read_info);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
