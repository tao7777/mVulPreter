ModuleExport size_t RegisterPSImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("EPI");
   entry->decoder=(DecodeImageHandler *) ReadPSImage;
   entry->encoder=(EncodeImageHandler *) WritePSImage;
   entry->magick=(IsImageFormatHandler *) IsPS;
  entry->seekable_stream=MagickTrue;
   entry->adjoin=MagickFalse;
   entry->blob_support=MagickFalse;
   entry->seekable_stream=MagickTrue;
  entry->description=ConstantString(
   "Encapsulated PostScript Interchange format");
  entry->mime_type=ConstantString("application/postscript");
  entry->module=ConstantString("PS");
  (void) RegisterMagickInfo(entry);
   entry=SetMagickInfo("EPS");
   entry->decoder=(DecodeImageHandler *) ReadPSImage;
   entry->encoder=(EncodeImageHandler *) WritePSImage;
  entry->seekable_stream=MagickTrue;
   entry->magick=(IsImageFormatHandler *) IsPS;
   entry->adjoin=MagickFalse;
   entry->blob_support=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Encapsulated PostScript");
  entry->mime_type=ConstantString("application/postscript");
  entry->module=ConstantString("PS");
  (void) RegisterMagickInfo(entry);
   entry=SetMagickInfo("EPSF");
   entry->decoder=(DecodeImageHandler *) ReadPSImage;
   entry->encoder=(EncodeImageHandler *) WritePSImage;
  entry->seekable_stream=MagickTrue;
   entry->magick=(IsImageFormatHandler *) IsPS;
   entry->adjoin=MagickFalse;
   entry->blob_support=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Encapsulated PostScript");
  entry->mime_type=ConstantString("application/postscript");
  entry->module=ConstantString("PS");
  (void) RegisterMagickInfo(entry);
   entry=SetMagickInfo("EPSI");
   entry->decoder=(DecodeImageHandler *) ReadPSImage;
   entry->encoder=(EncodeImageHandler *) WritePSImage;
  entry->seekable_stream=MagickTrue;
   entry->magick=(IsImageFormatHandler *) IsPS;
   entry->adjoin=MagickFalse;
   entry->blob_support=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString(
    "Encapsulated PostScript Interchange format");
  entry->mime_type=ConstantString("application/postscript");
  entry->module=ConstantString("PS");
  (void) RegisterMagickInfo(entry);
   entry=SetMagickInfo("PS");
   entry->decoder=(DecodeImageHandler *) ReadPSImage;
   entry->encoder=(EncodeImageHandler *) WritePSImage;
  entry->seekable_stream=MagickTrue;
   entry->magick=(IsImageFormatHandler *) IsPS;
   entry->mime_type=ConstantString("application/postscript");
   entry->module=ConstantString("PS");
  entry->blob_support=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("PostScript");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
