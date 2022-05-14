static Image *ReadMTVImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    buffer[MaxTextExtent];

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

  unsigned char
    *pixels;

  unsigned long
    columns,
    rows;

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
    Read MTV image.
  */
  (void) ReadBlobString(image,buffer);
  count=(ssize_t) sscanf(buffer,"%lu %lu\n",&columns,&rows);
  if (count <= 0)
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  do
  {
    /*
      Initialize image structure.
    */
    image->columns=columns;
    image->rows=rows;
    image->depth=8;
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
       Convert MTV raster image to pixel packets.
     */
    pixels=(unsigned char *) AcquireQuantumMemory((size_t) image->columns,
      3UL*sizeof(*pixels));
    if (pixels == (unsigned char *) NULL)
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    for (y=0; y < (ssize_t) image->rows; y++)
    {
      count=(ssize_t) ReadBlob(image,(size_t) (3*image->columns),pixels);
      if (count != (ssize_t) (3*image->columns))
        ThrowReaderException(CorruptImageError,"UnableToReadImageData");
      p=pixels;
      q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
      if (q == (PixelPacket *) NULL)
        break;
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        SetPixelRed(q,ScaleCharToQuantum(*p++));
        SetPixelGreen(q,ScaleCharToQuantum(*p++));
        SetPixelBlue(q,ScaleCharToQuantum(*p++));
        SetPixelOpacity(q,OpaqueOpacity);
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
    *buffer='\0';
    (void) ReadBlobString(image,buffer);
    count=(ssize_t) sscanf(buffer,"%lu %lu\n",&columns,&rows);
    if (count > 0)
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
  } while (count > 0);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
