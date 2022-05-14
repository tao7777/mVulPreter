static MagickBooleanType WritePSDImage(const ImageInfo *image_info,Image *image,
static MagickBooleanType WritePSDImage(const ImageInfo *image_info,
  Image *image,ExceptionInfo *exception)
 {
   const char
     *property;

  const StringInfo
    *icc_profile;

  Image
    *base_image,
    *next_image;

  MagickBooleanType
    status;

  PSDInfo
    psd_info;

  register ssize_t
    i;

  size_t
    channel_size,
    channelLength,
    layer_count,
    layer_info_size,
    length,
    num_channels,
    packet_size,
    rounded_layer_info_size;

  StringInfo
    *bim_profile;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  packet_size=(size_t) (image->depth > 8 ? 6 : 3);
  if (image->alpha_trait != UndefinedPixelTrait)
    packet_size+=image->depth > 8 ? 2 : 1;
  psd_info.version=1;
  if ((LocaleCompare(image_info->magick,"PSB") == 0) ||
      (image->columns > 30000) || (image->rows > 30000))
    psd_info.version=2;
  (void) WriteBlob(image,4,(const unsigned char *) "8BPS");
  (void) WriteBlobMSBShort(image,psd_info.version);  /* version */
  for (i=1; i <= 6; i++)
    (void) WriteBlobByte(image, 0);  /* 6 bytes of reserved */
  if (SetImageGray(image,exception) != MagickFalse)
    num_channels=(image->alpha_trait != UndefinedPixelTrait ? 2UL : 1UL);
  else
    if ((image_info->type != TrueColorType) && (image_info->type !=
         TrueColorAlphaType) && (image->storage_class == PseudoClass))
      num_channels=(image->alpha_trait != UndefinedPixelTrait ? 2UL : 1UL);
    else
      {
        if (image->storage_class == PseudoClass)
          (void) SetImageStorageClass(image,DirectClass,exception);
        if (image->colorspace != CMYKColorspace)
          num_channels=(image->alpha_trait != UndefinedPixelTrait ? 4UL : 3UL);
        else
          num_channels=(image->alpha_trait != UndefinedPixelTrait ? 5UL : 4UL);
      }
  (void) WriteBlobMSBShort(image,(unsigned short) num_channels);
  (void) WriteBlobMSBLong(image,(unsigned int) image->rows);
  (void) WriteBlobMSBLong(image,(unsigned int) image->columns);
  if (IsImageGray(image) != MagickFalse)
    {
      MagickBooleanType
        monochrome;

      /*
        Write depth & mode.
      */
      monochrome=IsImageMonochrome(image) && (image->depth == 1) ?
        MagickTrue : MagickFalse;
      (void) WriteBlobMSBShort(image,(unsigned short)
        (monochrome != MagickFalse ? 1 : image->depth > 8 ? 16 : 8));
      (void) WriteBlobMSBShort(image,(unsigned short)
        (monochrome != MagickFalse ? BitmapMode : GrayscaleMode));
    }
  else
    {
      (void) WriteBlobMSBShort(image,(unsigned short) (image->storage_class ==
        PseudoClass ? 8 : image->depth > 8 ? 16 : 8));

      if (((image_info->colorspace != UndefinedColorspace) ||
           (image->colorspace != CMYKColorspace)) &&
          (image_info->colorspace != CMYKColorspace))
        {
          (void) TransformImageColorspace(image,sRGBColorspace,exception);
          (void) WriteBlobMSBShort(image,(unsigned short)
            (image->storage_class == PseudoClass ? IndexedMode : RGBMode));
        }
      else
        {
          if (image->colorspace != CMYKColorspace)
            (void) TransformImageColorspace(image,CMYKColorspace,exception);
          (void) WriteBlobMSBShort(image,CMYKMode);
        }
    }
  if ((IsImageGray(image) != MagickFalse) ||
      (image->storage_class == DirectClass) || (image->colors > 256))
    (void) WriteBlobMSBLong(image,0);
  else
    {
      /*
        Write PSD raster colormap.
      */
      (void) WriteBlobMSBLong(image,768);
      for (i=0; i < (ssize_t) image->colors; i++)
        (void) WriteBlobByte(image,ScaleQuantumToChar(image->colormap[i].red));
      for ( ; i < 256; i++)
        (void) WriteBlobByte(image,0);
      for (i=0; i < (ssize_t) image->colors; i++)
        (void) WriteBlobByte(image,ScaleQuantumToChar(
          image->colormap[i].green));
      for ( ; i < 256; i++)
        (void) WriteBlobByte(image,0);
      for (i=0; i < (ssize_t) image->colors; i++)
        (void) WriteBlobByte(image,ScaleQuantumToChar(image->colormap[i].blue));
      for ( ; i < 256; i++)
        (void) WriteBlobByte(image,0);
    }
  /*
    Image resource block.
  */
  length=28; /* 0x03EB */
  bim_profile=(StringInfo *) GetImageProfile(image,"8bim");
  icc_profile=GetImageProfile(image,"icc");
  if (bim_profile != (StringInfo *) NULL)
    {
      bim_profile=CloneStringInfo(bim_profile);
      if (icc_profile != (StringInfo *) NULL)
        RemoveICCProfileFromResourceBlock(bim_profile);
      RemoveResolutionFromResourceBlock(bim_profile);
      length+=PSDQuantum(GetStringInfoLength(bim_profile));
    }
  if (icc_profile != (const StringInfo *) NULL)
    length+=PSDQuantum(GetStringInfoLength(icc_profile))+12;
  (void) WriteBlobMSBLong(image,(unsigned int) length);
  WriteResolutionResourceBlock(image);
  if (bim_profile != (StringInfo *) NULL)
    {
      (void) WriteBlob(image,GetStringInfoLength(bim_profile),
        GetStringInfoDatum(bim_profile));
      bim_profile=DestroyStringInfo(bim_profile);
    }
  if (icc_profile != (StringInfo *) NULL)
    {
      (void) WriteBlob(image,4,(const unsigned char *) "8BIM");
      (void) WriteBlobMSBShort(image,0x0000040F);
      (void) WriteBlobMSBShort(image,0);
      (void) WriteBlobMSBLong(image,(unsigned int) GetStringInfoLength(
        icc_profile));
      (void) WriteBlob(image,GetStringInfoLength(icc_profile),
        GetStringInfoDatum(icc_profile));
      if ((MagickOffsetType) GetStringInfoLength(icc_profile) !=
          PSDQuantum(GetStringInfoLength(icc_profile)))
        (void) WriteBlobByte(image,0);
    }
  layer_count=0;
  layer_info_size=2;
  base_image=GetNextImageInList(image);
  if ((image->alpha_trait != UndefinedPixelTrait) && (base_image == (Image *) NULL))
    base_image=image;
  next_image=base_image;
  while ( next_image != NULL )
  {
    packet_size=next_image->depth > 8 ? 2UL : 1UL;
    if (IsImageGray(next_image) != MagickFalse)
      num_channels=next_image->alpha_trait != UndefinedPixelTrait ? 2UL : 1UL;
    else
      if (next_image->storage_class == PseudoClass)
        num_channels=next_image->alpha_trait != UndefinedPixelTrait ? 2UL : 1UL;
      else
        if (next_image->colorspace != CMYKColorspace)
          num_channels=next_image->alpha_trait != UndefinedPixelTrait ? 4UL : 3UL;
        else
          num_channels=next_image->alpha_trait != UndefinedPixelTrait ? 5UL : 4UL;
    channelLength=(size_t) (next_image->columns*next_image->rows*packet_size+2);
    layer_info_size+=(size_t) (4*4+2+num_channels*6+(psd_info.version == 1 ? 8 :
      16)+4*1+4+num_channels*channelLength);
    property=(const char *) GetImageProperty(next_image,"label",exception);
    if (property == (const char *) NULL)
      layer_info_size+=16;
    else
      {
        size_t
          layer_length;

        layer_length=strlen(property);
        layer_info_size+=8+layer_length+(4-(layer_length % 4));
      }
    layer_count++;
    next_image=GetNextImageInList(next_image);
  }
  if (layer_count == 0)
    (void) SetPSDSize(&psd_info,image,0);
  else
    {
      CompressionType
        compression;

      (void) SetPSDSize(&psd_info,image,layer_info_size+
        (psd_info.version == 1 ? 8 : 16));
      if ((layer_info_size/2) != ((layer_info_size+1)/2))
        rounded_layer_info_size=layer_info_size+1;
      else
        rounded_layer_info_size=layer_info_size;
      (void) SetPSDSize(&psd_info,image,rounded_layer_info_size);
      if (image->alpha_trait != UndefinedPixelTrait)
        (void) WriteBlobMSBShort(image,-(unsigned short) layer_count);
      else
        (void) WriteBlobMSBShort(image,(unsigned short) layer_count);
      layer_count=1;
      compression=base_image->compression;
      for (next_image=base_image; next_image != NULL; )
      {
        next_image->compression=NoCompression;
        (void) WriteBlobMSBLong(image,(unsigned int) next_image->page.y);
        (void) WriteBlobMSBLong(image,(unsigned int) next_image->page.x);
        (void) WriteBlobMSBLong(image,(unsigned int) (next_image->page.y+
          next_image->rows));
        (void) WriteBlobMSBLong(image,(unsigned int) (next_image->page.x+
          next_image->columns));
        packet_size=next_image->depth > 8 ? 2UL : 1UL;
        channel_size=(unsigned int) ((packet_size*next_image->rows*
          next_image->columns)+2);
        if ((IsImageGray(next_image) != MagickFalse) ||
            (next_image->storage_class == PseudoClass))
          {
             (void) WriteBlobMSBShort(image,(unsigned short)
               (next_image->alpha_trait != UndefinedPixelTrait ? 2 : 1));
             (void) WriteBlobMSBShort(image,0);
             (void) SetPSDSize(&psd_info,image,channel_size);
             if (next_image->alpha_trait != UndefinedPixelTrait)
               {
                 (void) WriteBlobMSBShort(image,(unsigned short) -1);
                 (void) SetPSDSize(&psd_info,image,channel_size);
               }
           }
          else
            if (next_image->colorspace != CMYKColorspace)
              {
                (void) WriteBlobMSBShort(image,(unsigned short)
                  (next_image->alpha_trait != UndefinedPixelTrait ? 4 : 3));
               (void) WriteBlobMSBShort(image,0);
               (void) SetPSDSize(&psd_info,image,channel_size);
               (void) WriteBlobMSBShort(image,1);
               (void) SetPSDSize(&psd_info,image,channel_size);
               (void) WriteBlobMSBShort(image,2);
               (void) SetPSDSize(&psd_info,image,channel_size);
               if (next_image->alpha_trait != UndefinedPixelTrait)
                 {
                   (void) WriteBlobMSBShort(image,(unsigned short) -1);
                   (void) SetPSDSize(&psd_info,image,channel_size);
                 }
             }
           else
             {
               (void) WriteBlobMSBShort(image,(unsigned short)
                 (next_image->alpha_trait ? 5 : 4));
               (void) WriteBlobMSBShort(image,0);
               (void) SetPSDSize(&psd_info,image,channel_size);
               (void) WriteBlobMSBShort(image,1);
               (void) SetPSDSize(&psd_info,image,channel_size);
               (void) WriteBlobMSBShort(image,2);
               (void) SetPSDSize(&psd_info,image,channel_size);
               (void) WriteBlobMSBShort(image,3);
               (void) SetPSDSize(&psd_info,image,channel_size);
               if (next_image->alpha_trait)
                 {
                   (void) WriteBlobMSBShort(image,(unsigned short) -1);
                   (void) SetPSDSize(&psd_info,image,channel_size);
                 }
             }
        (void) WriteBlob(image,4,(const unsigned char *) "8BIM");
        (void) WriteBlob(image,4,(const unsigned char *)
          CompositeOperatorToPSDBlendMode(next_image->compose));
        (void) WriteBlobByte(image,255); /* layer opacity */
        (void) WriteBlobByte(image,0);
        (void) WriteBlobByte(image,next_image->compose==NoCompositeOp ?
          1 << 0x02 : 1); /* layer properties - visible, etc. */
        (void) WriteBlobByte(image,0);
        property=(const char *) GetImageProperty(next_image,"label",exception);
        if (property == (const char *) NULL)
          {
            char
              layer_name[MagickPathExtent];

            (void) WriteBlobMSBLong(image,16);
            (void) WriteBlobMSBLong(image,0);
            (void) WriteBlobMSBLong(image,0);
            (void) FormatLocaleString(layer_name,MagickPathExtent,"L%04ld",(long)
              layer_count++);
            WritePascalString(image,layer_name,4);
          }
        else
          {
            size_t
              label_length;

            label_length=strlen(property);
            (void) WriteBlobMSBLong(image,(unsigned int) (label_length+(4-
              (label_length % 4))+8));
            (void) WriteBlobMSBLong(image,0);
            (void) WriteBlobMSBLong(image,0);
            WritePascalString(image,property,4);
          }
        next_image=GetNextImageInList(next_image);
      }
      /*
        Now the image data!
      */
      next_image=base_image;
      while (next_image != NULL)
      {
        status=WriteImageChannels(&psd_info,image_info,image,next_image,
          MagickTrue,exception);
        next_image=GetNextImageInList(next_image);
      }
      (void) WriteBlobMSBLong(image,0);  /* user mask data */
      base_image->compression=compression;
    }
  /*
    Write composite image.
  */
  if (status != MagickFalse)
    status=WriteImageChannels(&psd_info,image_info,image,image,MagickFalse,
      exception);
  (void) CloseBlob(image);
  return(status);
}
