static Image *ReadFITSImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  typedef struct _FITSInfo
  {
    MagickBooleanType
      extend,
      simple;

    int
      bits_per_pixel,
      columns,
      rows,
      number_axes,
      number_planes;

    double
      min_data,
      max_data,
      zero,
      scale;

    EndianType
      endian;
  } FITSInfo;

  char
    *comment,
    keyword[9],
    property[MaxTextExtent],
    value[73];

  double
    pixel,
    scale;

  FITSInfo
    fits_info;

  Image
    *image;

  int
    c;

  MagickBooleanType
    status;

  MagickSizeType
    number_pixels;

  register ssize_t
    i,
    x;

  register PixelPacket
    *q;

  ssize_t
    count,
    scene,
    y;

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
  /*
    Initialize image header.
  */
  (void) ResetMagickMemory(&fits_info,0,sizeof(fits_info));
  fits_info.extend=MagickFalse;
  fits_info.simple=MagickFalse;
  fits_info.bits_per_pixel=8;
  fits_info.columns=1;
  fits_info.rows=1;
  fits_info.rows=1;
  fits_info.number_planes=1;
  fits_info.min_data=0.0;
  fits_info.max_data=0.0;
  fits_info.zero=0.0;
  fits_info.scale=1.0;
  fits_info.endian=MSBEndian;
  /*
    Decode image header.
  */
  for (comment=(char *) NULL; EOFBlob(image) == MagickFalse; )
  {
    for ( ; EOFBlob(image) == MagickFalse; )
    {
      register char
        *p;

      count=ReadBlob(image,8,(unsigned char *) keyword);
      if (count != 8)
        break;
      for (i=0; i < 8; i++)
      {
        if (isspace((int) ((unsigned char) keyword[i])) != 0)
          break;
        keyword[i]=tolower((int) ((unsigned char) keyword[i]));
      }
      keyword[i]='\0';
      count=ReadBlob(image,72,(unsigned char *) value);
      value[72]='\0';
      if (count != 72)
        break;
      p=value;
      if (*p == '=')
        {
          p+=2;
          while (isspace((int) ((unsigned char) *p)) != 0)
            p++;
        }
      if (LocaleCompare(keyword,"end") == 0)
        break;
      if (LocaleCompare(keyword,"extend") == 0)
        fits_info.extend=(*p == 'T') || (*p == 't') ? MagickTrue : MagickFalse;
      if (LocaleCompare(keyword,"simple") == 0)
        fits_info.simple=(*p == 'T') || (*p == 't') ? MagickTrue : MagickFalse;
      if (LocaleCompare(keyword,"bitpix") == 0)
        fits_info.bits_per_pixel=StringToLong(p);
      if (LocaleCompare(keyword,"naxis") == 0)
        fits_info.number_axes=StringToLong(p);
      if (LocaleCompare(keyword,"naxis1") == 0)
        fits_info.columns=StringToLong(p);
      if (LocaleCompare(keyword,"naxis2") == 0)
        fits_info.rows=StringToLong(p);
      if (LocaleCompare(keyword,"naxis3") == 0)
        fits_info.number_planes=StringToLong(p);
      if (LocaleCompare(keyword,"datamax") == 0)
        fits_info.max_data=StringToDouble(p,(char **) NULL);
      if (LocaleCompare(keyword,"datamin") == 0)
        fits_info.min_data=StringToDouble(p,(char **) NULL);
      if (LocaleCompare(keyword,"bzero") == 0)
        fits_info.zero=StringToDouble(p,(char **) NULL);
      if (LocaleCompare(keyword,"bscale") == 0)
        fits_info.scale=StringToDouble(p,(char **) NULL);
      if (LocaleCompare(keyword,"comment") == 0)
        {
          if (comment == (char *) NULL)
            comment=ConstantString(p);
          else
            (void) ConcatenateString(&comment,p);
        }
      if (LocaleCompare(keyword,"xendian") == 0)
        {
          if (LocaleNCompare(p,"big",3) == 0)
            fits_info.endian=MSBEndian;
          else
            fits_info.endian=LSBEndian;
        }
      (void) FormatLocaleString(property,MaxTextExtent,"fits:%s",keyword);
      (void) SetImageProperty(image,property,p);
    }
    c=0;
    while (((TellBlob(image) % FITSBlocksize) != 0) && (c != EOF))
      c=ReadBlobByte(image);
    if (fits_info.extend == MagickFalse)
      break;
    number_pixels=(MagickSizeType) fits_info.columns*fits_info.rows;
    if ((fits_info.simple != MagickFalse) && (fits_info.number_axes >= 1) &&
        (fits_info.number_axes <= 4) && (number_pixels != 0))
      break;
  }
  /*
    Verify that required image information is defined.
  */
  if (comment != (char *) NULL)
    {
      (void) SetImageProperty(image,"comment",comment);
      comment=DestroyString(comment);
    }
  if (EOFBlob(image) != MagickFalse)
    ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
      image->filename);
  number_pixels=(MagickSizeType) fits_info.columns*fits_info.rows;
  if ((fits_info.simple == MagickFalse) || (fits_info.number_axes < 1) ||
      (fits_info.number_axes > 4) || (number_pixels == 0))
    ThrowReaderException(CorruptImageError,"ImageTypeNotSupported");
  for (scene=0; scene < (ssize_t) fits_info.number_planes; scene++)
  {
    image->columns=(size_t) fits_info.columns;
    image->rows=(size_t) fits_info.rows;
    image->depth=(size_t) (fits_info.bits_per_pixel < 0 ? -1 : 1)*
      fits_info.bits_per_pixel;
    image->endian=fits_info.endian;
    image->scene=(size_t) scene;
     if ((image_info->ping != MagickFalse) && (image_info->number_scenes != 0))
       if (image->scene >= (image_info->scene+image_info->number_scenes-1))
         break;
    status=SetImageExtent(image,image->columns,image->rows);
    if (status == MagickFalse)
      {
        InheritException(exception,&image->exception);
        return(DestroyImageList(image));
      }
     /*
       Initialize image structure.
     */
    (void) SetImageColorspace(image,GRAYColorspace);
    if ((fits_info.min_data == 0.0) && (fits_info.max_data == 0.0))
      {
        if (fits_info.zero == 0.0)
          (void) GetFITSPixelExtrema(image,fits_info.bits_per_pixel,
            &fits_info.min_data,&fits_info.max_data);
        else
          fits_info.max_data=GetFITSPixelRange((size_t)
            fits_info.bits_per_pixel);
      }
    else
      fits_info.max_data=GetFITSPixelRange((size_t) fits_info.bits_per_pixel);
    /*
      Convert FITS pixels to pixel packets.
    */
    scale=QuantumRange/(fits_info.max_data-fits_info.min_data);
    for (y=(ssize_t) image->rows-1; y >= 0; y--)
    {
      q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
      if (q == (PixelPacket *) NULL)
        break;
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        pixel=GetFITSPixel(image,fits_info.bits_per_pixel);
        if ((image->depth == 16) || (image->depth == 32) ||
            (image->depth == 64))
          SetFITSUnsignedPixels(1,image->depth,image->endian,(unsigned char *)
            &pixel);
        SetPixelRed(q,ClampToQuantum(scale*(fits_info.scale*(pixel-
          fits_info.min_data)+fits_info.zero)));
        SetPixelGreen(q,GetPixelRed(q));
        SetPixelBlue(q,GetPixelRed(q));
        q++;
      }
      if (SyncAuthenticPixels(image,exception) == MagickFalse)
        break;
      if (image->previous == (Image *) NULL)
        {
          status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
                image->rows);
          if (status == MagickFalse)
            break;
        }
    }
    if (EOFBlob(image) != MagickFalse)
      {
        ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
          image->filename);
        break;
      }
    /*
      Proceed to next image.
    */
    if (image_info->number_scenes != 0)
      if (image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    if (scene < (ssize_t) (fits_info.number_planes-1))
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
  }
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
