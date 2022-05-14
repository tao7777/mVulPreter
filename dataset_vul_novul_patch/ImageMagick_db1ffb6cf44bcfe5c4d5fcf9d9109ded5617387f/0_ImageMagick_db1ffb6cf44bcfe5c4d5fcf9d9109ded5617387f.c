static Image *ReadPALMImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  IndexPacket
    index;

  MagickBooleanType
    status;

  MagickOffsetType
    totalOffset,
    seekNextDepth;

  MagickPixelPacket
    transpix;

  register IndexPacket
    *indexes;

  register ssize_t
    i,
    x;

  register PixelPacket
    *q;

  size_t
    bytes_per_row,
    flags,
    bits_per_pixel,
    version,
    nextDepthOffset,
    transparentIndex,
    compressionType,
    byte,
    mask,
    redbits,
    greenbits,
    bluebits,
    one,
    pad,
    size,
    bit;

  ssize_t
    count,
     y;
 
   unsigned char
    *last_row,
     *one_row,
     *ptr;
 
  unsigned short
    color16;

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
      (void) DestroyImageList(image);
      return((Image *) NULL);
    }
  totalOffset=0;
  do
  {
    image->columns=ReadBlobMSBShort(image);
    image->rows=ReadBlobMSBShort(image);
    if (EOFBlob(image) != MagickFalse)
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    if ((image->columns == 0) || (image->rows == 0))
      ThrowReaderException(CorruptImageError,"NegativeOrZeroImageSize");
    status=SetImageExtent(image,image->columns,image->rows);
    if (status == MagickFalse)
      {
        InheritException(exception,&image->exception);
        return(DestroyImageList(image));
      }
    bytes_per_row=ReadBlobMSBShort(image);
    flags=ReadBlobMSBShort(image);
    bits_per_pixel=(size_t) ReadBlobByte(image);
    if ((bits_per_pixel != 1) && (bits_per_pixel != 2) &&
        (bits_per_pixel != 4) && (bits_per_pixel != 8) &&
        (bits_per_pixel != 16))
      ThrowReaderException(CorruptImageError,"UnrecognizedBitsPerPixel");
    version=(size_t) ReadBlobByte(image);
    if ((version != 0) && (version != 1) && (version != 2))
      ThrowReaderException(CorruptImageError,"FileFormatVersionMismatch");
    nextDepthOffset=(size_t) ReadBlobMSBShort(image);
    transparentIndex=(size_t) ReadBlobByte(image);
    compressionType=(size_t) ReadBlobByte(image);
    if ((compressionType != PALM_COMPRESSION_NONE) &&
        (compressionType != PALM_COMPRESSION_SCANLINE ) &&
        (compressionType != PALM_COMPRESSION_RLE))
      ThrowReaderException(CorruptImageError,"UnrecognizedImageCompression");
    pad=ReadBlobMSBShort(image);
    (void) pad;
    /*
      Initialize image colormap.
    */
    one=1;
    if ((bits_per_pixel < 16) &&
        (AcquireImageColormap(image,one << bits_per_pixel) == MagickFalse))
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    GetMagickPixelPacket(image,&transpix);
    if (bits_per_pixel == 16)  /* Direct Color */
      {
        redbits=(size_t) ReadBlobByte(image);  /* # of bits of red */
        (void) redbits;
        greenbits=(size_t) ReadBlobByte(image);  /* # of bits of green */
        (void) greenbits;
        bluebits=(size_t) ReadBlobByte(image);  /* # of bits of blue */
        (void) bluebits;
        ReadBlobByte(image);  /* reserved by Palm */
        ReadBlobByte(image);  /* reserved by Palm */
        transpix.red=(MagickRealType) (QuantumRange*ReadBlobByte(image)/31);
        transpix.green=(MagickRealType) (QuantumRange*ReadBlobByte(image)/63);
        transpix.blue=(MagickRealType) (QuantumRange*ReadBlobByte(image)/31);
      }
    if (bits_per_pixel == 8)
      {
        IndexPacket
          index;

        if (flags & PALM_HAS_COLORMAP_FLAG)
          {
            count=(ssize_t) ReadBlobMSBShort(image);
            for (i=0; i < (ssize_t) count; i++)
            {
              ReadBlobByte(image);
              index=ConstrainColormapIndex(image,(size_t) (255-i));
              image->colormap[(int) index].red=ScaleCharToQuantum(
                (unsigned char) ReadBlobByte(image));
              image->colormap[(int) index].green=ScaleCharToQuantum(
                (unsigned char) ReadBlobByte(image));
              image->colormap[(int) index].blue=ScaleCharToQuantum(
                (unsigned char) ReadBlobByte(image));
          }
        }
      else
        for (i=0; i < (ssize_t) (1L << bits_per_pixel); i++)
        {
          index=ConstrainColormapIndex(image,(size_t) (255-i));
          image->colormap[(int) index].red=ScaleCharToQuantum(
            PalmPalette[i][0]);
          image->colormap[(int) index].green=ScaleCharToQuantum(
            PalmPalette[i][1]);
          image->colormap[(int) index].blue=ScaleCharToQuantum(
            PalmPalette[i][2]);
        }
      }
    if (flags & PALM_IS_COMPRESSED_FLAG)
      size=ReadBlobMSBShort(image);
    (void) size;
    image->storage_class=DirectClass;
    if (bits_per_pixel < 16)
      {
        image->storage_class=PseudoClass;
        image->depth=8;
      }
    if (image_info->ping != MagickFalse) 
      {
        (void) CloseBlob(image);
        return(image);
      }
    status=SetImageExtent(image,image->columns,image->rows);
    if (status == MagickFalse)
      {
        InheritException(exception,&image->exception);
        return(DestroyImageList(image));
      }
    one_row=(unsigned char *) AcquireQuantumMemory(MagickMax(bytes_per_row,
       2*image->columns),sizeof(*one_row));
     if (one_row == (unsigned char *) NULL)
       ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    last_row=(unsigned char *) NULL;
     if (compressionType == PALM_COMPRESSION_SCANLINE)
       {
        last_row=(unsigned char *) AcquireQuantumMemory(MagickMax(bytes_per_row,
          2*image->columns),sizeof(*last_row));
        if (last_row == (unsigned char *) NULL)
           ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
       }
     mask=(size_t) (1U << bits_per_pixel)-1;
    for (y=0; y < (ssize_t) image->rows; y++)
    {
      if ((flags & PALM_IS_COMPRESSED_FLAG) == 0)
        {
          /* TODO move out of loop! */
          image->compression=NoCompression;
          count=ReadBlob(image,bytes_per_row,one_row);
          if (count != (ssize_t) bytes_per_row)
            break;
        }
      else
        {
          if (compressionType == PALM_COMPRESSION_RLE)
            {
              /* TODO move out of loop! */
              image->compression=RLECompression;
              for (i=0; i < (ssize_t) bytes_per_row; )
              {
                count=(ssize_t) ReadBlobByte(image);
                if (count < 0)
                  break;
                count=MagickMin(count,(ssize_t) bytes_per_row-i);
                byte=(size_t) ReadBlobByte(image);
                (void) ResetMagickMemory(one_row+i,(int) byte,(size_t) count);
                i+=count;
              }
          }
        else
          if (compressionType == PALM_COMPRESSION_SCANLINE)
            {
              size_t
                one;

              /* TODO move out of loop! */
              one=1;
              image->compression=FaxCompression;
              for (i=0; i < (ssize_t) bytes_per_row; i+=8)
              {
                count=(ssize_t) ReadBlobByte(image);
                if (count < 0)
                  break;
                byte=(size_t) MagickMin((ssize_t) bytes_per_row-i,8);
                for (bit=0; bit < byte; bit++)
                {
                   if ((y == 0) || (count & (one << (7 - bit))))
                     one_row[i+bit]=(unsigned char) ReadBlobByte(image);
                   else
                    one_row[i+bit]=last_row[i+bit];
                 }
               }
              (void) CopyMagickMemory(last_row, one_row, bytes_per_row);
             }
         }
       ptr=one_row;
      q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
      if (q == (PixelPacket *) NULL)
        break;
      indexes=GetAuthenticIndexQueue(image);
      if (bits_per_pixel == 16)
        {
          if (image->columns > (2*bytes_per_row))
             {
               one_row=(unsigned char *) RelinquishMagickMemory(one_row);
               if (compressionType == PALM_COMPRESSION_SCANLINE)
                last_row=(unsigned char *) RelinquishMagickMemory(last_row);
               ThrowReaderException(CorruptImageError,"CorruptImage");
             }
           for (x=0; x < (ssize_t) image->columns; x++)
          {
            color16=(*ptr++ << 8);
            color16|=(*ptr++);
            SetPixelRed(q,(QuantumRange*((color16 >> 11) & 0x1f))/0x1f);
            SetPixelGreen(q,(QuantumRange*((color16 >> 5) & 0x3f))/0x3f);
            SetPixelBlue(q,(QuantumRange*((color16 >> 0) & 0x1f))/0x1f);
            SetPixelOpacity(q,OpaqueOpacity);
            q++;
          }
        }
      else
        {
          bit=8-bits_per_pixel;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            if ((size_t) (ptr-one_row) >= bytes_per_row)
               {
                 one_row=(unsigned char *) RelinquishMagickMemory(one_row);
                 if (compressionType == PALM_COMPRESSION_SCANLINE)
                  last_row=(unsigned char *) RelinquishMagickMemory(last_row);
                 ThrowReaderException(CorruptImageError,"CorruptImage");
               }
             index=(IndexPacket) (mask-(((*ptr) & (mask << bit)) >> bit));
            SetPixelIndex(indexes+x,index);
            SetPixelRGBO(q,image->colormap+(ssize_t) index);
            if (bit)
              bit-=bits_per_pixel;
            else
              {
                ptr++;
                bit=8-bits_per_pixel;
              }
            q++;
          }
          if (SyncAuthenticPixels(image,exception) == MagickFalse)
            break;
        }
        if (image->previous == (Image *) NULL)
          {
            status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
              image->rows);
            if (status == MagickFalse)
              break;
          }
      }
    if (flags & PALM_HAS_TRANSPARENCY_FLAG)
      {
        IndexPacket index=ConstrainColormapIndex(image,(mask-transparentIndex));
        if (bits_per_pixel != 16)
          SetMagickPixelPacket(image,image->colormap+(ssize_t) index,
            (const IndexPacket *) NULL,&transpix);
        (void) TransparentPaintImage(image,&transpix,(Quantum)
          TransparentOpacity,MagickFalse);
       }
     one_row=(unsigned char *) RelinquishMagickMemory(one_row);
     if (compressionType == PALM_COMPRESSION_SCANLINE)
      last_row=(unsigned char *) RelinquishMagickMemory(last_row);
     if (EOFBlob(image) != MagickFalse)
       {
         ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
          image->filename);
        break;
      }
    /*
      Proceed to next image. Copied from coders/pnm.c
    */
    if (image_info->number_scenes != 0)
      if (image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    if (nextDepthOffset != 0)
      {
        /*
          Skip to next image.
        */
        totalOffset+=(MagickOffsetType) (nextDepthOffset*4);
        if (totalOffset >= (MagickOffsetType) GetBlobSize(image))
          ThrowReaderException(CorruptImageError,"ImproperImageHeader")
        else
          seekNextDepth=SeekBlob(image,totalOffset,SEEK_SET);
        if (seekNextDepth != totalOffset)
          ThrowReaderException(CorruptImageError,"ImproperImageHeader");
        /*
          Allocate next image structure. Copied from coders/pnm.c
        */
        AcquireNextImage(image_info,image);
        if (GetNextImageInList(image) == (Image *) NULL)
          {
            (void) DestroyImageList(image);
            return((Image *) NULL);
          }
        image=SyncNextImageInList(image);
        status=SetImageProgress(image,LoadImagesTag,TellBlob(image),
          GetBlobSize(image));
        if (status == MagickFalse)
          break;
      }
  } while (nextDepthOffset != 0);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
