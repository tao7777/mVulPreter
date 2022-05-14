static MagickBooleanType WriteIPLImage(const ImageInfo *image_info,Image *image)
{
  ExceptionInfo
    *exception;

  IPLInfo
    ipl_info;

  MagickBooleanType
    status;

  MagickOffsetType
    scene;

  register const PixelPacket
    *p;

  QuantumInfo
    *quantum_info;

  ssize_t
    y;

  unsigned char
    *pixels;

   /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFalse)
    return(status);
   scene=0;
 
   quantum_info=AcquireQuantumInfo(image_info, image);
  if (quantum_info == (QuantumInfo *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
   if ((quantum_info->format == UndefinedQuantumFormat) &&
       (IsHighDynamicRangeImage(image,&image->exception) != MagickFalse))
     SetQuantumFormat(image,quantum_info,FloatingPointQuantumFormat);
  switch(quantum_info->depth){
  case 8:
    ipl_info.byteType = 0;
    break;
  case 16:
    if(quantum_info->format == SignedQuantumFormat){
      ipl_info.byteType = 2;
    }
    else{
      ipl_info.byteType = 1;
    }
    break;
  case 32:
    if(quantum_info->format == FloatingPointQuantumFormat){
      ipl_info.byteType = 3;
    }
    else{
      ipl_info.byteType = 4;
    }
    break;
  case 64:
    ipl_info.byteType = 10;
    break;
  default:
    ipl_info.byteType = 2;
    break;

  }
  ipl_info.z = (unsigned int) GetImageListLength(image);
  /* There is no current method for detecting whether we have T or Z stacks */
  ipl_info.time = 1;
  ipl_info.width = (unsigned int) image->columns;
  ipl_info.height = (unsigned int) image->rows;
  (void) TransformImageColorspace(image,sRGBColorspace);
  if(IssRGBCompatibleColorspace(image->colorspace) != MagickFalse) { ipl_info.colors = 3; }
  else{ ipl_info.colors = 1; }

  ipl_info.size = (unsigned int) (28 +
    ((image->depth)/8)*ipl_info.height*ipl_info.width*ipl_info.colors*ipl_info.z);

  /* Ok!  Calculations are done.  Lets write this puppy down! */

  /*
    Write IPL header.
  */
  /* Shockingly (maybe not if you have used IPLab),  IPLab itself CANNOT read MSBEndian
  files!   The reader above can, but they cannot.  For compatability reasons, I will leave
  the code in here, but it is all but useless if you want to use IPLab. */

  if(image_info->endian == MSBEndian)
    (void) WriteBlob(image, 4, (const unsigned char *) "mmmm");
  else{
    image->endian = LSBEndian;
    (void) WriteBlob(image, 4, (const unsigned char *) "iiii");
  }
  (void) WriteBlobLong(image, 4);
  (void) WriteBlob(image, 4, (const unsigned char *) "100f");
  (void) WriteBlob(image, 4, (const unsigned char *) "data");
  (void) WriteBlobLong(image, ipl_info.size);
  (void) WriteBlobLong(image, ipl_info.width);
  (void) WriteBlobLong(image, ipl_info.height);
  (void) WriteBlobLong(image, ipl_info.colors);
  if(image_info->adjoin == MagickFalse)
  (void) WriteBlobLong(image, 1);
  else
  (void) WriteBlobLong(image, ipl_info.z);
  (void) WriteBlobLong(image, ipl_info.time);
  (void) WriteBlobLong(image, ipl_info.byteType);

  exception=(&image->exception);
  do
    {
      /*
  Convert MIFF to IPL raster pixels.
      */
      pixels=GetQuantumPixels(quantum_info);
  if(ipl_info.colors == 1){
  /* Red frame */
  for(y = 0; y < (ssize_t) ipl_info.height; y++){
    p=GetAuthenticPixels(image,0,y,image->columns,1,exception);
    if (p == (PixelPacket *) NULL)
      break;
    (void) ExportQuantumPixels(image,(const CacheView *) NULL, quantum_info,
      GrayQuantum, pixels,&image->exception);
    (void) WriteBlob(image, image->columns*image->depth/8, pixels);
  }

}
  if(ipl_info.colors == 3){
  /* Red frame */
  for(y = 0; y < (ssize_t) ipl_info.height; y++){
    p=GetAuthenticPixels(image,0,y,image->columns,1,exception);
    if (p == (PixelPacket *) NULL)
      break;
    (void) ExportQuantumPixels(image,(const CacheView *) NULL, quantum_info,
      RedQuantum, pixels,&image->exception);
    (void) WriteBlob(image, image->columns*image->depth/8, pixels);
  }

    /* Green frame */
    for(y = 0; y < (ssize_t) ipl_info.height; y++){
      p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
      if (p == (PixelPacket *) NULL)
        break;
      (void) ExportQuantumPixels(image,(const CacheView *) NULL, quantum_info,
        GreenQuantum, pixels,&image->exception);
      (void) WriteBlob(image, image->columns*image->depth/8, pixels);
    }
    /* Blue frame */
    for(y = 0; y < (ssize_t) ipl_info.height; y++){
      p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
      if (p == (PixelPacket *) NULL)
        break;
      (void) ExportQuantumPixels(image,(const CacheView *) NULL, quantum_info,
        BlueQuantum, pixels,&image->exception);
      (void) WriteBlob(image, image->columns*image->depth/8, pixels);
      if (image->previous == (Image *) NULL)
        {
          status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                image->rows);
          if (status == MagickFalse)
            break;
        }
    }
  }
  quantum_info=DestroyQuantumInfo(quantum_info);
  if (GetNextImageInList(image) == (Image *) NULL)
    break;
      image=SyncNextImageInList(image);
      status=SetImageProgress(image,SaveImagesTag,scene++,
        GetImageListLength(image));
      if (status == MagickFalse)
        break;
    }while (image_info->adjoin != MagickFalse);

  (void) WriteBlob(image, 4, (const unsigned char *) "fini");
  (void) WriteBlobLong(image, 0);

CloseBlob(image);
return(MagickTrue);
}
