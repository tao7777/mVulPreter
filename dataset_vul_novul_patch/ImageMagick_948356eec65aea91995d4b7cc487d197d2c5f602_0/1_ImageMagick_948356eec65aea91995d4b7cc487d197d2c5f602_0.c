ModuleExport size_t RegisterJPEGImage(void)
{
  char
    version[MaxTextExtent];

  MagickInfo
    *entry;

  static const char
    description[] = "Joint Photographic Experts Group JFIF format";

  *version='\0';
#if defined(JPEG_LIB_VERSION)
  (void) FormatLocaleString(version,MaxTextExtent,"%d",JPEG_LIB_VERSION);
#endif
  entry=SetMagickInfo("JPE");
#if (JPEG_LIB_VERSION < 80) && !defined(LIBJPEG_TURBO_VERSION)
  entry->thread_support=NoThreadSupport;
#endif
#if defined(MAGICKCORE_JPEG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadJPEGImage;
  entry->encoder=(EncodeImageHandler *) WriteJPEGImage;
 #endif
   entry->magick=(IsImageFormatHandler *) IsJPEG;
   entry->adjoin=MagickFalse;
   entry->description=ConstantString(description);
   if (*version != '\0')
     entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/jpeg");
  entry->module=ConstantString("JPEG");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("JPEG");
#if (JPEG_LIB_VERSION < 80) && !defined(LIBJPEG_TURBO_VERSION)
  entry->thread_support=NoThreadSupport;
#endif
#if defined(MAGICKCORE_JPEG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadJPEGImage;
  entry->encoder=(EncodeImageHandler *) WriteJPEGImage;
 #endif
   entry->magick=(IsImageFormatHandler *) IsJPEG;
   entry->adjoin=MagickFalse;
   entry->description=ConstantString(description);
   if (*version != '\0')
     entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/jpeg");
  entry->module=ConstantString("JPEG");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("JPG");
#if (JPEG_LIB_VERSION < 80) && !defined(LIBJPEG_TURBO_VERSION)
  entry->thread_support=NoThreadSupport;
#endif
#if defined(MAGICKCORE_JPEG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadJPEGImage;
   entry->encoder=(EncodeImageHandler *) WriteJPEGImage;
 #endif
   entry->adjoin=MagickFalse;
   entry->description=ConstantString(description);
   if (*version != '\0')
     entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/jpeg");
  entry->module=ConstantString("JPEG");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("JPS");
#if (JPEG_LIB_VERSION < 80) && !defined(LIBJPEG_TURBO_VERSION)
  entry->thread_support=NoThreadSupport;
#endif
#if defined(MAGICKCORE_JPEG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadJPEGImage;
   entry->encoder=(EncodeImageHandler *) WriteJPEGImage;
 #endif
   entry->adjoin=MagickFalse;
   entry->description=ConstantString(description);
   if (*version != '\0')
     entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/jpeg");
  entry->module=ConstantString("JPEG");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PJPEG");
#if (JPEG_LIB_VERSION < 80) && !defined(LIBJPEG_TURBO_VERSION)
  entry->thread_support=NoThreadSupport;
#endif
#if defined(MAGICKCORE_JPEG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadJPEGImage;
   entry->encoder=(EncodeImageHandler *) WriteJPEGImage;
 #endif
   entry->adjoin=MagickFalse;
   entry->description=ConstantString(description);
   if (*version != '\0')
     entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/jpeg");
  entry->module=ConstantString("JPEG");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
