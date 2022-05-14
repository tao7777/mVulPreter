static Image *ReadBMPImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  BMPInfo
    bmp_info;

  Image
    *image;

  IndexPacket
    index;

  MagickBooleanType
    status;

  MagickOffsetType
    offset,
    start_position;

  MemoryInfo
    *pixel_info;

  register IndexPacket
    *indexes;

  register PixelPacket
    *q;

  register ssize_t
    i,
    x;

  register unsigned char
    *p;

  size_t
    bit,
    blue,
    bytes_per_line,
    green,
    length,
    red;

  ssize_t
    count,
    y;

  unsigned char
    magick[12],
    *pixels;

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
    Determine if this a BMP file.
  */
  (void) ResetMagickMemory(&bmp_info,0,sizeof(bmp_info));
  bmp_info.ba_offset=0;
  start_position=0;
  count=ReadBlob(image,2,magick);
  do
  {
    LongPixelPacket
      shift;

    PixelPacket
      quantum_bits;

    size_t
      profile_data,
      profile_size;

    /*
      Verify BMP identifier.
    */
    if (bmp_info.ba_offset == 0)
      start_position=TellBlob(image)-2;
    bmp_info.ba_offset=0;
    while (LocaleNCompare((char *) magick,"BA",2) == 0)
    {
      bmp_info.file_size=ReadBlobLSBLong(image);
      bmp_info.ba_offset=ReadBlobLSBLong(image);
      bmp_info.offset_bits=ReadBlobLSBLong(image);
      count=ReadBlob(image,2,magick);
      if (count != 2)
        break;
    }
    if (image->debug != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  Magick: %c%c",
        magick[0],magick[1]);
    if ((count == 0) || ((LocaleNCompare((char *) magick,"BM",2) != 0) &&
        (LocaleNCompare((char *) magick,"CI",2) != 0)))
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    bmp_info.file_size=ReadBlobLSBLong(image);
    (void) ReadBlobLSBLong(image);

    if (image->debug != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
         "  File_size in header:  %u bytes",bmp_info.file_size);

    bmp_info.offset_bits=ReadBlobLSBLong(image);
    bmp_info.size=ReadBlobLSBLong(image);
    if (image->debug != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  BMP size: %u",
        bmp_info.size);
    if (bmp_info.size == 12)
      {
        /*
          OS/2 BMP image file.
        */
        (void) CopyMagickString(image->magick,"BMP2",MaxTextExtent);
        bmp_info.width=(ssize_t) ((short) ReadBlobLSBShort(image));
        bmp_info.height=(ssize_t) ((short) ReadBlobLSBShort(image));
        bmp_info.planes=ReadBlobLSBShort(image);
        bmp_info.bits_per_pixel=ReadBlobLSBShort(image);
        bmp_info.x_pixels=0;
        bmp_info.y_pixels=0;
        bmp_info.number_colors=0;
        bmp_info.compression=BI_RGB;
        bmp_info.image_size=0;
        bmp_info.alpha_mask=0;
        if (image->debug != MagickFalse)
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  Format: OS/2 Bitmap");
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  Geometry: %.20gx%.20g",(double) bmp_info.width,(double)
              bmp_info.height);
          }
      }
    else
      {
        /*
          Microsoft Windows BMP image file.
        */
        if (bmp_info.size < 40)
          ThrowReaderException(CorruptImageError,"NonOS2HeaderSizeError");
        bmp_info.width=(ssize_t) ((int) ReadBlobLSBLong(image));
        bmp_info.height=(ssize_t) ((int) ReadBlobLSBLong(image));
        bmp_info.planes=ReadBlobLSBShort(image);
        bmp_info.bits_per_pixel=ReadBlobLSBShort(image);
        bmp_info.compression=ReadBlobLSBLong(image);
        bmp_info.image_size=ReadBlobLSBLong(image);
        bmp_info.x_pixels=ReadBlobLSBLong(image);
        bmp_info.y_pixels=ReadBlobLSBLong(image);
        bmp_info.number_colors=ReadBlobLSBLong(image);
        bmp_info.colors_important=ReadBlobLSBLong(image);
        profile_data=0;
        profile_size=0;
        if (image->debug != MagickFalse)
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  Format: MS Windows bitmap");
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  Geometry: %.20gx%.20g",(double) bmp_info.width,(double)
              bmp_info.height);
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  Bits per pixel: %.20g",(double) bmp_info.bits_per_pixel);
            switch ((int) bmp_info.compression)
            {
              case BI_RGB:
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "  Compression: BI_RGB");
                break;
              }
              case BI_RLE4:
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "  Compression: BI_RLE4");
                break;
              }
              case BI_RLE8:
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "  Compression: BI_RLE8");
                break;
              }
              case BI_BITFIELDS:
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "  Compression: BI_BITFIELDS");
                break;
              }
              case BI_PNG:
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "  Compression: BI_PNG");
                break;
              }
              case BI_JPEG:
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "  Compression: BI_JPEG");
                break;
              }
              default:
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "  Compression: UNKNOWN (%u)",bmp_info.compression);
              }
            }
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  Number of colors: %u",bmp_info.number_colors);
          }
        bmp_info.red_mask=ReadBlobLSBLong(image);
        bmp_info.green_mask=ReadBlobLSBLong(image);
        bmp_info.blue_mask=ReadBlobLSBLong(image);
        if (bmp_info.size > 40)
          {

            double
              sum;

            /*
              Read color management information.
            */
            bmp_info.alpha_mask=ReadBlobLSBLong(image);
            bmp_info.colorspace=(int) ReadBlobLSBLong(image);
            /*
              Decode 2^30 fixed point formatted CIE primaries.
            */
#           define BMP_DENOM ((double) 0x40000000)
            bmp_info.red_primary.x=(double) ReadBlobLSBLong(image)/BMP_DENOM;
            bmp_info.red_primary.y=(double) ReadBlobLSBLong(image)/BMP_DENOM;
            bmp_info.red_primary.z=(double) ReadBlobLSBLong(image)/BMP_DENOM;
            bmp_info.green_primary.x=(double) ReadBlobLSBLong(image)/BMP_DENOM;
            bmp_info.green_primary.y=(double) ReadBlobLSBLong(image)/BMP_DENOM;
            bmp_info.green_primary.z=(double) ReadBlobLSBLong(image)/BMP_DENOM;
            bmp_info.blue_primary.x=(double) ReadBlobLSBLong(image)/BMP_DENOM;
            bmp_info.blue_primary.y=(double) ReadBlobLSBLong(image)/BMP_DENOM;
            bmp_info.blue_primary.z=(double) ReadBlobLSBLong(image)/BMP_DENOM;

            sum=bmp_info.red_primary.x+bmp_info.red_primary.y+
              bmp_info.red_primary.z;
            bmp_info.red_primary.x/=sum;
            bmp_info.red_primary.y/=sum;
            image->chromaticity.red_primary.x=bmp_info.red_primary.x;
            image->chromaticity.red_primary.y=bmp_info.red_primary.y;

            sum=bmp_info.green_primary.x+bmp_info.green_primary.y+
              bmp_info.green_primary.z;
            bmp_info.green_primary.x/=sum;
            bmp_info.green_primary.y/=sum;
            image->chromaticity.green_primary.x=bmp_info.green_primary.x;
            image->chromaticity.green_primary.y=bmp_info.green_primary.y;

            sum=bmp_info.blue_primary.x+bmp_info.blue_primary.y+
              bmp_info.blue_primary.z;
            bmp_info.blue_primary.x/=sum;
            bmp_info.blue_primary.y/=sum;
            image->chromaticity.blue_primary.x=bmp_info.blue_primary.x;
            image->chromaticity.blue_primary.y=bmp_info.blue_primary.y;

            /*
              Decode 16^16 fixed point formatted gamma_scales.
            */
            bmp_info.gamma_scale.x=(double) ReadBlobLSBLong(image)/0x10000;
            bmp_info.gamma_scale.y=(double) ReadBlobLSBLong(image)/0x10000;
            bmp_info.gamma_scale.z=(double) ReadBlobLSBLong(image)/0x10000;
            /*
              Compute a single gamma from the BMP 3-channel gamma.
            */
            image->gamma=(bmp_info.gamma_scale.x+bmp_info.gamma_scale.y+
              bmp_info.gamma_scale.z)/3.0;
          }
        else
          (void) CopyMagickString(image->magick,"BMP3",MaxTextExtent);

        if (bmp_info.size > 108)
          {
            size_t
              intent;

            /*
              Read BMP Version 5 color management information.
            */
            intent=ReadBlobLSBLong(image);
            switch ((int) intent)
            {
              case LCS_GM_BUSINESS:
              {
                image->rendering_intent=SaturationIntent;
                break;
              }
              case LCS_GM_GRAPHICS:
              {
                image->rendering_intent=RelativeIntent;
                break;
              }
              case LCS_GM_IMAGES:
              {
                image->rendering_intent=PerceptualIntent;
                break;
              }
              case LCS_GM_ABS_COLORIMETRIC:
              {
                image->rendering_intent=AbsoluteIntent;
                break;
              }
            }
            profile_data=ReadBlobLSBLong(image);
            profile_size=ReadBlobLSBLong(image);
            (void) profile_data;
            (void) profile_size;
            (void) ReadBlobLSBLong(image);  /* Reserved byte */
          }
      }
    if ((MagickSizeType) bmp_info.file_size > GetBlobSize(image))
      (void) ThrowMagickException(exception,GetMagickModule(),CorruptImageError,
        "LengthAndFilesizeDoNotMatch","`%s'",image->filename);
    else
      if ((MagickSizeType) bmp_info.file_size < GetBlobSize(image))
        (void) ThrowMagickException(exception,GetMagickModule(),
          CorruptImageWarning,"LengthAndFilesizeDoNotMatch","`%s'",
          image->filename);
    if (bmp_info.width <= 0)
      ThrowReaderException(CorruptImageError,"NegativeOrZeroImageSize");
    if (bmp_info.height == 0)
      ThrowReaderException(CorruptImageError,"NegativeOrZeroImageSize");
    if (bmp_info.planes != 1)
      ThrowReaderException(CorruptImageError,"StaticPlanesValueNotEqualToOne");
    if ((bmp_info.bits_per_pixel != 1) && (bmp_info.bits_per_pixel != 4) &&
        (bmp_info.bits_per_pixel != 8) && (bmp_info.bits_per_pixel != 16) &&
        (bmp_info.bits_per_pixel != 24) && (bmp_info.bits_per_pixel != 32))
      ThrowReaderException(CorruptImageError,"UnrecognizedBitsPerPixel");
    if (bmp_info.bits_per_pixel < 16 &&
        bmp_info.number_colors > (1U << bmp_info.bits_per_pixel))
      {
        ThrowReaderException(CorruptImageError,
            "UnrecognizedNumberOfColors");
      }
    if ((bmp_info.compression == 1) && (bmp_info.bits_per_pixel != 8))
      ThrowReaderException(CorruptImageError,"UnrecognizedBitsPerPixel");
    if ((bmp_info.compression == 2) && (bmp_info.bits_per_pixel != 4))
      ThrowReaderException(CorruptImageError,"UnrecognizedBitsPerPixel");
    if ((bmp_info.compression == 3) && (bmp_info.bits_per_pixel < 16))
      ThrowReaderException(CorruptImageError,"UnrecognizedBitsPerPixel");
    switch (bmp_info.compression)
    {
      case BI_RGB:
      case BI_RLE8:
      case BI_RLE4:
      case BI_BITFIELDS:
        break;
      case BI_JPEG:
        ThrowReaderException(CoderError,"JPEGCompressNotSupported");
      case BI_PNG:
        ThrowReaderException(CoderError,"PNGCompressNotSupported");
      default:
        ThrowReaderException(CorruptImageError,"UnrecognizedImageCompression");
    }
    image->columns=(size_t) MagickAbsoluteValue(bmp_info.width);
    image->rows=(size_t) MagickAbsoluteValue(bmp_info.height);
    image->depth=bmp_info.bits_per_pixel <= 8 ? bmp_info.bits_per_pixel : 8;
    image->matte=((bmp_info.alpha_mask != 0) &&
      (bmp_info.compression == BI_BITFIELDS)) || 
       (bmp_info.bits_per_pixel == 32) ? MagickTrue : MagickFalse;
    if (bmp_info.bits_per_pixel < 16)
      {
        size_t
          one;

        image->storage_class=PseudoClass;
        image->colors=bmp_info.number_colors;
        one=1;
        if (image->colors == 0)
          image->colors=one << bmp_info.bits_per_pixel;
      }
    if (image->storage_class == PseudoClass)
      {
        unsigned char
          *bmp_colormap;

        size_t
          packet_size;

        /*
          Read BMP raster colormap.
        */
        if (image->debug != MagickFalse)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "  Reading colormap of %.20g colors",(double) image->colors);
        if (AcquireImageColormap(image,image->colors) == MagickFalse)
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        bmp_colormap=(unsigned char *) AcquireQuantumMemory((size_t)
          image->colors,4*sizeof(*bmp_colormap));
        if (bmp_colormap == (unsigned char *) NULL)
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        if ((bmp_info.size == 12) || (bmp_info.size == 64))
          packet_size=3;
        else
          packet_size=4;
        offset=SeekBlob(image,start_position+14+bmp_info.size,SEEK_SET);
        if (offset < 0)
          ThrowReaderException(CorruptImageError,"ImproperImageHeader");
        count=ReadBlob(image,packet_size*image->colors,bmp_colormap);
        if (count != (ssize_t) (packet_size*image->colors))
          ThrowReaderException(CorruptImageError,"InsufficientImageDataInFile");
        p=bmp_colormap;
        for (i=0; i < (ssize_t) image->colors; i++)
        {
          image->colormap[i].blue=ScaleCharToQuantum(*p++);
          image->colormap[i].green=ScaleCharToQuantum(*p++);
          image->colormap[i].red=ScaleCharToQuantum(*p++);
          if (packet_size == 4)
            p++;
        }
        bmp_colormap=(unsigned char *) RelinquishMagickMemory(bmp_colormap);
      }
    image->x_resolution=(double) bmp_info.x_pixels/100.0;
    image->y_resolution=(double) bmp_info.y_pixels/100.0;
    image->units=PixelsPerCentimeterResolution;
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
    offset=SeekBlob(image,start_position+bmp_info.offset_bits,SEEK_SET);
    if (offset < 0)
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    if (bmp_info.compression == BI_RLE4)
      bmp_info.bits_per_pixel<<=1;
    bytes_per_line=4*((image->columns*bmp_info.bits_per_pixel+31)/32);
    length=(size_t) bytes_per_line*image->rows;
    pixel_info=AcquireVirtualMemory((size_t) image->rows,
      MagickMax(bytes_per_line,image->columns+256UL)*sizeof(*pixels));
    if (pixel_info == (MemoryInfo *) NULL)
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
    if ((bmp_info.compression == BI_RGB) ||
        (bmp_info.compression == BI_BITFIELDS))
      {
        if (image->debug != MagickFalse)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "  Reading pixels (%.20g bytes)",(double) length);
        count=ReadBlob(image,length,pixels);
        if (count != (ssize_t) length)
          {
            pixel_info=RelinquishVirtualMemory(pixel_info);
            ThrowReaderException(CorruptImageError,
              "InsufficientImageDataInFile");
          }
      }
    else
      {
        /*
          Convert run-length encoded raster pixels.
        */
        status=DecodeImage(image,bmp_info.compression,pixels);
        if (status == MagickFalse)
          {
            pixel_info=RelinquishVirtualMemory(pixel_info);
            ThrowReaderException(CorruptImageError,
              "UnableToRunlengthDecodeImage");
          }
      }
    /*
      Convert BMP raster image to pixel packets.
    */
    if (bmp_info.compression == BI_RGB)
      {
        bmp_info.alpha_mask=image->matte != MagickFalse ? 0xff000000U : 0U;
        bmp_info.red_mask=0x00ff0000U;
        bmp_info.green_mask=0x0000ff00U;
        bmp_info.blue_mask=0x000000ffU;
        if (bmp_info.bits_per_pixel == 16)
          {
            /*
              RGB555.
            */
            bmp_info.red_mask=0x00007c00U;
            bmp_info.green_mask=0x000003e0U;
            bmp_info.blue_mask=0x0000001fU;
          }
      }
    if ((bmp_info.bits_per_pixel == 16) || (bmp_info.bits_per_pixel == 32))
      {
        register size_t
          sample;

        /*
          Get shift and quantum bits info from bitfield masks.
        */
        (void) ResetMagickMemory(&shift,0,sizeof(shift));
        (void) ResetMagickMemory(&quantum_bits,0,sizeof(quantum_bits));
        if (bmp_info.red_mask != 0)
          while (((bmp_info.red_mask << shift.red) & 0x80000000UL) == 0)
            shift.red++;
        if (bmp_info.green_mask != 0)
          while (((bmp_info.green_mask << shift.green) & 0x80000000UL) == 0)
            shift.green++;
        if (bmp_info.blue_mask != 0)
          while (((bmp_info.blue_mask << shift.blue) & 0x80000000UL) == 0)
            shift.blue++;
        if (bmp_info.alpha_mask != 0)
          while (((bmp_info.alpha_mask << shift.opacity) & 0x80000000UL) == 0)
            shift.opacity++;
        sample=shift.red;
        while (((bmp_info.red_mask << sample) & 0x80000000UL) != 0)
          sample++;
        quantum_bits.red=ClampToQuantum((MagickRealType) sample-shift.red);
        sample=shift.green;
        while (((bmp_info.green_mask << sample) & 0x80000000UL) != 0)
          sample++;
        quantum_bits.green=ClampToQuantum((MagickRealType) sample-shift.green);
        sample=shift.blue;
        while (((bmp_info.blue_mask << sample) & 0x80000000UL) != 0)
          sample++;
        quantum_bits.blue=ClampToQuantum((MagickRealType) sample-shift.blue);
        sample=shift.opacity;
        while (((bmp_info.alpha_mask << sample) & 0x80000000UL) != 0)
          sample++;
        quantum_bits.opacity=ClampToQuantum((MagickRealType) sample-
          shift.opacity);
      }
    switch (bmp_info.bits_per_pixel)
    {
      case 1:
      {
        /*
          Convert bitmap scanline.
        */
        for (y=(ssize_t) image->rows-1; y >= 0; y--)
        {
          p=pixels+(image->rows-y-1)*bytes_per_line;
          q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetAuthenticIndexQueue(image);
          for (x=0; x < ((ssize_t) image->columns-7); x+=8)
          {
            for (bit=0; bit < 8; bit++)
            {
              index=(IndexPacket) (((*p) & (0x80 >> bit)) != 0 ? 0x01 : 0x00);
              SetPixelIndex(indexes+x+bit,index);
              q++;
            }
            p++;
          }
          if ((image->columns % 8) != 0)
            {
              for (bit=0; bit < (image->columns % 8); bit++)
              {
                index=(IndexPacket) (((*p) & (0x80 >> bit)) != 0 ? 0x01 : 0x00);
                SetPixelIndex(indexes+x+bit,index);
              }
              p++;
            }
          if (SyncAuthenticPixels(image,exception) == MagickFalse)
            break;
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,LoadImageTag,(MagickOffsetType)
                (image->rows-y),image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        (void) SyncImage(image);
        break;
      }
      case 4:
      {
        /*
          Convert PseudoColor scanline.
        */
        for (y=(ssize_t) image->rows-1; y >= 0; y--)
        {
          p=pixels+(image->rows-y-1)*bytes_per_line;
          q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetAuthenticIndexQueue(image);
          for (x=0; x < ((ssize_t) image->columns-1); x+=2)
          {
            index=ConstrainColormapIndex(image,(*p >> 4) & 0x0f);
            SetPixelIndex(indexes+x,index);
            index=ConstrainColormapIndex(image,*p & 0x0f);
            SetPixelIndex(indexes+x+1,index);
            p++;
          }
          if ((image->columns % 2) != 0)
            {
              index=ConstrainColormapIndex(image,(*p >> 4) & 0xf);
              SetPixelIndex(indexes+x,index);
              p++;
            }
          if (SyncAuthenticPixels(image,exception) == MagickFalse)
            break;
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,LoadImageTag,(MagickOffsetType)
                (image->rows-y),image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        (void) SyncImage(image);
        break;
      }
      case 8:
      {
        /*
          Convert PseudoColor scanline.
        */
        if ((bmp_info.compression == BI_RLE8) ||
            (bmp_info.compression == BI_RLE4))
          bytes_per_line=image->columns;
        for (y=(ssize_t) image->rows-1; y >= 0; y--)
        {
          p=pixels+(image->rows-y-1)*bytes_per_line;
          q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetAuthenticIndexQueue(image);
          for (x=(ssize_t) image->columns; x != 0; --x)
          {
            index=ConstrainColormapIndex(image,*p);
            SetPixelIndex(indexes,index);
            indexes++;
            p++;
            q++;
          }
          if (SyncAuthenticPixels(image,exception) == MagickFalse)
            break;
          offset=(MagickOffsetType) (image->rows-y-1);
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,LoadImageTag,(MagickOffsetType)
                (image->rows-y),image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        (void) SyncImage(image);
        break;
      }
      case 16:
      {
        size_t
          alpha,
          pixel;

        /*
          Convert bitfield encoded 16-bit PseudoColor scanline.
        */
        if (bmp_info.compression != BI_RGB &&
            bmp_info.compression != BI_BITFIELDS)
          {
            pixel_info=RelinquishVirtualMemory(pixel_info);
            ThrowReaderException(CorruptImageError,
              "UnrecognizedImageCompression");
          }
        bytes_per_line=2*(image->columns+image->columns % 2);
        image->storage_class=DirectClass;
        for (y=(ssize_t) image->rows-1; y >= 0; y--)
        {
          p=pixels+(image->rows-y-1)*bytes_per_line;
          q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            pixel=(size_t) (*p++);
            pixel|=(*p++) << 8;
            red=((pixel & bmp_info.red_mask) << shift.red) >> 16;
            if (quantum_bits.red == 5)
              red|=((red & 0xe000) >> 5);
            if (quantum_bits.red <= 8)
              red|=((red & 0xff00) >> 8);
            green=((pixel & bmp_info.green_mask) << shift.green) >> 16;
            if (quantum_bits.green == 5)
              green|=((green & 0xe000) >> 5);
            if (quantum_bits.green == 6)
              green|=((green & 0xc000) >> 6);
            if (quantum_bits.green <= 8)
              green|=((green & 0xff00) >> 8);
            blue=((pixel & bmp_info.blue_mask) << shift.blue) >> 16;
            if (quantum_bits.blue == 5)
              blue|=((blue & 0xe000) >> 5);
            if (quantum_bits.blue <= 8)
              blue|=((blue & 0xff00) >> 8);
            alpha=((pixel & bmp_info.alpha_mask) << shift.opacity) >> 16;
            if (quantum_bits.opacity <= 8)
              alpha|=((alpha & 0xff00) >> 8);
            SetPixelRed(q,ScaleShortToQuantum((unsigned short) red));
            SetPixelGreen(q,ScaleShortToQuantum((unsigned short) green));
            SetPixelBlue(q,ScaleShortToQuantum((unsigned short) blue));
            SetPixelOpacity(q,OpaqueOpacity);
            if (image->matte != MagickFalse)
              SetPixelAlpha(q,ScaleShortToQuantum((unsigned short) alpha));
            q++;
          }
          if (SyncAuthenticPixels(image,exception) == MagickFalse)
            break;
          offset=(MagickOffsetType) (image->rows-y-1);
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,LoadImageTag,(MagickOffsetType)
                (image->rows-y),image->rows);
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
        bytes_per_line=4*((image->columns*24+31)/32);
        for (y=(ssize_t) image->rows-1; y >= 0; y--)
        {
          p=pixels+(image->rows-y-1)*bytes_per_line;
          q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            SetPixelBlue(q,ScaleCharToQuantum(*p++));
            SetPixelGreen(q,ScaleCharToQuantum(*p++));
            SetPixelRed(q,ScaleCharToQuantum(*p++));
            SetPixelOpacity(q,OpaqueOpacity);
            q++;
          }
          if (SyncAuthenticPixels(image,exception) == MagickFalse)
            break;
          offset=(MagickOffsetType) (image->rows-y-1);
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,LoadImageTag,(MagickOffsetType)
                (image->rows-y),image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        break;
      }
      case 32:
      {
        /*
          Convert bitfield encoded DirectColor scanline.
        */
        if ((bmp_info.compression != BI_RGB) &&
            (bmp_info.compression != BI_BITFIELDS))
          {
            pixel_info=RelinquishVirtualMemory(pixel_info);
            ThrowReaderException(CorruptImageError,
              "UnrecognizedImageCompression");
          }
        bytes_per_line=4*(image->columns);
        for (y=(ssize_t) image->rows-1; y >= 0; y--)
        {
          size_t
            alpha,
            pixel;

          p=pixels+(image->rows-y-1)*bytes_per_line;
          q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            pixel=(size_t) (*p++);
            pixel|=((size_t) *p++ << 8);
            pixel|=((size_t) *p++ << 16);
            pixel|=((size_t) *p++ << 24);
            red=((pixel & bmp_info.red_mask) << shift.red) >> 16;
            if (quantum_bits.red == 8)
              red|=(red >> 8);
            green=((pixel & bmp_info.green_mask) << shift.green) >> 16;
            if (quantum_bits.green == 8)
              green|=(green >> 8);
            blue=((pixel & bmp_info.blue_mask) << shift.blue) >> 16;
            if (quantum_bits.blue == 8)
              blue|=(blue >> 8);
            alpha=((pixel & bmp_info.alpha_mask) << shift.opacity) >> 16;
            if (quantum_bits.opacity == 8)
              alpha|=(alpha >> 8);
            SetPixelRed(q,ScaleShortToQuantum((unsigned short) red));
            SetPixelGreen(q,ScaleShortToQuantum((unsigned short) green));
            SetPixelBlue(q,ScaleShortToQuantum((unsigned short) blue));
            SetPixelAlpha(q,OpaqueOpacity);
            if (image->matte != MagickFalse)
              SetPixelAlpha(q,ScaleShortToQuantum((unsigned short) alpha));
            q++;
          }
          if (SyncAuthenticPixels(image,exception) == MagickFalse)
            break;
          offset=(MagickOffsetType) (image->rows-y-1);
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,LoadImageTag,(MagickOffsetType)
                (image->rows-y),image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        break;
      }
      default:
      {
        pixel_info=RelinquishVirtualMemory(pixel_info);
        ThrowReaderException(CorruptImageError,"ImproperImageHeader");
      }
    }
    pixel_info=RelinquishVirtualMemory(pixel_info);
    if (EOFBlob(image) != MagickFalse)
      {
        ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
          image->filename);
        break;
      }
    if (bmp_info.height < 0)
      {
        Image
          *flipped_image;

        /*
          Correct image orientation.
        */
        flipped_image=FlipImage(image,exception);
        if (flipped_image != (Image *) NULL)
          {
            DuplicateBlob(flipped_image,image);
            image=DestroyImage(image);
            image=flipped_image;
          }
      }
    /*
      Proceed to next image.
    */
    if (image_info->number_scenes != 0)
      if (image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    *magick='\0';
    if (bmp_info.ba_offset != 0)
      {
        offset=SeekBlob(image,(MagickOffsetType) bmp_info.ba_offset,SEEK_SET);
        if (offset < 0)
          ThrowReaderException(CorruptImageError,"ImproperImageHeader");
      }
    count=ReadBlob(image,2,magick);
    if ((count == 2) && (IsBMP(magick,2) != MagickFalse))
      {
        /*
          Acquire next image structure.
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
  } while (IsBMP(magick,2) != MagickFalse);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
