static MagickBooleanType WriteTXTImage(const ImageInfo *image_info,Image *image,
  ExceptionInfo *exception)
{
  char
    buffer[MagickPathExtent],
    colorspace[MagickPathExtent],
    tuple[MagickPathExtent];

  MagickBooleanType
    status;

  MagickOffsetType
    scene;

  PixelInfo
    pixel;

  register const Quantum
    *p;

  register ssize_t
    x;

  ssize_t
    y;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  status=OpenBlob(image_info,image,WriteBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  scene=0;
  do
  {
    ComplianceType
      compliance;

    const char
      *value;

    (void) CopyMagickString(colorspace,CommandOptionToMnemonic(
      MagickColorspaceOptions,(ssize_t) image->colorspace),MagickPathExtent);
    LocaleLower(colorspace);
    image->depth=GetImageQuantumDepth(image,MagickTrue);
    if (image->alpha_trait != UndefinedPixelTrait)
      (void) ConcatenateMagickString(colorspace,"a",MagickPathExtent);
    compliance=NoCompliance;
    value=GetImageOption(image_info,"txt:compliance");
    if (value != (char *) NULL)
      compliance=(ComplianceType) ParseCommandOption(MagickComplianceOptions,
        MagickFalse,value);
    if (LocaleCompare(image_info->magick,"SPARSE-COLOR") != 0)
      {
        size_t
          depth;

        depth=compliance == SVGCompliance ? image->depth :
          MAGICKCORE_QUANTUM_DEPTH;
        (void) FormatLocaleString(buffer,MagickPathExtent,
          "# ImageMagick pixel enumeration: %.20g,%.20g,%.20g,%s\n",(double)
          image->columns,(double) image->rows,(double) ((MagickOffsetType)
          GetQuantumRange(depth)),colorspace);
        (void) WriteBlobString(image,buffer);
      }
    GetPixelInfo(image,&pixel);
    for (y=0; y < (ssize_t) image->rows; y++)
    {
      p=GetVirtualPixels(image,0,y,image->columns,1,exception);
      if (p == (const Quantum *) NULL)
        break;
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        GetPixelInfoPixel(image,p,&pixel);
        if (pixel.colorspace == LabColorspace)
          {
            pixel.green-=(QuantumRange+1)/2.0;
            pixel.blue-=(QuantumRange+1)/2.0;
          }
        if (LocaleCompare(image_info->magick,"SPARSE-COLOR") == 0)
          {
            /*
              Sparse-color format.
            */
            if (GetPixelAlpha(image,p) == (Quantum) OpaqueAlpha)
              {
                GetColorTuple(&pixel,MagickFalse,tuple);
                (void) FormatLocaleString(buffer,MagickPathExtent,
                  "%.20g,%.20g,",(double) x,(double) y);
                (void) WriteBlobString(image,buffer);
                (void) WriteBlobString(image,tuple);
                (void) WriteBlobString(image," ");
              }
            p+=GetPixelChannels(image);
            continue;
          }
        (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g,%.20g: ",
          (double) x,(double) y);
         (void) WriteBlobString(image,buffer);
         (void) CopyMagickString(tuple,"(",MagickPathExtent);
         if (pixel.colorspace == GRAYColorspace)
          ConcatenateColorComponent(&pixel,GrayPixelChannel,compliance,tuple);
         else
           {
             ConcatenateColorComponent(&pixel,RedPixelChannel,compliance,tuple);
            (void) ConcatenateMagickString(tuple,",",MagickPathExtent);
            ConcatenateColorComponent(&pixel,GreenPixelChannel,compliance,
              tuple);
            (void) ConcatenateMagickString(tuple,",",MagickPathExtent);
            ConcatenateColorComponent(&pixel,BluePixelChannel,compliance,tuple);
          }
        if (pixel.colorspace == CMYKColorspace)
          {
            (void) ConcatenateMagickString(tuple,",",MagickPathExtent);
            ConcatenateColorComponent(&pixel,BlackPixelChannel,compliance,
              tuple);
          }
        if (pixel.alpha_trait != UndefinedPixelTrait)
          {
            (void) ConcatenateMagickString(tuple,",",MagickPathExtent);
            ConcatenateColorComponent(&pixel,AlphaPixelChannel,compliance,
              tuple);
          }
        (void) ConcatenateMagickString(tuple,")",MagickPathExtent);
        (void) WriteBlobString(image,tuple);
        (void) WriteBlobString(image,"  ");
        GetColorTuple(&pixel,MagickTrue,tuple);
        (void) FormatLocaleString(buffer,MagickPathExtent,"%s",tuple);
        (void) WriteBlobString(image,buffer);
        (void) WriteBlobString(image,"  ");
        (void) QueryColorname(image,&pixel,SVGCompliance,tuple,exception);
        (void) WriteBlobString(image,tuple);
        (void) WriteBlobString(image,"\n");
        p+=GetPixelChannels(image);
      }
      status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
        image->rows);
      if (status == MagickFalse)
        break;
    }
    if (GetNextImageInList(image) == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=SetImageProgress(image,SaveImagesTag,scene++,
      GetImageListLength(image));
    if (status == MagickFalse)
      break;
  } while (image_info->adjoin != MagickFalse);
  (void) CloseBlob(image);
  return(MagickTrue);
}
