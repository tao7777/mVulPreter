static MagickBooleanType WriteImageChannels(const PSDInfo *psd_info,
  const ImageInfo *image_info,Image *image,Image *next_image,
  const MagickBooleanType separate,ExceptionInfo *exception)
{
  size_t
    channels,
    packet_size;

  unsigned char
    *compact_pixels;

  /*
    Write uncompressed pixels as separate planes.
  */
  channels=1;
  packet_size=next_image->depth > 8UL ? 2UL : 1UL;
   compact_pixels=(unsigned char *) NULL;
   if (next_image->compression == RLECompression)
     {
      compact_pixels=(unsigned char *) AcquireQuantumMemory((2*channels*
        next_image->columns)+1,packet_size*sizeof(*compact_pixels));
       if (compact_pixels == (unsigned char *) NULL)
         ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
     }
  if (IsImageGray(next_image) != MagickFalse)
    {
      if (next_image->compression == RLECompression)
        {
          /*
            Packbits compression.
          */
          (void) WriteBlobMSBShort(image,1);
          WritePackbitsLength(psd_info,image_info,image,next_image,
            compact_pixels,GrayQuantum,exception);
          if (next_image->alpha_trait != UndefinedPixelTrait)
            WritePackbitsLength(psd_info,image_info,image,next_image,
              compact_pixels,AlphaQuantum,exception);
        }
      WriteOneChannel(psd_info,image_info,image,next_image,compact_pixels,
        GrayQuantum,MagickTrue,exception);
      if (next_image->alpha_trait != UndefinedPixelTrait)
        WriteOneChannel(psd_info,image_info,image,next_image,compact_pixels,
          AlphaQuantum,separate,exception);
      (void) SetImageProgress(image,SaveImagesTag,0,1);
    }
  else
    if (next_image->storage_class == PseudoClass)
      {
        if (next_image->compression == RLECompression)
          {
            /*
              Packbits compression.
            */
            (void) WriteBlobMSBShort(image,1);
            WritePackbitsLength(psd_info,image_info,image,next_image,
              compact_pixels,IndexQuantum,exception);
            if (next_image->alpha_trait != UndefinedPixelTrait)
              WritePackbitsLength(psd_info,image_info,image,next_image,
                compact_pixels,AlphaQuantum,exception);
          }
        WriteOneChannel(psd_info,image_info,image,next_image,compact_pixels,
          IndexQuantum,MagickTrue,exception);
        if (next_image->alpha_trait != UndefinedPixelTrait)
          WriteOneChannel(psd_info,image_info,image,next_image,compact_pixels,
            AlphaQuantum,separate,exception);
        (void) SetImageProgress(image,SaveImagesTag,0,1);
      }
    else
      {
        if (next_image->colorspace == CMYKColorspace)
          (void) NegateCMYK(next_image,exception);
        if (next_image->compression == RLECompression)
          {
            /*
              Packbits compression.
            */
            (void) WriteBlobMSBShort(image,1);
            WritePackbitsLength(psd_info,image_info,image,next_image,
              compact_pixels,RedQuantum,exception);
            WritePackbitsLength(psd_info,image_info,image,next_image,
              compact_pixels,GreenQuantum,exception);
            WritePackbitsLength(psd_info,image_info,image,next_image,
              compact_pixels,BlueQuantum,exception);
            if (next_image->colorspace == CMYKColorspace)
              WritePackbitsLength(psd_info,image_info,image,next_image,
                compact_pixels,BlackQuantum,exception);
            if (next_image->alpha_trait != UndefinedPixelTrait)
              WritePackbitsLength(psd_info,image_info,image,next_image,
                compact_pixels,AlphaQuantum,exception);
          }
        (void) SetImageProgress(image,SaveImagesTag,0,6);
        WriteOneChannel(psd_info,image_info,image,next_image,compact_pixels,
          RedQuantum,MagickTrue,exception);
        (void) SetImageProgress(image,SaveImagesTag,1,6);
        WriteOneChannel(psd_info,image_info,image,next_image,compact_pixels,
          GreenQuantum,separate,exception);
        (void) SetImageProgress(image,SaveImagesTag,2,6);
        WriteOneChannel(psd_info,image_info,image,next_image,compact_pixels,
          BlueQuantum,separate,exception);
        (void) SetImageProgress(image,SaveImagesTag,3,6);
        if (next_image->colorspace == CMYKColorspace)
          WriteOneChannel(psd_info,image_info,image,next_image,compact_pixels,
            BlackQuantum,separate,exception);
        (void) SetImageProgress(image,SaveImagesTag,4,6);
        if (next_image->alpha_trait != UndefinedPixelTrait)
          WriteOneChannel(psd_info,image_info,image,next_image,compact_pixels,
            AlphaQuantum,separate,exception);
        (void) SetImageProgress(image,SaveImagesTag,5,6);
        if (next_image->colorspace == CMYKColorspace)
          (void) NegateCMYK(next_image,exception);
      }
  if (next_image->compression == RLECompression)
    compact_pixels=(unsigned char *) RelinquishMagickMemory(compact_pixels);
  return(MagickTrue);
}
