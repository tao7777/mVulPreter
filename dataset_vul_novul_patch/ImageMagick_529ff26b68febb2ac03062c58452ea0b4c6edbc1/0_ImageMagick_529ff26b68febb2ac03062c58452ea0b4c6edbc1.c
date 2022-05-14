ModuleExport size_t RegisterMPCImage(void)
{
  MagickInfo
    *entry;

   entry=SetMagickInfo("CACHE");
   entry->description=ConstantString("Magick Persistent Cache image format");
   entry->module=ConstantString("MPC");
   entry->stealth=MagickTrue;
   (void) RegisterMagickInfo(entry);
   entry=SetMagickInfo("MPC");
  entry->decoder=(DecodeImageHandler *) ReadMPCImage;
  entry->encoder=(EncodeImageHandler *) WriteMPCImage;
  entry->magick=(IsImageFormatHandler *) IsMPC;
  entry->description=ConstantString("Magick Persistent Cache image format");
  entry->seekable_stream=MagickTrue;
  entry->module=ConstantString("MPC");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}
