static Image *ReadPCXImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define ThrowPCXException(severity,tag) \
  { \
    scanline=(unsigned char *) RelinquishMagickMemory(scanline); \
    pixel_info=RelinquishVirtualMemory(pixel_info); \
    ThrowReaderException(severity,tag); \
  }

  Image
    *image;

  int
    bits,
    id,
    mask;

  MagickBooleanType
    status;

  MagickOffsetType
    offset,
    *page_table;

  MemoryInfo
    *pixel_info;

  PCXInfo
    pcx_info;

  register IndexPacket
    *indexes;

  register ssize_t
    x;

  register PixelPacket
    *q;

  register ssize_t
    i;

  register unsigned char
    *p,
    *r;

  size_t
    one,
    pcx_packets;

  ssize_t
    count,
    y;

  unsigned char
    packet,
    pcx_colormap[768],
    *pixels,
    *scanline;

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
    Determine if this a PCX file.
  */
  page_table=(MagickOffsetType *) NULL;
  if (LocaleCompare(image_info->magick,"DCX") == 0)
    {
      size_t
        magic;

      /*
        Read the DCX page table.
      */
      magic=ReadBlobLSBLong(image);
      if (magic != 987654321)
        ThrowReaderException(CorruptImageError,"ImproperImageHeader");
      page_table=(MagickOffsetType *) AcquireQuantumMemory(1024UL,
        sizeof(*page_table));
      if (page_table == (MagickOffsetType *) NULL)
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
      for (id=0; id < 1024; id++)
      {
        page_table[id]=(MagickOffsetType) ReadBlobLSBLong(image);
        if (page_table[id] == 0)
          break;
      }
    }
  if (page_table != (MagickOffsetType *) NULL)
    {
      offset=SeekBlob(image,(MagickOffsetType) page_table[0],SEEK_SET);
      if (offset < 0)
        ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    }
  count=ReadBlob(image,1,&pcx_info.identifier);
  for (id=1; id < 1024; id++)
  {
    int
      bits_per_pixel;

    /*
      Verify PCX identifier.
    */
    pcx_info.version=(unsigned char) ReadBlobByte(image);
    if ((count == 0) || (pcx_info.identifier != 0x0a))
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    pcx_info.encoding=(unsigned char) ReadBlobByte(image);
    bits_per_pixel=ReadBlobByte(image);
    if (bits_per_pixel == -1)
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    pcx_info.bits_per_pixel=(unsigned char) bits_per_pixel;
    pcx_info.left=ReadBlobLSBShort(image);
    pcx_info.top=ReadBlobLSBShort(image);
    pcx_info.right=ReadBlobLSBShort(image);
    pcx_info.bottom=ReadBlobLSBShort(image);
    pcx_info.horizontal_resolution=ReadBlobLSBShort(image);
    pcx_info.vertical_resolution=ReadBlobLSBShort(image);
    /*
      Read PCX raster colormap.
    */
    image->columns=(size_t) MagickAbsoluteValue((ssize_t) pcx_info.right-
      pcx_info.left)+1UL;
    image->rows=(size_t) MagickAbsoluteValue((ssize_t) pcx_info.bottom-
      pcx_info.top)+1UL;
    if ((image->columns == 0) || (image->rows == 0) ||
        (pcx_info.bits_per_pixel == 0))
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    image->depth=pcx_info.bits_per_pixel <= 8 ? 8U : MAGICKCORE_QUANTUM_DEPTH;
    image->units=PixelsPerInchResolution;
    image->x_resolution=(double) pcx_info.horizontal_resolution;
    image->y_resolution=(double) pcx_info.vertical_resolution;
    image->colors=16;
    count=ReadBlob(image,3*image->colors,pcx_colormap);
    pcx_info.reserved=(unsigned char) ReadBlobByte(image);
    pcx_info.planes=(unsigned char) ReadBlobByte(image);
    if ((pcx_info.bits_per_pixel*pcx_info.planes) >= 64)
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    one=1;
    if ((pcx_info.bits_per_pixel != 8) || (pcx_info.planes == 1))
      if ((pcx_info.version == 3) || (pcx_info.version == 5) ||
          ((pcx_info.bits_per_pixel*pcx_info.planes) == 1))
        image->colors=(size_t) MagickMin(one << (1UL*
          (pcx_info.bits_per_pixel*pcx_info.planes)),256UL);
    if (AcquireImageColormap(image,image->colors) == MagickFalse)
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    if ((pcx_info.bits_per_pixel >= 8) && (pcx_info.planes != 1))
      image->storage_class=DirectClass;
    p=pcx_colormap;
    for (i=0; i < (ssize_t) image->colors; i++)
    {
      image->colormap[i].red=ScaleCharToQuantum(*p++);
      image->colormap[i].green=ScaleCharToQuantum(*p++);
      image->colormap[i].blue=ScaleCharToQuantum(*p++);
    }
    pcx_info.bytes_per_line=ReadBlobLSBShort(image);
    pcx_info.palette_info=ReadBlobLSBShort(image);
    for (i=0; i < 58; i++)
      (void) ReadBlobByte(image);
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
    pcx_packets=(size_t) image->rows*pcx_info.bytes_per_line*pcx_info.planes;
    if ((size_t) (pcx_info.bits_per_pixel*pcx_info.planes*image->columns) >
        (pcx_packets*8U))
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    scanline=(unsigned char *) AcquireQuantumMemory(MagickMax(image->columns,
      pcx_info.bytes_per_line),MagickMax(8,pcx_info.planes)*sizeof(*scanline));
    pixel_info=AcquireVirtualMemory(pcx_packets,2*sizeof(*pixels));
    if ((scanline == (unsigned char *) NULL) ||
        (pixel_info == (MemoryInfo *) NULL))
      {
        if (scanline != (unsigned char *) NULL)
          scanline=(unsigned char *) RelinquishMagickMemory(scanline);
        if (pixel_info != (MemoryInfo *) NULL)
          pixel_info=RelinquishVirtualMemory(pixel_info);
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
      }
    pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
    /*
      Uncompress image data.
    */
    p=pixels;
    if (pcx_info.encoding == 0)
      while (pcx_packets != 0)
      {
        packet=(unsigned char) ReadBlobByte(image);
        if (EOFBlob(image) != MagickFalse)
          ThrowPCXException(CorruptImageError,"UnexpectedEndOfFile");
        *p++=packet;
        pcx_packets--;
      }
    else
      while (pcx_packets != 0)
      {
        packet=(unsigned char) ReadBlobByte(image);
        if (EOFBlob(image) != MagickFalse)
          ThrowPCXException(CorruptImageError,"UnexpectedEndOfFile");
        if ((packet & 0xc0) != 0xc0)
          {
            *p++=packet;
            pcx_packets--;
            continue;
          }
        count=(ssize_t) (packet & 0x3f);
        packet=(unsigned char) ReadBlobByte(image);
        if (EOFBlob(image) != MagickFalse)
          ThrowPCXException(CorruptImageError,"UnexpectedEndOfFile");
        for ( ; count != 0; count--)
        {
          *p++=packet;
          pcx_packets--;
          if (pcx_packets == 0)
            break;
        }
      }
    if (image->storage_class == DirectClass)
      image->matte=pcx_info.planes > 3 ? MagickTrue : MagickFalse;
    else
      if ((pcx_info.version == 5) ||
          ((pcx_info.bits_per_pixel*pcx_info.planes) == 1))
        {
          /*
            Initialize image colormap.
          */
          if (image->colors > 256)
            ThrowPCXException(CorruptImageError,"ColormapExceeds256Colors");
          if ((pcx_info.bits_per_pixel*pcx_info.planes) == 1)
            {
              /*
                Monochrome colormap.
              */
              image->colormap[0].red=(Quantum) 0;
              image->colormap[0].green=(Quantum) 0;
              image->colormap[0].blue=(Quantum) 0;
              image->colormap[1].red=QuantumRange;
              image->colormap[1].green=QuantumRange;
              image->colormap[1].blue=QuantumRange;
            }
          else
            if (image->colors > 16)
              {
                /*
                  256 color images have their color map at the end of the file.
                */
                pcx_info.colormap_signature=(unsigned char) ReadBlobByte(image);
                count=ReadBlob(image,3*image->colors,pcx_colormap);
                p=pcx_colormap;
                for (i=0; i < (ssize_t) image->colors; i++)
                {
                  image->colormap[i].red=ScaleCharToQuantum(*p++);
                  image->colormap[i].green=ScaleCharToQuantum(*p++);
                  image->colormap[i].blue=ScaleCharToQuantum(*p++);
                }
            }
        }
    /*
      Convert PCX raster image to pixel packets.
    */
    for (y=0; y < (ssize_t) image->rows; y++)
    {
      p=pixels+(y*pcx_info.bytes_per_line*pcx_info.planes);
      q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
      if (q == (PixelPacket *) NULL)
        break;
      indexes=GetAuthenticIndexQueue(image);
      r=scanline;
      if (image->storage_class == DirectClass)
        for (i=0; i < pcx_info.planes; i++)
        {
          r=scanline+i;
          for (x=0; x < (ssize_t) pcx_info.bytes_per_line; x++)
          {
            switch (i)
            {
              case 0:
              {
                *r=(*p++);
                break;
              }
              case 1:
              {
                *r=(*p++);
                break;
              }
              case 2:
              {
                *r=(*p++);
                break;
              }
              case 3:
              default:
              {
                *r=(*p++);
                break;
              }
            }
            r+=pcx_info.planes;
          }
        }
      else
        if (pcx_info.planes > 1)
          {
            for (x=0; x < (ssize_t) image->columns; x++)
              *r++=0;
            for (i=0; i < pcx_info.planes; i++)
            {
              r=scanline;
              for (x=0; x < (ssize_t) pcx_info.bytes_per_line; x++)
              {
                 bits=(*p++);
                 for (mask=0x80; mask != 0; mask>>=1)
                 {
                   if (bits & mask)
                     *r|=1 << i;
                   r++;
                 }
               }
            }
          }
        else
          switch (pcx_info.bits_per_pixel)
          {
            case 1:
            {
              register ssize_t
                bit;

              for (x=0; x < ((ssize_t) image->columns-7); x+=8)
              {
                for (bit=7; bit >= 0; bit--)
                  *r++=(unsigned char) ((*p) & (0x01 << bit) ? 0x01 : 0x00);
                p++;
              }
              if ((image->columns % 8) != 0)
                {
                  for (bit=7; bit >= (ssize_t) (8-(image->columns % 8)); bit--)
                    *r++=(unsigned char) ((*p) & (0x01 << bit) ? 0x01 : 0x00);
                  p++;
                }
              break;
            }
            case 2:
            {
              for (x=0; x < ((ssize_t) image->columns-3); x+=4)
              {
                *r++=(*p >> 6) & 0x3;
                *r++=(*p >> 4) & 0x3;
                *r++=(*p >> 2) & 0x3;
                *r++=(*p) & 0x3;
                p++;
              }
              if ((image->columns % 4) != 0)
                {
                  for (i=3; i >= (ssize_t) (4-(image->columns % 4)); i--)
                    *r++=(unsigned char) ((*p >> (i*2)) & 0x03);
                  p++;
                }
              break;
            }
            case 4:
            {
              for (x=0; x < ((ssize_t) image->columns-1); x+=2)
              {
                *r++=(*p >> 4) & 0xf;
                *r++=(*p) & 0xf;
                p++;
              }
              if ((image->columns % 2) != 0)
                *r++=(*p++ >> 4) & 0xf;
              break;
            }
            case 8:
            {
              (void) CopyMagickMemory(r,p,image->columns);
              break;
            }
            default:
              break;
          }
      /*
        Transfer image scanline.
      */
      r=scanline;
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        if (image->storage_class == PseudoClass)
          SetPixelIndex(indexes+x,*r++)
        else
          {
            SetPixelRed(q,ScaleCharToQuantum(*r++));
            SetPixelGreen(q,ScaleCharToQuantum(*r++));
            SetPixelBlue(q,ScaleCharToQuantum(*r++));
            if (image->matte != MagickFalse)
              SetPixelAlpha(q,ScaleCharToQuantum(*r++));
          }
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
    if (image->storage_class == PseudoClass)
      (void) SyncImage(image);
    scanline=(unsigned char *) RelinquishMagickMemory(scanline);
    pixel_info=RelinquishVirtualMemory(pixel_info);
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
    if (page_table == (MagickOffsetType *) NULL)
      break;
    if (page_table[id] == 0)
      break;
    offset=SeekBlob(image,(MagickOffsetType) page_table[id],SEEK_SET);
    if (offset < 0)
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    count=ReadBlob(image,1,&pcx_info.identifier);
    if ((count != 0) && (pcx_info.identifier == 0x0a))
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
  if (page_table != (MagickOffsetType *) NULL)
    page_table=(MagickOffsetType *) RelinquishMagickMemory(page_table);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
