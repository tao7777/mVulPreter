static MagickBooleanType WriteRGFImage(const ImageInfo *image_info,Image *image,
static MagickBooleanType WriteRGFImage(const ImageInfo *image_info,Image *image)
 {
   MagickBooleanType
     status;

  int
    bit;

  register const PixelPacket
    *p;

  register ssize_t
    x;

  ssize_t
    y;

  unsigned char
    byte;

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
   (void) TransformImageColorspace(image,sRGBColorspace);
  if((image->columns > 255L) || (image->rows > 255L))
    ThrowWriterException(ImageError,"Dimensions must be less than 255x255");
  /*
    Write header (just the image dimensions)
  */
  (void) WriteBlobByte(image,image->columns & 0xff);
  (void) WriteBlobByte(image,image->rows & 0xff);
  /*
    Convert MIFF to bit pixels.
  */
  (void) SetImageType(image,BilevelType);
  x=0;
   y=0;
   for (y=0; y < (ssize_t) image->rows; y++)
   {
    p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
     if (p == (const PixelPacket *) NULL)
       break;
     bit=0;
    byte=0;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      byte>>=1;
      if (GetPixelLuma(image,p) < (QuantumRange/2.0))
        byte|=0x80;
      bit++;
      if (bit == 8)
        {
          /*
            Write a bitmap byte to the image file.
          */
       	  (void) WriteBlobByte(image,byte);
          bit=0;
          byte=0;
        }
      p++;
    }
    if (bit != 0)
      (void) WriteBlobByte(image,byte);
    status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  (void) CloseBlob(image);
  return(MagickTrue);
}
