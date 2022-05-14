ModuleExport size_t RegisterPNGImage(void)
{
  char
    version[MaxTextExtent];

  MagickInfo
    *entry;

  static const char
    *PNGNote=
    {
      "See http://www.libpng.org/ for details about the PNG format."
    },

    *JNGNote=
    {
      "See http://www.libpng.org/pub/mng/ for details about the JNG\n"
      "format."
    },

    *MNGNote=
    {
      "See http://www.libpng.org/pub/mng/ for details about the MNG\n"
      "format."
    };

  *version='\0';

#if defined(PNG_LIBPNG_VER_STRING)
  (void) ConcatenateMagickString(version,"libpng ",MaxTextExtent);
  (void) ConcatenateMagickString(version,PNG_LIBPNG_VER_STRING,MaxTextExtent);

  if (LocaleCompare(PNG_LIBPNG_VER_STRING,png_get_header_ver(NULL)) != 0)
    {
      (void) ConcatenateMagickString(version,",",MaxTextExtent);
      (void) ConcatenateMagickString(version,png_get_libpng_ver(NULL),
            MaxTextExtent);
    }
 #endif
 
   entry=SetMagickInfo("MNG");
  entry->seekable_stream=MagickTrue;
 
 #if defined(MAGICKCORE_PNG_DELEGATE)
   entry->decoder=(DecodeImageHandler *) ReadMNGImage;
  entry->encoder=(EncodeImageHandler *) WriteMNGImage;
#endif

  entry->magick=(IsImageFormatHandler *) IsMNG;
  entry->description=ConstantString("Multiple-image Network Graphics");

  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->mime_type=ConstantString("video/x-mng");
  entry->module=ConstantString("PNG");
  entry->note=ConstantString(MNGNote);
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("PNG");

#if defined(MAGICKCORE_PNG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadPNGImage;
  entry->encoder=(EncodeImageHandler *) WritePNGImage;
 #endif
 
   entry->magick=(IsImageFormatHandler *) IsPNG;
  entry->seekable_stream=MagickTrue;
   entry->adjoin=MagickFalse;
   entry->description=ConstantString("Portable Network Graphics");
   entry->mime_type=ConstantString("image/png");
  entry->module=ConstantString("PNG");

  if (*version != '\0')
    entry->version=ConstantString(version);

  entry->note=ConstantString(PNGNote);
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("PNG8");

#if defined(MAGICKCORE_PNG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadPNGImage;
  entry->encoder=(EncodeImageHandler *) WritePNGImage;
 #endif
 
   entry->magick=(IsImageFormatHandler *) IsPNG;
  entry->seekable_stream=MagickTrue;
   entry->adjoin=MagickFalse;
   entry->description=ConstantString(
             "8-bit indexed with optional binary transparency");
  entry->mime_type=ConstantString("image/png");
  entry->module=ConstantString("PNG");
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("PNG24");
  *version='\0';

#if defined(ZLIB_VERSION)
  (void) ConcatenateMagickString(version,"zlib ",MaxTextExtent);
  (void) ConcatenateMagickString(version,ZLIB_VERSION,MaxTextExtent);

  if (LocaleCompare(ZLIB_VERSION,zlib_version) != 0)
    {
      (void) ConcatenateMagickString(version,",",MaxTextExtent);
      (void) ConcatenateMagickString(version,zlib_version,MaxTextExtent);
    }
#endif

  if (*version != '\0')
    entry->version=ConstantString(version);

#if defined(MAGICKCORE_PNG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadPNGImage;
  entry->encoder=(EncodeImageHandler *) WritePNGImage;
 #endif
 
   entry->magick=(IsImageFormatHandler *) IsPNG;
  entry->seekable_stream=MagickTrue;
   entry->adjoin=MagickFalse;
   entry->description=ConstantString("opaque or binary transparent 24-bit RGB");
   entry->mime_type=ConstantString("image/png");
  entry->module=ConstantString("PNG");
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("PNG32");

#if defined(MAGICKCORE_PNG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadPNGImage;
  entry->encoder=(EncodeImageHandler *) WritePNGImage;
 #endif
 
   entry->magick=(IsImageFormatHandler *) IsPNG;
  entry->seekable_stream=MagickTrue;
   entry->adjoin=MagickFalse;
   entry->description=ConstantString("opaque or transparent 32-bit RGBA");
   entry->mime_type=ConstantString("image/png");
  entry->module=ConstantString("PNG");
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("PNG48");

#if defined(MAGICKCORE_PNG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadPNGImage;
  entry->encoder=(EncodeImageHandler *) WritePNGImage;
 #endif
 
   entry->magick=(IsImageFormatHandler *) IsPNG;
  entry->seekable_stream=MagickTrue;
   entry->adjoin=MagickFalse;
   entry->description=ConstantString("opaque or binary transparent 48-bit RGB");
   entry->mime_type=ConstantString("image/png");
  entry->module=ConstantString("PNG");
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("PNG64");

#if defined(MAGICKCORE_PNG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadPNGImage;
  entry->encoder=(EncodeImageHandler *) WritePNGImage;
 #endif
 
   entry->magick=(IsImageFormatHandler *) IsPNG;
  entry->seekable_stream=MagickTrue;
   entry->adjoin=MagickFalse;
   entry->description=ConstantString("opaque or transparent 64-bit RGBA");
   entry->mime_type=ConstantString("image/png");
  entry->module=ConstantString("PNG");
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("PNG00");

#if defined(MAGICKCORE_PNG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadPNGImage;
  entry->encoder=(EncodeImageHandler *) WritePNGImage;
 #endif
 
   entry->magick=(IsImageFormatHandler *) IsPNG;
  entry->seekable_stream=MagickTrue;
   entry->adjoin=MagickFalse;
   entry->description=ConstantString(
      "PNG inheriting bit-depth, color-type from original if possible");
  entry->mime_type=ConstantString("image/png");
  entry->module=ConstantString("PNG");
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("JNG");

#if defined(JNG_SUPPORTED)
#if defined(MAGICKCORE_PNG_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadJNGImage;
  entry->encoder=(EncodeImageHandler *) WriteJNGImage;
#endif
 #endif
 
   entry->magick=(IsImageFormatHandler *) IsJNG;
  entry->seekable_stream=MagickTrue;
   entry->adjoin=MagickFalse;
   entry->description=ConstantString("JPEG Network Graphics");
   entry->mime_type=ConstantString("image/x-jng");
  entry->module=ConstantString("PNG");
  entry->note=ConstantString(JNGNote);
  (void) RegisterMagickInfo(entry);

#ifdef IMPNG_SETJMP_NOT_THREAD_SAFE
  ping_semaphore=AllocateSemaphoreInfo();
#endif

  return(MagickImageCoderSignature);
}
