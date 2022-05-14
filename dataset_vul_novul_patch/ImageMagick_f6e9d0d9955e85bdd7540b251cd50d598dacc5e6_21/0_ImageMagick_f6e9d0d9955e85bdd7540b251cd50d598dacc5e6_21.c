static Image *ReadGRAYImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *canvas_image,
    *image;

  MagickBooleanType
    status;

  MagickOffsetType
    scene;

  QuantumInfo
    *quantum_info;

  QuantumType
    quantum_type;

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
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionError,"MustSpecifyImageSize");
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  if (DiscardBlobBytes(image,(size_t) image->offset) == MagickFalse)
    ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
      image->filename);
  /*
    Create virtual canvas to support cropping (i.e. image.gray[100x100+10+20]).
  */
  SetImageColorspace(image,GRAYColorspace);
  canvas_image=CloneImage(image,image->extract_info.width,1,MagickFalse,
    exception);
  (void) SetImageVirtualPixelMethod(canvas_image,BlackVirtualPixelMethod);
  quantum_type=GrayQuantum;
  quantum_info=AcquireQuantumInfo(image_info,canvas_image);
  if (quantum_info == (QuantumInfo *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  pixels=GetQuantumPixels(quantum_info);
  if (image_info->number_scenes != 0)
    while (image->scene < image_info->scene)
    {
      /*
        Skip to next image.
      */
      image->scene++;
      length=GetQuantumExtent(canvas_image,quantum_info,quantum_type);
      for (y=0; y < (ssize_t) image->rows; y++)
      {
        count=ReadBlob(image,length,pixels);
        if (count != (ssize_t) length)
          break;
      }
    }
  scene=0;
  count=0;
  length=0;
  do
  {
    /*
      Read pixels to virtual canvas image then push to image.
    */
     if ((image_info->ping != MagickFalse) && (image_info->number_scenes != 0))
       if (image->scene >= (image_info->scene+image_info->number_scenes-1))
         break;
    status=SetImageExtent(image,image->columns,image->rows);
    if (status == MagickFalse)
      {
        InheritException(exception,&image->exception);
        return(DestroyImageList(image));
      }
     SetImageColorspace(image,GRAYColorspace);
     if (scene == 0)
       {
        length=GetQuantumExtent(canvas_image,quantum_info,quantum_type);
        count=ReadBlob(image,length,pixels);
      }
    for (y=0; y < (ssize_t) image->extract_info.height; y++)
    {
      register const PixelPacket
        *restrict p;

      register ssize_t
        x;

      register PixelPacket
        *restrict q;

      if (count != (ssize_t) length)
        {
          ThrowFileException(exception,CorruptImageError,
            "UnexpectedEndOfFile",image->filename);
          break;
        }
      q=GetAuthenticPixels(canvas_image,0,0,canvas_image->columns,1,exception);
      if (q == (PixelPacket *) NULL)
        break;
      length=ImportQuantumPixels(canvas_image,(CacheView *) NULL,quantum_info,
        quantum_type,pixels,exception);
      if (SyncAuthenticPixels(canvas_image,exception) == MagickFalse)
        break;
      if (((y-image->extract_info.y) >= 0) &&
          ((y-image->extract_info.y) < (ssize_t) image->rows))
        {
          p=GetVirtualPixels(canvas_image,canvas_image->extract_info.x,0,
            image->columns,1,exception);
          q=QueueAuthenticPixels(image,0,y-image->extract_info.y,image->columns,
            1,exception);
          if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
            break;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            SetPixelRed(q,GetPixelRed(p));
            SetPixelGreen(q,GetPixelGreen(p));
            SetPixelBlue(q,GetPixelBlue(p));
            p++;
            q++;
          }
          if (SyncAuthenticPixels(image,exception) == MagickFalse)
            break;
        }
      if (image->previous == (Image *) NULL)
        {
          status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
            image->rows);
          if (status == MagickFalse)
            break;
        }
      count=ReadBlob(image,length,pixels);
    }
    SetQuantumImageType(image,quantum_type);
    /*
      Proceed to next image.
    */
    if (image_info->number_scenes != 0)
      if (image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    if (count == (ssize_t) length)
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
    scene++;
  } while (count == (ssize_t) length);
  quantum_info=DestroyQuantumInfo(quantum_info);
  InheritException(&image->exception,&canvas_image->exception);
  canvas_image=DestroyImage(canvas_image);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
