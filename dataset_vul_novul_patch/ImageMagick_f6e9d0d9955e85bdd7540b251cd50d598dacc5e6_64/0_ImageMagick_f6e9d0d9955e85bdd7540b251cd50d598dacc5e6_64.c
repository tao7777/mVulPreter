static Image *ReadTIMImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  typedef struct _TIMInfo
  {
    size_t
      id,
      flag;
  } TIMInfo;

  TIMInfo
    tim_info;

  Image
    *image;

  int
    bits_per_pixel,
    has_clut;

  MagickBooleanType
    status;

  register IndexPacket
    *indexes;

  register ssize_t
    x;

  register PixelPacket
    *q;

  register ssize_t
    i;

  register unsigned char
    *p;

  size_t
    bytes_per_line,
    height,
    image_size,
    pixel_mode,
    width;

  ssize_t
    count,
    y;

  unsigned char
    *tim_data,
    *tim_pixels;

  unsigned short
    word;

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
    Determine if this a TIM file.
  */
  tim_info.id=ReadBlobLSBLong(image);
  do
  {
    /*
      Verify TIM identifier.
    */
    if (tim_info.id != 0x00000010)
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    tim_info.flag=ReadBlobLSBLong(image);
    has_clut=tim_info.flag & (1 << 3) ? 1 : 0;
    pixel_mode=tim_info.flag & 0x07;
    switch ((int) pixel_mode)
    {
      case 0: bits_per_pixel=4; break;
      case 1: bits_per_pixel=8; break;
      case 2: bits_per_pixel=16; break;
      case 3: bits_per_pixel=24; break;
      default: bits_per_pixel=4; break;
    }
    image->depth=8;
    if (has_clut)
      {
        unsigned char
          *tim_colormap;

        /*
          Read TIM raster colormap.
        */
        (void)ReadBlobLSBLong(image);
        (void)ReadBlobLSBShort(image);
        (void)ReadBlobLSBShort(image);
        width=ReadBlobLSBShort(image);
        height=ReadBlobLSBShort(image);
        image->columns=width;
        image->rows=height;
        if (AcquireImageColormap(image,pixel_mode == 1 ? 256UL : 16UL) == MagickFalse)
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        tim_colormap=(unsigned char *) AcquireQuantumMemory(image->colors,
          2UL*sizeof(*tim_colormap));
        if (tim_colormap == (unsigned char *) NULL)
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        count=ReadBlob(image,2*image->colors,tim_colormap);
        if (count != (ssize_t) (2*image->colors))
          ThrowReaderException(CorruptImageError,"InsufficientImageDataInFile");
        p=tim_colormap;
        for (i=0; i < (ssize_t) image->colors; i++)
        {
          word=(*p++);
          word|=(unsigned short) (*p++ << 8);
          image->colormap[i].blue=ScaleCharToQuantum(
            ScaleColor5to8(1UL*(word >> 10) & 0x1f));
          image->colormap[i].green=ScaleCharToQuantum(
            ScaleColor5to8(1UL*(word >> 5) & 0x1f));
          image->colormap[i].red=ScaleCharToQuantum(
            ScaleColor5to8(1UL*word & 0x1f));
        }
        tim_colormap=(unsigned char *) RelinquishMagickMemory(tim_colormap);
      }
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
       Read image data.
     */
    (void) ReadBlobLSBLong(image);
    (void) ReadBlobLSBShort(image);
    (void) ReadBlobLSBShort(image);
    width=ReadBlobLSBShort(image);
    height=ReadBlobLSBShort(image);
    image_size=2*width*height;
    bytes_per_line=width*2;
    width=(width*16)/bits_per_pixel;
    tim_data=(unsigned char *) AcquireQuantumMemory(image_size,
      sizeof(*tim_data));
    if (tim_data == (unsigned char *) NULL)
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    count=ReadBlob(image,image_size,tim_data);
    if (count != (ssize_t) (image_size))
      ThrowReaderException(CorruptImageError,"InsufficientImageDataInFile");
    tim_pixels=tim_data;
    /*
      Initialize image structure.
    */
    image->columns=width;
    image->rows=height;
    /*
      Convert TIM raster image to pixel packets.
    */
    switch (bits_per_pixel)
    {
      case 4:
      {
        /*
          Convert PseudoColor scanline.
        */
        for (y=(ssize_t) image->rows-1; y >= 0; y--)
        {
          q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetAuthenticIndexQueue(image);
          p=tim_pixels+y*bytes_per_line;
          for (x=0; x < ((ssize_t) image->columns-1); x+=2)
          {
            SetPixelIndex(indexes+x,(*p) & 0x0f);
            SetPixelIndex(indexes+x+1,(*p >> 4) & 0x0f);
            p++;
          }
          if ((image->columns % 2) != 0)
            {
              SetPixelIndex(indexes+x,(*p >> 4) & 0x0f);
              p++;
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
        break;
      }
      case 8:
      {
        /*
          Convert PseudoColor scanline.
        */
        for (y=(ssize_t) image->rows-1; y >= 0; y--)
        {
          q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetAuthenticIndexQueue(image);
          p=tim_pixels+y*bytes_per_line;
          for (x=0; x < (ssize_t) image->columns; x++)
            SetPixelIndex(indexes+x,*p++);
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
        break;
      }
      case 16:
      {
        /*
          Convert DirectColor scanline.
        */
        for (y=(ssize_t) image->rows-1; y >= 0; y--)
        {
          p=tim_pixels+y*bytes_per_line;
          q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            word=(*p++);
            word|=(*p++ << 8);
            SetPixelBlue(q,ScaleCharToQuantum(ScaleColor5to8(
              (1UL*word >> 10) & 0x1f)));
            SetPixelGreen(q,ScaleCharToQuantum(ScaleColor5to8(
              (1UL*word >> 5) & 0x1f)));
            SetPixelRed(q,ScaleCharToQuantum(ScaleColor5to8(
              (1UL*word >> 0) & 0x1f)));
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
        break;
      }
      case 24:
      {
        /*
          Convert DirectColor scanline.
        */
        for (y=(ssize_t) image->rows-1; y >= 0; y--)
        {
          p=tim_pixels+y*bytes_per_line;
          q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            SetPixelRed(q,ScaleCharToQuantum(*p++));
            SetPixelGreen(q,ScaleCharToQuantum(*p++));
            SetPixelBlue(q,ScaleCharToQuantum(*p++));
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
        break;
      }
      default:
        ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    }
    if (image->storage_class == PseudoClass)
      (void) SyncImage(image);
    tim_pixels=(unsigned char *) RelinquishMagickMemory(tim_pixels);
    if (EOFBlob(image) != MagickFalse)
      {
        ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
          image->filename);
        break;
      }
    /*
      Proceed to next image.
    */
    tim_info.id=ReadBlobLSBLong(image);
    if (tim_info.id == 0x00000010)
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
  } while (tim_info.id == 0x00000010);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
