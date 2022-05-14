static MagickBooleanType WritePALMImage(const ImageInfo *image_info,
  Image *image)
{
  ExceptionInfo
    *exception;

  MagickBooleanType
    status;

  MagickOffsetType
    currentOffset,
    offset,
    scene;

  MagickSizeType
    cc;

  PixelPacket
    transpix;

  QuantizeInfo
    *quantize_info;

  register IndexPacket
    *indexes;

  register ssize_t
    x;

  register PixelPacket
    *p;

  ssize_t
    y;

  size_t
    count,
    bits_per_pixel,
    bytes_per_row,
    nextDepthOffset,
    one;

  unsigned char
     bit,
     byte,
     color,
    *last_row,
     *one_row,
     *ptr,
     version;

  unsigned int
    transparentIndex;

  unsigned short
    color16,
    flags;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  exception=AcquireExceptionInfo();
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  quantize_info=AcquireQuantizeInfo(image_info);
  flags=0;
  currentOffset=0;
  transparentIndex=0;
  transpix.red=0;
  transpix.green=0;
  transpix.blue=0;
  transpix.opacity=0;
  one=1;
  version=0;
  scene=0;
  do
  {
    (void) TransformImageColorspace(image,sRGBColorspace);
    count=GetNumberColors(image,NULL,exception);
    for (bits_per_pixel=1;  (one << bits_per_pixel) < count; bits_per_pixel*=2) ;
    if (bits_per_pixel > 16)
      bits_per_pixel=16;
    else
      if (bits_per_pixel < 16)
        (void) TransformImageColorspace(image,image->colorspace);
    if (bits_per_pixel < 8)
      {
        (void) TransformImageColorspace(image,GRAYColorspace);
        (void) SetImageType(image,PaletteType);
        (void) SortColormapByIntensity(image);
      }
    if ((image->storage_class == PseudoClass) && (image->colors > 256))
      (void) SetImageStorageClass(image,DirectClass);
    if (image->storage_class == PseudoClass)
      flags|=PALM_HAS_COLORMAP_FLAG;
    else
      flags|=PALM_IS_DIRECT_COLOR;
    (void) WriteBlobMSBShort(image,(unsigned short) image->columns); /* width */
    (void) WriteBlobMSBShort(image,(unsigned short) image->rows);  /* height */
    bytes_per_row=((image->columns+(16/bits_per_pixel-1))/(16/
      bits_per_pixel))*2;
    (void) WriteBlobMSBShort(image,(unsigned short) bytes_per_row);
    if ((image_info->compression == RLECompression) ||
        (image_info->compression == FaxCompression))
      flags|=PALM_IS_COMPRESSED_FLAG;
    (void) WriteBlobMSBShort(image, flags);
    (void) WriteBlobByte(image,(unsigned char) bits_per_pixel);
    if (bits_per_pixel > 1)
      version=1;
    if ((image_info->compression == RLECompression) ||
        (image_info->compression == FaxCompression))
      version=2;
    (void) WriteBlobByte(image,version);
    (void) WriteBlobMSBShort(image,0);  /* nextDepthOffset */
    (void) WriteBlobByte(image,(unsigned char) transparentIndex);
    if (image_info->compression == RLECompression)
      (void) WriteBlobByte(image,PALM_COMPRESSION_RLE);
    else
      if (image_info->compression == FaxCompression)
        (void) WriteBlobByte(image,PALM_COMPRESSION_SCANLINE);
      else
        (void) WriteBlobByte(image,PALM_COMPRESSION_NONE);
    (void) WriteBlobMSBShort(image,0);  /* reserved */
    offset=16;
    if (bits_per_pixel == 16)
      {
        (void) WriteBlobByte(image,5);  /* # of bits of red */
        (void) WriteBlobByte(image,6);  /* # of bits of green */
        (void) WriteBlobByte(image,5);  /* # of bits of blue */
        (void) WriteBlobByte(image,0);  /* reserved by Palm */
        (void) WriteBlobMSBLong(image,0);  /* no transparent color, YET */
        offset+=8;
      }
    if (bits_per_pixel == 8)
      {
        if (flags & PALM_HAS_COLORMAP_FLAG)  /* Write out colormap */
          {
            quantize_info->dither=IsPaletteImage(image,&image->exception);
            quantize_info->number_colors=image->colors;
            (void) QuantizeImage(quantize_info,image);
            (void) WriteBlobMSBShort(image,(unsigned short) image->colors);
            for (count = 0; count < image->colors; count++)
            {
              (void) WriteBlobByte(image,(unsigned char) count);
              (void) WriteBlobByte(image,ScaleQuantumToChar(
                image->colormap[count].red));
              (void) WriteBlobByte(image,
                ScaleQuantumToChar(image->colormap[count].green));
              (void) WriteBlobByte(image,
                ScaleQuantumToChar(image->colormap[count].blue));
            }
            offset+=2+count*4;
          }
      else  /* Map colors to Palm standard colormap */
        {
          Image
            *affinity_image;

          affinity_image=ConstituteImage(256,1,"RGB",CharPixel,&PalmPalette,
            exception);
          (void) TransformImageColorspace(affinity_image,
            affinity_image->colorspace);
          (void) RemapImage(quantize_info,image,affinity_image);
          for (y=0; y < (ssize_t) image->rows; y++)
          {
            p=GetAuthenticPixels(image,0,y,image->columns,1,exception);
            indexes=GetAuthenticIndexQueue(image);
            for (x=0; x < (ssize_t) image->columns; x++)
              SetPixelIndex(indexes+x,FindColor(&image->colormap[
                (ssize_t) GetPixelIndex(indexes+x)]));
          }
          affinity_image=DestroyImage(affinity_image);
        }
       }
     if (flags & PALM_IS_COMPRESSED_FLAG)
       (void) WriteBlobMSBShort(image,0);  /* fill in size later */
    last_row=(unsigned char *) NULL;
     if (image_info->compression == FaxCompression)
      {
        last_row=(unsigned char *) AcquireQuantumMemory(bytes_per_row,
          sizeof(*last_row));
        if (last_row == (unsigned char *) NULL)
          {
            quantize_info=DestroyQuantizeInfo(quantize_info);
            ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
          }
      }
     one_row=(unsigned char *) AcquireQuantumMemory(bytes_per_row,
       sizeof(*one_row));
     if (one_row == (unsigned char *) NULL)
      {
        quantize_info=DestroyQuantizeInfo(quantize_info);
        ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
      }
     for (y=0; y < (ssize_t) image->rows; y++)
     {
       ptr=one_row;
      (void) ResetMagickMemory(ptr,0,bytes_per_row);
      p=GetAuthenticPixels(image,0,y,image->columns,1,exception);
      if (p == (PixelPacket *) NULL)
        break;
      indexes=GetAuthenticIndexQueue(image);
      if (bits_per_pixel == 16)
        {
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            color16=(unsigned short) ((((31*(size_t) GetPixelRed(p))/
              (size_t) QuantumRange) << 11) |
              (((63*(size_t) GetPixelGreen(p))/(size_t) QuantumRange) << 5) |
              ((31*(size_t) GetPixelBlue(p))/(size_t) QuantumRange));
            if (GetPixelOpacity(p) == (Quantum) TransparentOpacity)
              {
                transpix.red=GetPixelRed(p);
                transpix.green=GetPixelGreen(p);
                transpix.blue=GetPixelBlue(p);
                transpix.opacity=GetPixelOpacity(p);
                flags|=PALM_HAS_TRANSPARENCY_FLAG;
              }
            *ptr++=(unsigned char) ((color16 >> 8) & 0xff);
            *ptr++=(unsigned char) (color16 & 0xff);
            p++;
          }
        }
      else
        {
          byte=0x00;
          bit=(unsigned char) (8-bits_per_pixel);
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            if (bits_per_pixel >= 8)
              color=(unsigned char) GetPixelIndex(indexes+x);
            else
              color=(unsigned char) (GetPixelIndex(indexes+x)*
                ((one << bits_per_pixel)-1)/MagickMax(1*image->colors-1,1));
            byte|=color << bit;
            if (bit != 0)
              bit-=(unsigned char) bits_per_pixel;
            else
              {
                *ptr++=byte;
                byte=0x00;
                bit=(unsigned char) (8-bits_per_pixel);
              }
          }
          if ((image->columns % (8/bits_per_pixel)) != 0)
            *ptr++=byte;
        }
      if (image_info->compression == RLECompression)
        {
          x=0;
          while (x < (ssize_t) bytes_per_row)
          {
            byte=one_row[x];
            count=1;
            while ((one_row[++x] == byte) && (count < 255) &&
                   (x < (ssize_t) bytes_per_row))
              count++;
            (void) WriteBlobByte(image,(unsigned char) count);
            (void) WriteBlobByte(image,(unsigned char) byte);
          }
        }
      else
        if (image_info->compression == FaxCompression)
          {
            char
              tmpbuf[8],
              *tptr;

            for (x = 0;  x < (ssize_t) bytes_per_row;  x += 8)
            {
               tptr = tmpbuf;
               for (bit=0, byte=0; bit < (unsigned char) MagickMin(8,(ssize_t) bytes_per_row-x); bit++)
               {
                if ((y == 0) || (last_row[x + bit] != one_row[x + bit]))
                   {
                     byte |= (1 << (7 - bit));
                     *tptr++ = (char) one_row[x + bit];
                  }
              }
               (void) WriteBlobByte(image, byte);
               (void) WriteBlob(image,tptr-tmpbuf,(unsigned char *) tmpbuf);
             }
            (void) CopyMagickMemory(last_row,one_row,bytes_per_row);
           }
         else
           (void) WriteBlob(image,bytes_per_row,one_row);
      }
    if (flags & PALM_HAS_TRANSPARENCY_FLAG)
      {
        offset=SeekBlob(image,currentOffset+6,SEEK_SET);
        (void) WriteBlobMSBShort(image,flags);
        offset=SeekBlob(image,currentOffset+12,SEEK_SET);
        (void) WriteBlobByte(image,(unsigned char) transparentIndex);  /* trans index */
      }
    if (bits_per_pixel == 16)
       {
         offset=SeekBlob(image,currentOffset+20,SEEK_SET);
         (void) WriteBlobByte(image,0);  /* reserved by Palm */
        (void) WriteBlobByte(image,(unsigned char) ((31*transpix.red)/
          QuantumRange));
        (void) WriteBlobByte(image,(unsigned char) ((63*transpix.green)/
          QuantumRange));
        (void) WriteBlobByte(image,(unsigned char) ((31*transpix.blue)/
          QuantumRange));
       }
     if (flags & PALM_IS_COMPRESSED_FLAG)  /* fill in size now */
       {
        offset=SeekBlob(image,currentOffset+offset,SEEK_SET);
        (void) WriteBlobMSBShort(image,(unsigned short) (GetBlobSize(image)-
          currentOffset-offset));
       }
     if (one_row != (unsigned char *) NULL)
       one_row=(unsigned char *) RelinquishMagickMemory(one_row);
    if (last_row != (unsigned char *) NULL)
      last_row=(unsigned char *) RelinquishMagickMemory(last_row);
     if (GetNextImageInList(image) == (Image *) NULL)
       break;
     /* padding to 4 byte word */
    for (cc=(GetBlobSize(image)) % 4; cc > 0; cc--)
      (void) WriteBlobByte(image,0);
    /* write nextDepthOffset and return to end of image */
    (void) SeekBlob(image,currentOffset+10,SEEK_SET);
    nextDepthOffset=(size_t) ((GetBlobSize(image)-currentOffset)/4);
    (void) WriteBlobMSBShort(image,(unsigned short) nextDepthOffset);
    currentOffset=(MagickOffsetType) GetBlobSize(image);
    (void) SeekBlob(image,currentOffset,SEEK_SET);
    image=SyncNextImageInList(image);
    status=SetImageProgress(image,SaveImagesTag,scene++,
      GetImageListLength(image));
    if (status == MagickFalse)
      break;
  } while (image_info->adjoin != MagickFalse);
  quantize_info=DestroyQuantizeInfo(quantize_info);
  (void) CloseBlob(image);
  (void) DestroyExceptionInfo(exception);
  return(MagickTrue);
}
