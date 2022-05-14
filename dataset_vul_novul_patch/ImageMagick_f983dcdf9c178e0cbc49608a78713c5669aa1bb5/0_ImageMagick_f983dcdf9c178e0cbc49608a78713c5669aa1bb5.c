static MagickBooleanType WriteTIFFImage(const ImageInfo *image_info,
  Image *image)
{
#if !defined(TIFFDefaultStripSize)
#define TIFFDefaultStripSize(tiff,request)  (8192UL/TIFFScanlineSize(tiff))
#endif

  const char
    *mode,
    *option;

  CompressionType
    compression;

  EndianType
    endian_type;

  MagickBooleanType
    debug,
    status;

  MagickOffsetType
    scene;

  QuantumInfo
    *quantum_info;

  QuantumType
    quantum_type;

  register ssize_t
    i;

  ssize_t
    y;

  TIFF
    *tiff;

  TIFFInfo
    tiff_info;

  uint16
    bits_per_sample,
    compress_tag,
    endian,
    photometric;

  uint32
    rows_per_strip;

  unsigned char
    *pixels;

  /*
    Open TIFF file.
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
  (void) SetMagickThreadValue(tiff_exception,&image->exception);
  endian_type=UndefinedEndian;
  option=GetImageOption(image_info,"tiff:endian");
  if (option != (const char *) NULL)
    {
      if (LocaleNCompare(option,"msb",3) == 0)
        endian_type=MSBEndian;
      if (LocaleNCompare(option,"lsb",3) == 0)
        endian_type=LSBEndian;;
    }
  switch (endian_type)
  {
    case LSBEndian: mode="wl"; break;
    case MSBEndian: mode="wb"; break;
    default: mode="w"; break;
  }
#if defined(TIFF_VERSION_BIG)
  if (LocaleCompare(image_info->magick,"TIFF64") == 0)
    switch (endian_type)
    {
      case LSBEndian: mode="wl8"; break;
      case MSBEndian: mode="wb8"; break;
      default: mode="w8"; break;
    }
#endif
  tiff=TIFFClientOpen(image->filename,mode,(thandle_t) image,TIFFReadBlob,
    TIFFWriteBlob,TIFFSeekBlob,TIFFCloseBlob,TIFFGetBlobSize,TIFFMapBlob,
    TIFFUnmapBlob);
  if (tiff == (TIFF *) NULL)
    return(MagickFalse);
  scene=0;
  debug=IsEventLogging();
  (void) debug;
  do
  {
    /*
      Initialize TIFF fields.
    */
    if ((image_info->type != UndefinedType) &&
        (image_info->type != OptimizeType))
      (void) SetImageType(image,image_info->type);
    compression=UndefinedCompression;
    if (image->compression != JPEGCompression)
      compression=image->compression;
    if (image_info->compression != UndefinedCompression)
      compression=image_info->compression;
    switch (compression)
    {
      case FaxCompression:
      case Group4Compression:
      {
        (void) SetImageType(image,BilevelType);
        (void) SetImageDepth(image,1);
        break;
      }
      case JPEGCompression:
      {
        (void) SetImageStorageClass(image,DirectClass);
        (void) SetImageDepth(image,8);
        break;
      }
      default:
        break;
    }
    quantum_info=AcquireQuantumInfo(image_info,image);
    if (quantum_info == (QuantumInfo *) NULL)
      ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
    if ((image->storage_class != PseudoClass) && (image->depth >= 32) &&
        (quantum_info->format == UndefinedQuantumFormat) &&
        (IsHighDynamicRangeImage(image,&image->exception) != MagickFalse))
      {
        status=SetQuantumFormat(image,quantum_info,FloatingPointQuantumFormat);
        if (status == MagickFalse)
          ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
      }
    if ((LocaleCompare(image_info->magick,"PTIF") == 0) &&
        (GetPreviousImageInList(image) != (Image *) NULL))
      (void) TIFFSetField(tiff,TIFFTAG_SUBFILETYPE,FILETYPE_REDUCEDIMAGE);
    if ((image->columns != (uint32) image->columns) ||
        (image->rows != (uint32) image->rows))
      ThrowWriterException(ImageError,"WidthOrHeightExceedsLimit");
    (void) TIFFSetField(tiff,TIFFTAG_IMAGELENGTH,(uint32) image->rows);
    (void) TIFFSetField(tiff,TIFFTAG_IMAGEWIDTH,(uint32) image->columns);
    switch (compression)
    {
      case FaxCompression:
      {
        compress_tag=COMPRESSION_CCITTFAX3;
        SetQuantumMinIsWhite(quantum_info,MagickTrue);
        break;
      }
      case Group4Compression:
      {
        compress_tag=COMPRESSION_CCITTFAX4;
        SetQuantumMinIsWhite(quantum_info,MagickTrue);
        break;
      }
#if defined(COMPRESSION_JBIG)
      case JBIG1Compression:
      {
        compress_tag=COMPRESSION_JBIG;
        break;
      }
#endif
      case JPEGCompression:
      {
        compress_tag=COMPRESSION_JPEG;
        break;
      }
#if defined(COMPRESSION_LZMA)
      case LZMACompression:
      {
        compress_tag=COMPRESSION_LZMA;
        break;
      }
#endif
      case LZWCompression:
      {
        compress_tag=COMPRESSION_LZW;
        break;
      }
      case RLECompression:
      {
        compress_tag=COMPRESSION_PACKBITS;
        break;
      }
      case ZipCompression:
      {
        compress_tag=COMPRESSION_ADOBE_DEFLATE;
        break;
      }
      case NoCompression:
      default:
      {
        compress_tag=COMPRESSION_NONE;
        break;
      }
    }
#if defined(MAGICKCORE_HAVE_TIFFISCODECCONFIGURED) || (TIFFLIB_VERSION > 20040919)
    if ((compress_tag != COMPRESSION_NONE) &&
        (TIFFIsCODECConfigured(compress_tag) == 0))
      {
        (void) ThrowMagickException(&image->exception,GetMagickModule(),
          CoderError,"CompressionNotSupported","`%s'",CommandOptionToMnemonic(
          MagickCompressOptions,(ssize_t) compression));
        compress_tag=COMPRESSION_NONE;
      }
#else
      switch (compress_tag)
      {
#if defined(CCITT_SUPPORT)
        case COMPRESSION_CCITTFAX3:
        case COMPRESSION_CCITTFAX4:
#endif
#if defined(YCBCR_SUPPORT) && defined(JPEG_SUPPORT)
        case COMPRESSION_JPEG:
#endif
#if defined(LZMA_SUPPORT) && defined(COMPRESSION_LZMA)
        case COMPRESSION_LZMA:
#endif
#if defined(LZW_SUPPORT)
        case COMPRESSION_LZW:
#endif
#if defined(PACKBITS_SUPPORT)
        case COMPRESSION_PACKBITS:
#endif
#if defined(ZIP_SUPPORT)
        case COMPRESSION_ADOBE_DEFLATE:
#endif
        case COMPRESSION_NONE:
          break;
        default:
        {
          (void) ThrowMagickException(&image->exception,GetMagickModule(),
            CoderError,"CompressionNotSupported","`%s'",CommandOptionToMnemonic(
              MagickCompressOptions,(ssize_t) compression));
          compress_tag=COMPRESSION_NONE;
          break;
        }
      }
#endif
    if (image->colorspace == CMYKColorspace)
      {
        photometric=PHOTOMETRIC_SEPARATED;
        (void) TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,4);
        (void) TIFFSetField(tiff,TIFFTAG_INKSET,INKSET_CMYK);
      }
    else
      {
        /*
          Full color TIFF raster.
        */
        if (image->colorspace == LabColorspace)
          {
            photometric=PHOTOMETRIC_CIELAB;
            EncodeLabImage(image,&image->exception);
          }
        else
          if (image->colorspace == YCbCrColorspace)
            {
              photometric=PHOTOMETRIC_YCBCR;
              (void) TIFFSetField(tiff,TIFFTAG_YCBCRSUBSAMPLING,1,1);
              (void) SetImageStorageClass(image,DirectClass);
              (void) SetImageDepth(image,8);
            }
          else
            photometric=PHOTOMETRIC_RGB;
        (void) TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,3);
        if ((image_info->type != TrueColorType) &&
            (image_info->type != TrueColorMatteType))
          {
            if ((image_info->type != PaletteType) &&
                (SetImageGray(image,&image->exception) != MagickFalse))
              {
                photometric=(uint16) (quantum_info->min_is_white !=
                  MagickFalse ? PHOTOMETRIC_MINISWHITE :
                  PHOTOMETRIC_MINISBLACK);
                (void) TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,1);
                if ((image->depth == 1) && (image->matte == MagickFalse))
                  SetImageMonochrome(image,&image->exception);
              }
            else
              if (image->storage_class == PseudoClass)
                {
                  size_t
                    depth;

                  /*
                    Colormapped TIFF raster.
                  */
                  (void) TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,1);
                  photometric=PHOTOMETRIC_PALETTE;
                  depth=1;
                  while ((GetQuantumRange(depth)+1) < image->colors)
                    depth<<=1;
                  status=SetQuantumDepth(image,quantum_info,depth);
                  if (status == MagickFalse)
                    ThrowWriterException(ResourceLimitError,
                      "MemoryAllocationFailed");
                }
          }
      }
    (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_FILLORDER,&endian);
    if ((compress_tag == COMPRESSION_CCITTFAX3) &&
        (photometric != PHOTOMETRIC_MINISWHITE))
      {
        compress_tag=COMPRESSION_NONE;
        endian=FILLORDER_MSB2LSB;
      }
    else
      if ((compress_tag == COMPRESSION_CCITTFAX4) &&
         (photometric != PHOTOMETRIC_MINISWHITE))
       {
         compress_tag=COMPRESSION_NONE;
         endian=FILLORDER_MSB2LSB;
       }
    option=GetImageOption(image_info,"tiff:fill-order");
    if (option != (const char *) NULL)
      {
        if (LocaleNCompare(option,"msb",3) == 0)
          endian=FILLORDER_MSB2LSB;
        if (LocaleNCompare(option,"lsb",3) == 0)
          endian=FILLORDER_LSB2MSB;
      }
    (void) TIFFSetField(tiff,TIFFTAG_COMPRESSION,compress_tag);
    (void) TIFFSetField(tiff,TIFFTAG_FILLORDER,endian);
    (void) TIFFSetField(tiff,TIFFTAG_BITSPERSAMPLE,quantum_info->depth);
    if (image->matte != MagickFalse)
      {
        uint16
          extra_samples,
          sample_info[1],
          samples_per_pixel;

        /*
          TIFF has a matte channel.
        */
        extra_samples=1;
        sample_info[0]=EXTRASAMPLE_UNASSALPHA;
        option=GetImageOption(image_info,"tiff:alpha");
        if (option != (const char *) NULL)
          {
            if (LocaleCompare(option,"associated") == 0)
              sample_info[0]=EXTRASAMPLE_ASSOCALPHA;
            else
              if (LocaleCompare(option,"unspecified") == 0)
                sample_info[0]=EXTRASAMPLE_UNSPECIFIED;
          }
        (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_SAMPLESPERPIXEL,
          &samples_per_pixel);
        (void) TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,samples_per_pixel+1);
        (void) TIFFSetField(tiff,TIFFTAG_EXTRASAMPLES,extra_samples,
          &sample_info);
        if (sample_info[0] == EXTRASAMPLE_ASSOCALPHA)
          SetQuantumAlphaType(quantum_info,AssociatedQuantumAlpha);
      }
    (void) TIFFSetField(tiff,TIFFTAG_PHOTOMETRIC,photometric);
    switch (quantum_info->format)
    {
      case FloatingPointQuantumFormat:
      {
        (void) TIFFSetField(tiff,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_IEEEFP);
        (void) TIFFSetField(tiff,TIFFTAG_SMINSAMPLEVALUE,quantum_info->minimum);
        (void) TIFFSetField(tiff,TIFFTAG_SMAXSAMPLEVALUE,quantum_info->maximum);
        break;
      }
      case SignedQuantumFormat:
      {
        (void) TIFFSetField(tiff,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_INT);
        break;
      }
      case UnsignedQuantumFormat:
      {
        (void) TIFFSetField(tiff,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_UINT);
        break;
      }
      default:
        break;
    }
    (void) TIFFSetField(tiff,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);
    (void) TIFFSetField(tiff,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
    if (photometric == PHOTOMETRIC_RGB)
       if ((image_info->interlace == PlaneInterlace) ||
           (image_info->interlace == PartitionInterlace))
         (void) TIFFSetField(tiff,TIFFTAG_PLANARCONFIG,PLANARCONFIG_SEPARATE);
    rows_per_strip=1;
    if (TIFFScanlineSize(tiff) != 0)
      rows_per_strip=TIFFDefaultStripSize(tiff,0);
     option=GetImageOption(image_info,"tiff:rows-per-strip");
     if (option != (const char *) NULL)
       rows_per_strip=(size_t) strtol(option,(char **) NULL,10);
    switch (compress_tag)
    {
      case COMPRESSION_JPEG:
      {
#if defined(JPEG_SUPPORT)
        const char
          *sampling_factor;

        GeometryInfo
          geometry_info;

        MagickStatusType
          flags;

        rows_per_strip+=(16-(rows_per_strip % 16));
        if (image_info->quality != UndefinedCompressionQuality)
          (void) TIFFSetField(tiff,TIFFTAG_JPEGQUALITY,image_info->quality);
        (void) TIFFSetField(tiff,TIFFTAG_JPEGCOLORMODE,JPEGCOLORMODE_RAW);
        if (IssRGBCompatibleColorspace(image->colorspace) != MagickFalse)
          {
            const char
              *value;

            (void) TIFFSetField(tiff,TIFFTAG_JPEGCOLORMODE,JPEGCOLORMODE_RGB);
            sampling_factor=(const char *) NULL;
            value=GetImageProperty(image,"jpeg:sampling-factor");
            if (value != (char *) NULL)
              {
                sampling_factor=value;
                if (image->debug != MagickFalse)
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                    "  Input sampling-factors=%s",sampling_factor);
              }
            if (image_info->sampling_factor != (char *) NULL)
              sampling_factor=image_info->sampling_factor;
            if (sampling_factor != (const char *) NULL)
              {
                flags=ParseGeometry(sampling_factor,&geometry_info);
                if ((flags & SigmaValue) == 0)
                  geometry_info.sigma=geometry_info.rho;
                if (image->colorspace == YCbCrColorspace)
                  (void) TIFFSetField(tiff,TIFFTAG_YCBCRSUBSAMPLING,(uint16)
                    geometry_info.rho,(uint16) geometry_info.sigma);
              }
          }
        (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_BITSPERSAMPLE,
          &bits_per_sample);
        if (bits_per_sample == 12)
          (void) TIFFSetField(tiff,TIFFTAG_JPEGTABLESMODE,JPEGTABLESMODE_QUANT);
#endif
        break;
      }
      case COMPRESSION_ADOBE_DEFLATE:
      {
        rows_per_strip=(uint32) image->rows;
        (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_BITSPERSAMPLE,
          &bits_per_sample);
        if (((photometric == PHOTOMETRIC_RGB) ||
             (photometric == PHOTOMETRIC_MINISBLACK)) &&
            ((bits_per_sample == 8) || (bits_per_sample == 16)))
          (void) TIFFSetField(tiff,TIFFTAG_PREDICTOR,PREDICTOR_HORIZONTAL);
        (void) TIFFSetField(tiff,TIFFTAG_ZIPQUALITY,(long) (
          image_info->quality == UndefinedCompressionQuality ? 7 :
          MagickMin((ssize_t) image_info->quality/10,9)));
        break;
      }
      case COMPRESSION_CCITTFAX3:
      {
        /*
          Byte-aligned EOL.
        */
        rows_per_strip=(uint32) image->rows;
        (void) TIFFSetField(tiff,TIFFTAG_GROUP3OPTIONS,4);
        break;
      }
      case COMPRESSION_CCITTFAX4:
      {
        rows_per_strip=(uint32) image->rows;
        break;
      }
#if defined(LZMA_SUPPORT) && defined(COMPRESSION_LZMA)
      case COMPRESSION_LZMA:
      {
        if (((photometric == PHOTOMETRIC_RGB) ||
             (photometric == PHOTOMETRIC_MINISBLACK)) &&
            ((bits_per_sample == 8) || (bits_per_sample == 16)))
          (void) TIFFSetField(tiff,TIFFTAG_PREDICTOR,PREDICTOR_HORIZONTAL);
        (void) TIFFSetField(tiff,TIFFTAG_LZMAPRESET,(long) (
          image_info->quality == UndefinedCompressionQuality ? 7 :
          MagickMin((ssize_t) image_info->quality/10,9)));
        break;
      }
#endif
      case COMPRESSION_LZW:
      {
        (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_BITSPERSAMPLE,
          &bits_per_sample);
        if (((photometric == PHOTOMETRIC_RGB) ||
             (photometric == PHOTOMETRIC_MINISBLACK)) &&
            ((bits_per_sample == 8) || (bits_per_sample == 16)))
          (void) TIFFSetField(tiff,TIFFTAG_PREDICTOR,PREDICTOR_HORIZONTAL);
        break;
      }
      default:
        break;
    }
    if (rows_per_strip < 1)
      rows_per_strip=1;
    if ((image->rows/rows_per_strip) >= (1UL << 15))
      rows_per_strip=(uint32) (image->rows >> 15);
    (void) TIFFSetField(tiff,TIFFTAG_ROWSPERSTRIP,rows_per_strip);
    if ((image->x_resolution != 0.0) && (image->y_resolution != 0.0))
      {
        unsigned short
          units;

        /*
          Set image resolution.
        */
        units=RESUNIT_NONE;
        if (image->units == PixelsPerInchResolution)
          units=RESUNIT_INCH;
        if (image->units == PixelsPerCentimeterResolution)
          units=RESUNIT_CENTIMETER;
        (void) TIFFSetField(tiff,TIFFTAG_RESOLUTIONUNIT,(uint16) units);
        (void) TIFFSetField(tiff,TIFFTAG_XRESOLUTION,image->x_resolution);
        (void) TIFFSetField(tiff,TIFFTAG_YRESOLUTION,image->y_resolution);
        if ((image->page.x < 0) || (image->page.y < 0))
          (void) ThrowMagickException(&image->exception,GetMagickModule(),
            CoderError,"TIFF: negative image positions unsupported","%s",
            image->filename);
        if ((image->page.x > 0) && (image->x_resolution > 0.0))
          {
            /*
              Set horizontal image position.
            */
            (void) TIFFSetField(tiff,TIFFTAG_XPOSITION,(float) image->page.x/
              image->x_resolution);
          }
        if ((image->page.y > 0) && (image->y_resolution > 0.0))
          {
            /*
              Set vertical image position.
            */
            (void) TIFFSetField(tiff,TIFFTAG_YPOSITION,(float) image->page.y/
              image->y_resolution);
          }
      }
    if (image->chromaticity.white_point.x != 0.0)
      {
        float
          chromaticity[6];

        /*
          Set image chromaticity.
        */
        chromaticity[0]=(float) image->chromaticity.red_primary.x;
        chromaticity[1]=(float) image->chromaticity.red_primary.y;
        chromaticity[2]=(float) image->chromaticity.green_primary.x;
        chromaticity[3]=(float) image->chromaticity.green_primary.y;
        chromaticity[4]=(float) image->chromaticity.blue_primary.x;
        chromaticity[5]=(float) image->chromaticity.blue_primary.y;
        (void) TIFFSetField(tiff,TIFFTAG_PRIMARYCHROMATICITIES,chromaticity);
        chromaticity[0]=(float) image->chromaticity.white_point.x;
        chromaticity[1]=(float) image->chromaticity.white_point.y;
        (void) TIFFSetField(tiff,TIFFTAG_WHITEPOINT,chromaticity);
      }
    if ((LocaleCompare(image_info->magick,"PTIF") != 0) &&
        (image_info->adjoin != MagickFalse) && (GetImageListLength(image) > 1))
      {
        (void) TIFFSetField(tiff,TIFFTAG_SUBFILETYPE,FILETYPE_PAGE);
        if (image->scene != 0)
          (void) TIFFSetField(tiff,TIFFTAG_PAGENUMBER,(uint16) image->scene,
            GetImageListLength(image));
      }
    if (image->orientation != UndefinedOrientation)
      (void) TIFFSetField(tiff,TIFFTAG_ORIENTATION,(uint16) image->orientation);
    (void) TIFFSetProfiles(tiff,image);
    {
      uint16
        page,
        pages;

      page=(uint16) scene;
      pages=(uint16) GetImageListLength(image);
      if ((LocaleCompare(image_info->magick,"PTIF") != 0) &&
          (image_info->adjoin != MagickFalse) && (pages > 1))
        (void) TIFFSetField(tiff,TIFFTAG_SUBFILETYPE,FILETYPE_PAGE);
      (void) TIFFSetField(tiff,TIFFTAG_PAGENUMBER,page,pages);
    }
    (void) TIFFSetProperties(tiff,image_info,image);
DisableMSCWarning(4127)
    if (0)
RestoreMSCWarning
      (void) TIFFSetEXIFProperties(tiff,image);
    /*
      Write image scanlines.
    */
    if (GetTIFFInfo(image_info,tiff,&tiff_info) == MagickFalse)
      ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
    quantum_info->endian=LSBEndian;
    pixels=GetQuantumPixels(quantum_info);
    tiff_info.scanline=GetQuantumPixels(quantum_info);
    switch (photometric)
    {
      case PHOTOMETRIC_CIELAB:
      case PHOTOMETRIC_YCBCR:
      case PHOTOMETRIC_RGB:
      {
        /*
          RGB TIFF image.
        */
        switch (image_info->interlace)
        {
          case NoInterlace:
          default:
          {
            quantum_type=RGBQuantum;
            if (image->matte != MagickFalse)
              quantum_type=RGBAQuantum;
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              register const PixelPacket
                *magick_restrict p;

              p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
              if (p == (const PixelPacket *) NULL)
                break;
              (void) ExportQuantumPixels(image,(const CacheView *) NULL,
                quantum_info,quantum_type,pixels,&image->exception);
              if (TIFFWritePixels(tiff,&tiff_info,y,0,image) == -1)
                break;
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,SaveImageTag,(MagickOffsetType)
                    y,image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            break;
          }
          case PlaneInterlace:
          case PartitionInterlace:
          {
            /*
              Plane interlacing:  RRRRRR...GGGGGG...BBBBBB...
            */
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              register const PixelPacket
                *magick_restrict p;

              p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
              if (p == (const PixelPacket *) NULL)
                break;
              (void) ExportQuantumPixels(image,(const CacheView *) NULL,
                quantum_info,RedQuantum,pixels,&image->exception);
              if (TIFFWritePixels(tiff,&tiff_info,y,0,image) == -1)
                break;
            }
            if (image->previous == (Image *) NULL)
              {
                status=SetImageProgress(image,SaveImageTag,100,400);
                if (status == MagickFalse)
                  break;
              }
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              register const PixelPacket
                *magick_restrict p;

              p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
              if (p == (const PixelPacket *) NULL)
                break;
              (void) ExportQuantumPixels(image,(const CacheView *) NULL,
                quantum_info,GreenQuantum,pixels,&image->exception);
              if (TIFFWritePixels(tiff,&tiff_info,y,1,image) == -1)
                break;
            }
            if (image->previous == (Image *) NULL)
              {
                status=SetImageProgress(image,SaveImageTag,200,400);
                if (status == MagickFalse)
                  break;
              }
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              register const PixelPacket
                *magick_restrict p;

              p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
              if (p == (const PixelPacket *) NULL)
                break;
              (void) ExportQuantumPixels(image,(const CacheView *) NULL,
                quantum_info,BlueQuantum,pixels,&image->exception);
              if (TIFFWritePixels(tiff,&tiff_info,y,2,image) == -1)
                break;
            }
            if (image->previous == (Image *) NULL)
              {
                status=SetImageProgress(image,SaveImageTag,300,400);
                if (status == MagickFalse)
                  break;
              }
            if (image->matte != MagickFalse)
              for (y=0; y < (ssize_t) image->rows; y++)
              {
                register const PixelPacket
                  *magick_restrict p;

                p=GetVirtualPixels(image,0,y,image->columns,1,
                  &image->exception);
                if (p == (const PixelPacket *) NULL)
                  break;
                (void) ExportQuantumPixels(image,(const CacheView *) NULL,
                  quantum_info,AlphaQuantum,pixels,&image->exception);
                if (TIFFWritePixels(tiff,&tiff_info,y,3,image) == -1)
                  break;
              }
            if (image->previous == (Image *) NULL)
              {
                status=SetImageProgress(image,SaveImageTag,400,400);
                if (status == MagickFalse)
                  break;
              }
            break;
          }
        }
        break;
      }
      case PHOTOMETRIC_SEPARATED:
      {
        /*
          CMYK TIFF image.
        */
        quantum_type=CMYKQuantum;
        if (image->matte != MagickFalse)
          quantum_type=CMYKAQuantum;
        if (image->colorspace != CMYKColorspace)
          (void) TransformImageColorspace(image,CMYKColorspace);
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          register const PixelPacket
            *magick_restrict p;

          p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          (void) ExportQuantumPixels(image,(const CacheView *) NULL,
            quantum_info,quantum_type,pixels,&image->exception);
          if (TIFFWritePixels(tiff,&tiff_info,y,0,image) == -1)
            break;
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        break;
      }
      case PHOTOMETRIC_PALETTE:
      {
        uint16
          *blue,
          *green,
          *red;

        /*
          Colormapped TIFF image.
        */
        red=(uint16 *) AcquireQuantumMemory(65536,sizeof(*red));
        green=(uint16 *) AcquireQuantumMemory(65536,sizeof(*green));
        blue=(uint16 *) AcquireQuantumMemory(65536,sizeof(*blue));
        if ((red == (uint16 *) NULL) || (green == (uint16 *) NULL) ||
            (blue == (uint16 *) NULL))
          ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
        /*
          Initialize TIFF colormap.
        */
        (void) ResetMagickMemory(red,0,65536*sizeof(*red));
        (void) ResetMagickMemory(green,0,65536*sizeof(*green));
        (void) ResetMagickMemory(blue,0,65536*sizeof(*blue));
        for (i=0; i < (ssize_t) image->colors; i++)
        {
          red[i]=ScaleQuantumToShort(image->colormap[i].red);
          green[i]=ScaleQuantumToShort(image->colormap[i].green);
          blue[i]=ScaleQuantumToShort(image->colormap[i].blue);
        }
        (void) TIFFSetField(tiff,TIFFTAG_COLORMAP,red,green,blue);
        red=(uint16 *) RelinquishMagickMemory(red);
        green=(uint16 *) RelinquishMagickMemory(green);
        blue=(uint16 *) RelinquishMagickMemory(blue);
      }
      default:
      {
        /*
          Convert PseudoClass packets to contiguous grayscale scanlines.
        */
        quantum_type=IndexQuantum;
        if (image->matte != MagickFalse)
          {
            if (photometric != PHOTOMETRIC_PALETTE)
              quantum_type=GrayAlphaQuantum;
            else
              quantum_type=IndexAlphaQuantum;
           }
         else
           if (photometric != PHOTOMETRIC_PALETTE)
             quantum_type=GrayQuantum;
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          register const PixelPacket
            *magick_restrict p;

          p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          (void) ExportQuantumPixels(image,(const CacheView *) NULL,
            quantum_info,quantum_type,pixels,&image->exception);
          if (TIFFWritePixels(tiff,&tiff_info,y,0,image) == -1)
            break;
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        break;
      }
    }
    quantum_info=DestroyQuantumInfo(quantum_info);
    if (image->colorspace == LabColorspace)
      DecodeLabImage(image,&image->exception);
    DestroyTIFFInfo(&tiff_info);
DisableMSCWarning(4127)
    if (0 && (image_info->verbose != MagickFalse))
RestoreMSCWarning
      TIFFPrintDirectory(tiff,stdout,MagickFalse);
    (void) TIFFWriteDirectory(tiff);
    image=SyncNextImageInList(image);
    if (image == (Image *) NULL)
      break;
    status=SetImageProgress(image,SaveImagesTag,scene++,
      GetImageListLength(image));
    if (status == MagickFalse)
      break;
  } while (image_info->adjoin != MagickFalse);
  TIFFClose(tiff);
  return(MagickTrue);
}
