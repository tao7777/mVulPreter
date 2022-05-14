MagickExport MagickBooleanType SetImageType(Image *image,const ImageType type)
{
  const char
    *artifact;

  ImageInfo
    *image_info;

  MagickBooleanType
    status;

  QuantizeInfo
    *quantize_info;

  assert(image != (Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image->signature == MagickSignature);
  status=MagickTrue;
  image_info=AcquireImageInfo();
  image_info->dither=image->dither;
  artifact=GetImageArtifact(image,"dither");
  if (artifact != (const char *) NULL)
    (void) SetImageOption(image_info,"dither",artifact);
  switch (type)
  {
    case BilevelType:
    {
      if (SetImageMonochrome(image,&image->exception) == MagickFalse)
        {
          status=TransformImageColorspace(image,GRAYColorspace);
          (void) NormalizeImage(image);
          quantize_info=AcquireQuantizeInfo(image_info);
          quantize_info->number_colors=2;
          quantize_info->colorspace=GRAYColorspace;
           status=QuantizeImage(quantize_info,image);
           quantize_info=DestroyQuantizeInfo(quantize_info);
         }
      status=AcquireImageColormap(image,2);
       image->matte=MagickFalse;
       break;
     }
    case GrayscaleType:
    {
      if (SetImageGray(image,&image->exception) == MagickFalse)
        status=TransformImageColorspace(image,GRAYColorspace);
      image->matte=MagickFalse;
      break;
    }
    case GrayscaleMatteType:
    {
      if (SetImageGray(image,&image->exception) == MagickFalse)
        status=TransformImageColorspace(image,GRAYColorspace);
      if (image->matte == MagickFalse)
        (void) SetImageAlphaChannel(image,OpaqueAlphaChannel);
      break;
    }
    case PaletteType:
    {
      if (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse)
        status=TransformImageColorspace(image,sRGBColorspace);
      if ((image->storage_class == DirectClass) || (image->colors > 256))
        {
          quantize_info=AcquireQuantizeInfo(image_info);
          quantize_info->number_colors=256;
          status=QuantizeImage(quantize_info,image);
          quantize_info=DestroyQuantizeInfo(quantize_info);
        }
      image->matte=MagickFalse;
      break;
    }
    case PaletteBilevelMatteType:
    {
      if (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse)
        status=TransformImageColorspace(image,sRGBColorspace);
      if (image->matte == MagickFalse)
        (void) SetImageAlphaChannel(image,OpaqueAlphaChannel);
      (void) BilevelImageChannel(image,AlphaChannel,(double) QuantumRange/2.0);
      quantize_info=AcquireQuantizeInfo(image_info);
      status=QuantizeImage(quantize_info,image);
      quantize_info=DestroyQuantizeInfo(quantize_info);
      break;
    }
    case PaletteMatteType:
    {
      if (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse)
        status=TransformImageColorspace(image,sRGBColorspace);
      if (image->matte == MagickFalse)
        (void) SetImageAlphaChannel(image,OpaqueAlphaChannel);
      quantize_info=AcquireQuantizeInfo(image_info);
      quantize_info->colorspace=TransparentColorspace;
      status=QuantizeImage(quantize_info,image);
      quantize_info=DestroyQuantizeInfo(quantize_info);
      break;
    }
    case TrueColorType:
    {
      if (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse)
        status=TransformImageColorspace(image,sRGBColorspace);
      if (image->storage_class != DirectClass)
        status=SetImageStorageClass(image,DirectClass);
      image->matte=MagickFalse;
      break;
    }
    case TrueColorMatteType:
    {
      if (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse)
        status=TransformImageColorspace(image,sRGBColorspace);
      if (image->storage_class != DirectClass)
        status=SetImageStorageClass(image,DirectClass);
      if (image->matte == MagickFalse)
        (void) SetImageAlphaChannel(image,OpaqueAlphaChannel);
      break;
    }
    case ColorSeparationType:
    {
      if (image->colorspace != CMYKColorspace)
        {
          if (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse)
            (void) TransformImageColorspace(image,sRGBColorspace);
          status=TransformImageColorspace(image,CMYKColorspace);
        }
      if (image->storage_class != DirectClass)
        status=SetImageStorageClass(image,DirectClass);
      image->matte=MagickFalse;
      break;
    }
    case ColorSeparationMatteType:
    {
      if (image->colorspace != CMYKColorspace)
        {
          if (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse)
            (void) TransformImageColorspace(image,sRGBColorspace);
          status=TransformImageColorspace(image,CMYKColorspace);
        }
      if (image->storage_class != DirectClass)
        status=SetImageStorageClass(image,DirectClass);
      if (image->matte == MagickFalse)
        (void) SetImageAlphaChannel(image,OpaqueAlphaChannel);
      break;
    }
    case OptimizeType:
    case UndefinedType:
      break;
  }
  image_info=DestroyImageInfo(image_info);
  if (status == MagickFalse)
    return(MagickFalse);
  image->type=type;
  return(MagickTrue);
}
