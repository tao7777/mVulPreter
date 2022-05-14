static Image *ReadPCDImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  MagickBooleanType
    status;

  MagickOffsetType
    offset;

  MagickSizeType
    number_pixels;

  register ssize_t
    i,
    y;

  register PixelPacket
    *q;

  register unsigned char
    *c1,
    *c2,
    *yy;

  size_t
    height,
    number_images,
    rotate,
    scene,
    width;

  ssize_t
    count,
    x;

  unsigned char
    *chroma1,
    *chroma2,
    *header,
    *luma;

  unsigned int
    overview;

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
    Determine if this a PCD file.
  */
  header=(unsigned char *) AcquireQuantumMemory(0x800,3UL*sizeof(*header));
  if (header == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  count=ReadBlob(image,3*0x800,header);
  overview=LocaleNCompare((char *) header,"PCD_OPA",7) == 0;
  if ((count == 0) ||
      ((LocaleNCompare((char *) header+0x800,"PCD",3) != 0) && (overview == 0)))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  rotate=header[0x0e02] & 0x03;
  number_images=(header[10] << 8) | header[11];
  if (number_images > 65535)
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  header=(unsigned char *) RelinquishMagickMemory(header);
  /*
    Determine resolution by scene specification.
  */
  if ((image->columns == 0) || (image->rows == 0))
    scene=3;
  else
    {
      width=192;
      height=128;
      for (scene=1; scene < 6; scene++)
      {
        if ((width >= image->columns) && (height >= image->rows))
          break;
        width<<=1;
        height<<=1;
      }
    }
  if (image_info->number_scenes != 0)
    scene=(size_t) MagickMin(image_info->scene,6);
  if (overview != 0)
    scene=1;
  /*
    Initialize image structure.
  */
  width=192;
  height=128;
  for (i=1; i < (ssize_t) MagickMin(scene,3); i++)
  {
    width<<=1;
    height<<=1;
  }
  image->columns=width;
  image->rows=height;
  image->depth=8;
  for ( ; i < (ssize_t) scene; i++)
  {
     image->columns<<=1;
     image->rows<<=1;
   }
  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }
   /*
     Allocate luma and chroma memory.
   */
  number_pixels=(MagickSizeType) image->columns*image->rows;
  if (number_pixels != (size_t) number_pixels)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  chroma1=(unsigned char *) AcquireQuantumMemory(image->columns+1UL,image->rows*
    sizeof(*chroma1));
  chroma2=(unsigned char *) AcquireQuantumMemory(image->columns+1UL,image->rows*
    sizeof(*chroma2));
  luma=(unsigned char *) AcquireQuantumMemory(image->columns+1UL,image->rows*
    sizeof(*luma));
  if ((chroma1 == (unsigned char *) NULL) ||
      (chroma2 == (unsigned char *) NULL) || (luma == (unsigned char *) NULL))
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  /*
    Advance to image data.
  */
  offset=93;
  if (overview != 0)
    offset=2;
  else
    if (scene == 2)
      offset=20;
    else
      if (scene <= 1)
        offset=1;
  for (i=0; i < (ssize_t) (offset*0x800); i++)
    (void) ReadBlobByte(image);
  if (overview != 0)
    {
      Image
        *overview_image;

      MagickProgressMonitor
        progress_monitor;

      register ssize_t
        j;

      /*
        Read thumbnails from overview image.
      */
      for (j=1; j <= (ssize_t) number_images; j++)
      {
        progress_monitor=SetImageProgressMonitor(image,
          (MagickProgressMonitor) NULL,image->client_data);
        (void) FormatLocaleString(image->filename,MaxTextExtent,
          "images/img%04ld.pcd",(long) j);
        (void) FormatLocaleString(image->magick_filename,MaxTextExtent,
          "images/img%04ld.pcd",(long) j);
        image->scene=(size_t) j;
        image->columns=width;
        image->rows=height;
        image->depth=8;
        yy=luma;
        c1=chroma1;
        c2=chroma2;
        for (y=0; y < (ssize_t) height; y+=2)
        {
          count=ReadBlob(image,width,yy);
          yy+=image->columns;
          count=ReadBlob(image,width,yy);
          yy+=image->columns;
          count=ReadBlob(image,width >> 1,c1);
          c1+=image->columns;
          count=ReadBlob(image,width >> 1,c2);
          c2+=image->columns;
        }
        Upsample(image->columns >> 1,image->rows >> 1,image->columns,chroma1);
        Upsample(image->columns >> 1,image->rows >> 1,image->columns,chroma2);
        /*
          Transfer luminance and chrominance channels.
        */
        yy=luma;
        c1=chroma1;
        c2=chroma2;
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            SetPixelRed(q,ScaleCharToQuantum(*yy++));
            SetPixelGreen(q,ScaleCharToQuantum(*c1++));
            SetPixelBlue(q,ScaleCharToQuantum(*c2++));
            q++;
          }
          if (SyncAuthenticPixels(image,exception) == MagickFalse)
            break;
        }
        image->colorspace=YCCColorspace;
        if (LocaleCompare(image_info->magick,"PCDS") == 0)
          SetImageColorspace(image,sRGBColorspace);
        if (j < (ssize_t) number_images)
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
          }
        (void) SetImageProgressMonitor(image,progress_monitor,
          image->client_data);
        if (image->previous == (Image *) NULL)
          {
            status=SetImageProgress(image,LoadImageTag,j-1,number_images);
            if (status == MagickFalse)
              break;
          }
      }
      chroma2=(unsigned char *) RelinquishMagickMemory(chroma2);
      chroma1=(unsigned char *) RelinquishMagickMemory(chroma1);
      luma=(unsigned char *) RelinquishMagickMemory(luma);
      image=GetFirstImageInList(image);
      overview_image=OverviewImage(image_info,image,exception);
      return(overview_image);
    }
  /*
    Read interleaved image.
  */
  yy=luma;
  c1=chroma1;
  c2=chroma2;
  for (y=0; y < (ssize_t) height; y+=2)
  {
    count=ReadBlob(image,width,yy);
    yy+=image->columns;
    count=ReadBlob(image,width,yy);
    yy+=image->columns;
    count=ReadBlob(image,width >> 1,c1);
    c1+=image->columns;
    count=ReadBlob(image,width >> 1,c2);
    c2+=image->columns;
  }
  if (scene >= 4)
    {
      /*
        Recover luminance deltas for 1536x1024 image.
      */
      Upsample(768,512,image->columns,luma);
      Upsample(384,256,image->columns,chroma1);
      Upsample(384,256,image->columns,chroma2);
      image->rows=1024;
      for (i=0; i < (4*0x800); i++)
        (void) ReadBlobByte(image);
      status=DecodeImage(image,luma,chroma1,chroma2);
      if ((scene >= 5) && status)
        {
          /*
            Recover luminance deltas for 3072x2048 image.
          */
          Upsample(1536,1024,image->columns,luma);
          Upsample(768,512,image->columns,chroma1);
          Upsample(768,512,image->columns,chroma2);
          image->rows=2048;
          offset=TellBlob(image)/0x800+12;
          offset=SeekBlob(image,offset*0x800,SEEK_SET);
          status=DecodeImage(image,luma,chroma1,chroma2);
          if ((scene >= 6) && (status != MagickFalse))
            {
              /*
                Recover luminance deltas for 6144x4096 image (vaporware).
              */
              Upsample(3072,2048,image->columns,luma);
              Upsample(1536,1024,image->columns,chroma1);
              Upsample(1536,1024,image->columns,chroma2);
              image->rows=4096;
            }
        }
    }
  Upsample(image->columns >> 1,image->rows >> 1,image->columns,chroma1);
  Upsample(image->columns >> 1,image->rows >> 1,image->columns,chroma2);
  /*
    Transfer luminance and chrominance channels.
  */
  yy=luma;
  c1=chroma1;
  c2=chroma2;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      SetPixelRed(q,ScaleCharToQuantum(*yy++));
      SetPixelGreen(q,ScaleCharToQuantum(*c1++));
      SetPixelBlue(q,ScaleCharToQuantum(*c2++));
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
  chroma2=(unsigned char *) RelinquishMagickMemory(chroma2);
  chroma1=(unsigned char *) RelinquishMagickMemory(chroma1);
  luma=(unsigned char *) RelinquishMagickMemory(luma);
  if (EOFBlob(image) != MagickFalse)
    ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
      image->filename);
  (void) CloseBlob(image);
  if (image_info->ping == MagickFalse)
    if ((rotate == 1) || (rotate == 3))
      {
        double
          degrees;

        Image
          *rotate_image;

        /*
          Rotate image.
        */
        degrees=rotate == 1 ? -90.0 : 90.0;
        rotate_image=RotateImage(image,degrees,exception);
        if (rotate_image != (Image *) NULL)
          {
            image=DestroyImage(image);
            image=rotate_image;
          }
      }
  /*
    Set CCIR 709 primaries with a D65 white point.
  */
  image->chromaticity.red_primary.x=0.6400f;
  image->chromaticity.red_primary.y=0.3300f;
  image->chromaticity.green_primary.x=0.3000f;
  image->chromaticity.green_primary.y=0.6000f;
  image->chromaticity.blue_primary.x=0.1500f;
  image->chromaticity.blue_primary.y=0.0600f;
  image->chromaticity.white_point.x=0.3127f;
  image->chromaticity.white_point.y=0.3290f;
  image->gamma=1.000f/2.200f;
  image->colorspace=YCCColorspace;
  if (LocaleCompare(image_info->magick,"PCDS") == 0)
    SetImageColorspace(image,sRGBColorspace);
  return(GetFirstImageInList(image));
}
