static Image *ReadIPLImage(const ImageInfo *image_info,ExceptionInfo *exception)
{

  /*
    Declare variables.
  */
  Image *image;

  MagickBooleanType status;
  register PixelPacket *q;
  unsigned char magick[12], *pixels;
  ssize_t count;
  ssize_t y;
  size_t t_count=0;
  size_t length;
  IPLInfo
    ipl_info;
  QuantumFormatType
    quantum_format;
  QuantumInfo
    *quantum_info;
  QuantumType
    quantum_type;

  /*
   Open Image
   */

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if ( image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent, GetMagickModule(), "%s",
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
    Read IPL image
  */

  /*
    Determine endianness
    If we get back "iiii", we have LSB,"mmmm", MSB
  */
  count=ReadBlob(image,4,magick);
  (void) count;
  if((LocaleNCompare((char *) magick,"iiii",4) == 0))
    image->endian=LSBEndian;
  else{
    if((LocaleNCompare((char *) magick,"mmmm",4) == 0))
      image->endian=MSBEndian;
    else{
      ThrowReaderException(CorruptImageError, "ImproperImageHeader");
    }
  }
  /* Skip o'er the next 8 bytes (garbage) */
  count=ReadBlob(image, 8, magick);
  /*
   Excellent, now we read the header unimpeded.
   */
  count=ReadBlob(image,4,magick);
  if((LocaleNCompare((char *) magick,"data",4) != 0))
    ThrowReaderException(CorruptImageError, "ImproperImageHeader");
  ipl_info.size=ReadBlobLong(image);
  ipl_info.width=ReadBlobLong(image);
  ipl_info.height=ReadBlobLong(image);
  if((ipl_info.width == 0UL) || (ipl_info.height == 0UL))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  ipl_info.colors=ReadBlobLong(image);
  if(ipl_info.colors == 3){ SetImageColorspace(image,sRGBColorspace);}
  else { image->colorspace = GRAYColorspace; }
  ipl_info.z=ReadBlobLong(image);
  ipl_info.time=ReadBlobLong(image);

  ipl_info.byteType=ReadBlobLong(image);


  /* Initialize Quantum Info */

  switch (ipl_info.byteType) {
    case 0:
      ipl_info.depth=8;
      quantum_format = UnsignedQuantumFormat;
      break;
    case 1:
      ipl_info.depth=16;
      quantum_format = SignedQuantumFormat;
      break;
    case 2:
      ipl_info.depth=16;
      quantum_format = UnsignedQuantumFormat;
      break;
    case 3:
      ipl_info.depth=32;
      quantum_format = SignedQuantumFormat;
      break;
    case 4: ipl_info.depth=32;
      quantum_format = FloatingPointQuantumFormat;
      break;
    case 5:
      ipl_info.depth=8;
      quantum_format = UnsignedQuantumFormat;
      break;
    case 6:
      ipl_info.depth=16;
      quantum_format = UnsignedQuantumFormat;
      break;
    case 10:
      ipl_info.depth=64;
      quantum_format = FloatingPointQuantumFormat;
      break;
    default:
      ipl_info.depth=16;
      quantum_format = UnsignedQuantumFormat;
      break;
  }

  /*
    Set number of scenes of image
  */

  SetHeaderFromIPL(image, &ipl_info);

  /* Thats all we need if we are pinging. */
  if (image_info->ping != MagickFalse)
  {
    (void) CloseBlob(image);
    return(GetFirstImageInList(image));
  }
  length=image->columns;
  quantum_type=GetQuantumType(image,exception);
 do
   {
     SetHeaderFromIPL(image, &ipl_info);
 
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
    printf("Length: %.20g, Memory size: %.20g\n", (double) length,(double)
      image->depth);
*/
     quantum_info=AcquireQuantumInfo(image_info,image);
     if (quantum_info == (QuantumInfo *) NULL)
       ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
     status=SetQuantumFormat(image,quantum_info,quantum_format);
     if (status == MagickFalse)
       ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
     pixels=GetQuantumPixels(quantum_info);
     if(image->columns != ipl_info.width){
/*
     printf("Columns not set correctly!  Wanted: %.20g, got: %.20g\n",
       (double) ipl_info.width, (double) image->columns);
*/
     }

    /*
      Covert IPL binary to pixel packets
    */

  if(ipl_info.colors == 1){
      for(y = 0; y < (ssize_t) image->rows; y++){
        (void) ReadBlob(image, length*image->depth/8, pixels);
        q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
        if (q == (PixelPacket *) NULL)
                break;
        (void) ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
          GrayQuantum,pixels,exception);
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
    }
  }
  else{
      for(y = 0; y < (ssize_t) image->rows; y++){
        (void) ReadBlob(image, length*image->depth/8, pixels);
        q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
        if (q == (PixelPacket *) NULL)
                break;
        (void) ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
          RedQuantum,pixels,exception);
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
      }
      for(y = 0; y < (ssize_t) image->rows; y++){
        (void) ReadBlob(image, length*image->depth/8, pixels);
        q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
        if (q == (PixelPacket *) NULL)
          break;
        (void) ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
          GreenQuantum,pixels,exception);
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
      }
      for(y = 0; y < (ssize_t) image->rows; y++){
        (void) ReadBlob(image, length*image->depth/8, pixels);
        q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
        if (q == (PixelPacket *) NULL)
          break;
        (void) ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
          BlueQuantum,pixels,exception);
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
      }
   }
   SetQuantumImageType(image,quantum_type);

    t_count++;
  quantum_info = DestroyQuantumInfo(quantum_info);

    if (EOFBlob(image) != MagickFalse)
    {
      ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
                 image->filename);
      break;
    }
   if(t_count < ipl_info.z * ipl_info.time){
      /*
       Proceed to next image.
       */
      AcquireNextImage(image_info, image);
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
  } while (t_count < ipl_info.z*ipl_info.time);
  CloseBlob(image);
  return(GetFirstImageInList(image));
}
