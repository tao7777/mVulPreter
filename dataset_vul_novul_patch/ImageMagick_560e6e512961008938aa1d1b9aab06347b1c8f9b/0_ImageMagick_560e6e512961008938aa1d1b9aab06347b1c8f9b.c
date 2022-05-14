static MagickBooleanType WritePCXImage(const ImageInfo *image_info,Image *image)
{
  MagickBooleanType
    status;

  MagickOffsetType
    offset,
    *page_table,
    scene;

  MemoryInfo
    *pixel_info;

  PCXInfo
    pcx_info;

  register const IndexPacket
    *indexes;

  register const PixelPacket
    *p;

  register ssize_t
    i,
    x;

  register unsigned char
    *q;

  size_t
    length;

  ssize_t
    y;

  unsigned char
    *pcx_colormap,
    *pixels;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFalse)
    return(status);
  (void) TransformImageColorspace(image,sRGBColorspace);
  page_table=(MagickOffsetType *) NULL;
  if ((LocaleCompare(image_info->magick,"DCX") == 0) ||
      ((GetNextImageInList(image) != (Image *) NULL) &&
       (image_info->adjoin != MagickFalse)))
    {
      /*
        Write the DCX page table.
      */
      (void) WriteBlobLSBLong(image,0x3ADE68B1L);
      page_table=(MagickOffsetType *) AcquireQuantumMemory(1024UL,
        sizeof(*page_table));
      if (page_table == (MagickOffsetType *) NULL)
        ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
      for (scene=0; scene < 1024; scene++)
        (void) WriteBlobLSBLong(image,0x00000000L);
    }
  scene=0;
  do
  {
    if (page_table != (MagickOffsetType *) NULL)
      page_table[scene]=TellBlob(image);
    /*
      Initialize PCX raster file header.
    */
    pcx_info.identifier=0x0a;
    pcx_info.version=5;
    pcx_info.encoding=image_info->compression == NoCompression ? 0 : 1;
    pcx_info.bits_per_pixel=8;
    if ((image->storage_class == PseudoClass) &&
        (SetImageMonochrome(image,&image->exception) != MagickFalse))
      pcx_info.bits_per_pixel=1;
    pcx_info.left=0;
    pcx_info.top=0;
    pcx_info.right=(unsigned short) (image->columns-1);
    pcx_info.bottom=(unsigned short) (image->rows-1);
    switch (image->units)
    {
      case UndefinedResolution:
      case PixelsPerInchResolution:
      default:
      {
        pcx_info.horizontal_resolution=(unsigned short) image->x_resolution;
        pcx_info.vertical_resolution=(unsigned short) image->y_resolution;
        break;
      }
      case PixelsPerCentimeterResolution:
      {
        pcx_info.horizontal_resolution=(unsigned short)
          (2.54*image->x_resolution+0.5);
        pcx_info.vertical_resolution=(unsigned short)
          (2.54*image->y_resolution+0.5);
        break;
      }
    }
    pcx_info.reserved=0;
    pcx_info.planes=1;
    if ((image->storage_class == DirectClass) || (image->colors > 256))
      {
        pcx_info.planes=3;
        if (image->matte != MagickFalse)
          pcx_info.planes++;
      }
    pcx_info.bytes_per_line=(unsigned short) (((size_t) image->columns*
      pcx_info.bits_per_pixel+7)/8);
    pcx_info.palette_info=1;
    pcx_info.colormap_signature=0x0c;
    /*
      Write PCX header.
    */
    (void) WriteBlobByte(image,pcx_info.identifier);
    (void) WriteBlobByte(image,pcx_info.version);
    (void) WriteBlobByte(image,pcx_info.encoding);
    (void) WriteBlobByte(image,pcx_info.bits_per_pixel);
    (void) WriteBlobLSBShort(image,pcx_info.left);
    (void) WriteBlobLSBShort(image,pcx_info.top);
    (void) WriteBlobLSBShort(image,pcx_info.right);
    (void) WriteBlobLSBShort(image,pcx_info.bottom);
    (void) WriteBlobLSBShort(image,pcx_info.horizontal_resolution);
    (void) WriteBlobLSBShort(image,pcx_info.vertical_resolution);
    /*
      Dump colormap to file.
    */
    pcx_colormap=(unsigned char *) AcquireQuantumMemory(256UL,
      3*sizeof(*pcx_colormap));
    if (pcx_colormap == (unsigned char *) NULL)
      ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
    (void) memset(pcx_colormap,0,3*256*sizeof(*pcx_colormap));
    q=pcx_colormap;
    if ((image->storage_class == PseudoClass) && (image->colors <= 256))
      for (i=0; i < (ssize_t) image->colors; i++)
      {
        *q++=ScaleQuantumToChar(image->colormap[i].red);
        *q++=ScaleQuantumToChar(image->colormap[i].green);
        *q++=ScaleQuantumToChar(image->colormap[i].blue);
      }
    (void) WriteBlob(image,3*16,(const unsigned char *) pcx_colormap);
    (void) WriteBlobByte(image,pcx_info.reserved);
    (void) WriteBlobByte(image,pcx_info.planes);
    (void) WriteBlobLSBShort(image,pcx_info.bytes_per_line);
    (void) WriteBlobLSBShort(image,pcx_info.palette_info);
    for (i=0; i < 58; i++)
      (void) WriteBlobByte(image,'\0');
     length=(size_t) pcx_info.bytes_per_line;
     pixel_info=AcquireVirtualMemory(length,pcx_info.planes*sizeof(*pixels));
     if (pixel_info == (MemoryInfo *) NULL)
      {
        pcx_colormap=(unsigned char *) RelinquishMagickMemory(pcx_colormap);
        ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
      }
     pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
     q=pixels;
     if ((image->storage_class == DirectClass) || (image->colors > 256))
      {
        /*
          Convert DirectClass image to PCX raster pixels.
        */
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          q=pixels;
          for (i=0; i < pcx_info.planes; i++)
          {
            p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            switch ((int) i)
            {
              case 0:
              {
                for (x=0; x < (ssize_t) pcx_info.bytes_per_line; x++)
                {
                  *q++=ScaleQuantumToChar(GetPixelRed(p));
                  p++;
                }
                break;
              }
              case 1:
              {
                for (x=0; x < (ssize_t) pcx_info.bytes_per_line; x++)
                {
                  *q++=ScaleQuantumToChar(GetPixelGreen(p));
                  p++;
                }
                break;
              }
              case 2:
              {
                for (x=0; x < (ssize_t) pcx_info.bytes_per_line; x++)
                {
                  *q++=ScaleQuantumToChar(GetPixelBlue(p));
                  p++;
                }
                break;
              }
              case 3:
              default:
              {
                for (x=(ssize_t) pcx_info.bytes_per_line; x != 0; x--)
                {
                  *q++=ScaleQuantumToChar((Quantum)
                    (GetPixelAlpha(p)));
                  p++;
                }
                break;
              }
            }
          }
          if (PCXWritePixels(&pcx_info,pixels,image) == MagickFalse)
            break;
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
        }
      }
    else
      {
        if (pcx_info.bits_per_pixel > 1)
          for (y=0; y < (ssize_t) image->rows; y++)
          {
            p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            indexes=GetVirtualIndexQueue(image);
            q=pixels;
            for (x=0; x < (ssize_t) image->columns; x++)
              *q++=(unsigned char) GetPixelIndex(indexes+x);
            if (PCXWritePixels(&pcx_info,pixels,image) == MagickFalse)
              break;
            if (image->previous == (Image *) NULL)
              {
                status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                image->rows);
                if (status == MagickFalse)
                  break;
              }
          }
        else
          {
            register unsigned char
              bit,
              byte;

            /*
              Convert PseudoClass image to a PCX monochrome image.
            */
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
              if (p == (const PixelPacket *) NULL)
                break;
              indexes=GetVirtualIndexQueue(image);
              bit=0;
              byte=0;
              q=pixels;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                byte<<=1;
                if (GetPixelLuma(image,p) >= (QuantumRange/2.0))
                  byte|=0x01;
                bit++;
                if (bit == 8)
                  {
                    *q++=byte;
                    bit=0;
                    byte=0;
                  }
                p++;
              }
              if (bit != 0)
                *q++=byte << (8-bit);
              if (PCXWritePixels(&pcx_info,pixels,image) == MagickFalse)
                break;
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,SaveImageTag,(MagickOffsetType)
                    y,image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
          }
        (void) WriteBlobByte(image,pcx_info.colormap_signature);
        (void) WriteBlob(image,3*256,pcx_colormap);
      }
    pixel_info=RelinquishVirtualMemory(pixel_info);
    pcx_colormap=(unsigned char *) RelinquishMagickMemory(pcx_colormap);
    if (page_table == (MagickOffsetType *) NULL)
      break;
    if (scene >= 1023)
      break;
    if (GetNextImageInList(image) == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=SetImageProgress(image,SaveImagesTag,scene++,
      GetImageListLength(image));
    if (status == MagickFalse)
      break;
  } while (image_info->adjoin != MagickFalse);
  if (page_table != (MagickOffsetType *) NULL)
    {
      /*
        Write the DCX page table.
      */
      page_table[scene+1]=0;
      offset=SeekBlob(image,0L,SEEK_SET);
      if (offset < 0)
        ThrowWriterException(CorruptImageError,"ImproperImageHeader");
      (void) WriteBlobLSBLong(image,0x3ADE68B1L);
      for (i=0; i <= (ssize_t) scene; i++)
        (void) WriteBlobLSBLong(image,(unsigned int) page_table[i]);
      page_table=(MagickOffsetType *) RelinquishMagickMemory(page_table);
    }
  if (status == MagickFalse)
    {
      char
        *message;

      message=GetExceptionMessage(errno);
      (void) ThrowMagickException(&image->exception,GetMagickModule(),
        FileOpenError,"UnableToWriteFile","`%s': %s",image->filename,message);
      message=DestroyString(message);
    }
  (void) CloseBlob(image);
  return(MagickTrue);
}
