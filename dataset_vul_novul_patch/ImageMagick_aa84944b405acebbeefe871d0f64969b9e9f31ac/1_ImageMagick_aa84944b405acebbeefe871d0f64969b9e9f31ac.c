static Image *ReadJNGImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  MagickBooleanType
    logging,
    status;

  MngInfo
    *mng_info;

  char
    magic_number[MaxTextExtent];

  size_t
    count;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  logging=LogMagickEvent(CoderEvent,GetMagickModule(),"Enter ReadJNGImage()");
  image=AcquireImage(image_info);
  mng_info=(MngInfo *) NULL;
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);

  if (status == MagickFalse)
    return((Image *) NULL);

  if (LocaleCompare(image_info->magick,"JNG") != 0)
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");

  /* Verify JNG signature.  */

  count=(size_t) ReadBlob(image,8,(unsigned char *) magic_number);

   if (count < 8 || memcmp(magic_number,"\213JNG\r\n\032\n",8) != 0)
     ThrowReaderException(CorruptImageError,"ImproperImageHeader");
 
   /* Allocate a MngInfo structure.  */
 
   mng_info=(MngInfo *) AcquireMagickMemory(sizeof(*mng_info));

  if (mng_info == (MngInfo *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");

  /* Initialize members of the MngInfo structure.  */

  (void) ResetMagickMemory(mng_info,0,sizeof(MngInfo));

  mng_info->image=image;
  image=ReadOneJNGImage(mng_info,image_info,exception);
  mng_info=MngInfoFreeStruct(mng_info);

  if (image == (Image *) NULL)
    {
      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "exit ReadJNGImage() with error");

      return((Image *) NULL);
    }
  (void) CloseBlob(image);

  if (image->columns == 0 || image->rows == 0)
    {
      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "exit ReadJNGImage() with error");

      ThrowReaderException(CorruptImageError,"CorruptImage");
    }

  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"exit ReadJNGImage()");

  return(image);
}
