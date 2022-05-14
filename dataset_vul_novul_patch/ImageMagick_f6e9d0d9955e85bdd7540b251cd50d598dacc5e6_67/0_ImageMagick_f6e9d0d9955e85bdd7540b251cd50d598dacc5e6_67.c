static Image *ReadTXTImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    colorspace[MaxTextExtent],
    text[MaxTextExtent];

  Image
    *image;

  IndexPacket
    *indexes;

  long
    type,
    x_offset,
    y,
    y_offset;

  MagickBooleanType
    status;

  MagickPixelPacket
    pixel;

  QuantumAny
    range;

  register ssize_t
    i,
    x;

  register PixelPacket
    *q;

  ssize_t
    count;

  unsigned long
    depth,
    height,
    max_value,
    width;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AcquireImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  (void) ResetMagickMemory(text,0,sizeof(text));
  (void) ReadBlobString(image,text);
  if (LocaleNCompare((char *) text,MagickID,strlen(MagickID)) != 0)
    return(ReadTEXTImage(image_info,image,text,exception));
  do
  {
    width=0;
    height=0;
    max_value=0;
    *colorspace='\0';
    count=(ssize_t) sscanf(text+32,"%lu,%lu,%lu,%s",&width,&height,&max_value,
      colorspace);
    if ((count != 4) || (width == 0) || (height == 0) || (max_value == 0))
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    image->columns=width;
     image->rows=height;
     for (depth=1; (GetQuantumRange(depth)+1) < max_value; depth++) ;
     image->depth=depth;
    status=SetImageExtent(image,image->columns,image->rows);
    if (status == MagickFalse)
      {
        InheritException(exception,&image->exception);
        return(DestroyImageList(image));
      }
     LocaleLower(colorspace);
     i=(ssize_t) strlen(colorspace)-1;
     image->matte=MagickFalse;
    if ((i > 0) && (colorspace[i] == 'a'))
      {
        colorspace[i]='\0';
        image->matte=MagickTrue;
      }
    type=ParseCommandOption(MagickColorspaceOptions,MagickFalse,colorspace);
    if (type < 0)
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    image->colorspace=(ColorspaceType) type;
    (void) ResetMagickMemory(&pixel,0,sizeof(pixel));
    (void) SetImageBackgroundColor(image);
    range=GetQuantumRange(image->depth);
    for (y=0; y < (ssize_t) image->rows; y++)
    {
      double
        blue,
        green,
        index,
        opacity,
        red;

      red=0.0;
      green=0.0;
      blue=0.0;
      index=0.0;
      opacity=0.0;
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        if (ReadBlobString(image,text) == (char *) NULL)
          break;
        switch (image->colorspace)
        {
          case GRAYColorspace:
          {
            if (image->matte != MagickFalse)
              {
                count=(ssize_t) sscanf(text,"%ld,%ld: (%lf%*[%,]%lf%*[%,]",
                  &x_offset,&y_offset,&red,&opacity);
                green=red;
                blue=red;
                break;
              }
            count=(ssize_t) sscanf(text,"%ld,%ld: (%lf%*[%,]",&x_offset,
              &y_offset,&red);
            green=red;
            blue=red;
            break;
          }
          case CMYKColorspace:
          {
            if (image->matte != MagickFalse)
              {
                count=(ssize_t) sscanf(text,
                  "%ld,%ld: (%lf%*[%,]%lf%*[%,]%lf%*[%,]%lf%*[%,]%lf%*[%,]",
                  &x_offset,&y_offset,&red,&green,&blue,&index,&opacity);
                break;
              }
            count=(ssize_t) sscanf(text,
              "%ld,%ld: (%lf%*[%,]%lf%*[%,]%lf%*[%,]%lf%*[%,]",&x_offset,
              &y_offset,&red,&green,&blue,&index);
            break;
          }
          default:
          {
            if (image->matte != MagickFalse)
              {
                count=(ssize_t) sscanf(text,
                  "%ld,%ld: (%lf%*[%,]%lf%*[%,]%lf%*[%,]%lf%*[%,]",
                  &x_offset,&y_offset,&red,&green,&blue,&opacity);
                break;
              }
            count=(ssize_t) sscanf(text,
              "%ld,%ld: (%lf%*[%,]%lf%*[%,]%lf%*[%,]",&x_offset,&y_offset,
               &red,&green,&blue);
            break;
          }
        }
        if (strchr(text,'%') != (char *) NULL)
          {
            red*=0.01*range;
            green*=0.01*range;
            blue*=0.01*range;
            index*=0.01*range;
            opacity*=0.01*range;
          }
        if (image->colorspace == LabColorspace)
          {
            green+=(range+1)/2.0;
            blue+=(range+1)/2.0;
          }
        pixel.red=ScaleAnyToQuantum((QuantumAny) (red+0.5),range);
        pixel.green=ScaleAnyToQuantum((QuantumAny) (green+0.5),range);
        pixel.blue=ScaleAnyToQuantum((QuantumAny) (blue+0.5),range);
        pixel.index=ScaleAnyToQuantum((QuantumAny) (index+0.5),range);
        pixel.opacity=ScaleAnyToQuantum((QuantumAny) (opacity+0.5),range);
        q=GetAuthenticPixels(image,x_offset,y_offset,1,1,exception);
        if (q == (PixelPacket *) NULL)
          continue;
        SetPixelRed(q,pixel.red);
        SetPixelGreen(q,pixel.green);
        SetPixelBlue(q,pixel.blue);
        if (image->colorspace == CMYKColorspace)
          {
            indexes=GetAuthenticIndexQueue(image);
            SetPixelIndex(indexes,pixel.index);
          }
        if (image->matte != MagickFalse)
          SetPixelAlpha(q,pixel.opacity);
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
      }
    }
    (void) ReadBlobString(image,text);
    if (LocaleNCompare((char *) text,MagickID,strlen(MagickID)) == 0)
      {
        /*
          Allocate next image structure.
        */
        AcquireNextImage(image_info,image);
        if (GetNextImageInList(image) == (Image *) NULL)
          {
            image=DestroyImageList(image);
            return((Image *) NULL);
          }
        image=SyncNextImageInList(image);
        status=SetImageProgress(image,LoadImagesTag,TellBlob(image),
          GetBlobSize(image));
        if (status == MagickFalse)
          break;
      }
  } while (LocaleNCompare((char *) text,MagickID,strlen(MagickID)) == 0);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
