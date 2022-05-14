static Image *ReadDDSImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  MagickBooleanType
    status,
    cubemap = MagickFalse,
    volume = MagickFalse,
    matte;

  CompressionType
    compression;

  DDSInfo
    dds_info;

  DDSDecoder
    *decoder;

  size_t
    n,
    num_images;

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
     Initialize image structure.
   */
  if (ReadDDSInfo(image, &dds_info) != MagickTrue)
     ThrowReaderException(CorruptImageError,"ImproperImageHeader");
 
   if (dds_info.ddscaps2 & DDSCAPS2_CUBEMAP)
     cubemap = MagickTrue;

  if (dds_info.ddscaps2 & DDSCAPS2_VOLUME && dds_info.depth > 0)
    volume = MagickTrue;

  (void) SeekBlob(image, 128, SEEK_SET);

  /*
    Determine pixel format
  */
  if (dds_info.pixelformat.flags & DDPF_RGB)
    {
      compression = NoCompression;
      if (dds_info.pixelformat.flags & DDPF_ALPHAPIXELS)
        {
          matte = MagickTrue;
          decoder = ReadUncompressedRGBA;
        }
      else
        {
          matte = MagickTrue;
          decoder = ReadUncompressedRGB;
        }
    }
  else if (dds_info.pixelformat.flags & DDPF_LUMINANCE)
   {
      compression = NoCompression;
      if (dds_info.pixelformat.flags & DDPF_ALPHAPIXELS)
        {
          /* Not sure how to handle this */
          ThrowReaderException(CorruptImageError, "ImageTypeNotSupported");
        }
      else
        {
          matte = MagickFalse;
          decoder = ReadUncompressedRGB;
        }
    }
  else if (dds_info.pixelformat.flags & DDPF_FOURCC)
    {
      switch (dds_info.pixelformat.fourcc)
      {
        case FOURCC_DXT1:
        {
          matte = MagickFalse;
          compression = DXT1Compression;
          decoder = ReadDXT1;
          break;
        }
        case FOURCC_DXT3:
        {
          matte = MagickTrue;
          compression = DXT3Compression;
          decoder = ReadDXT3;
          break;
        }
        case FOURCC_DXT5:
        {
          matte = MagickTrue;
          compression = DXT5Compression;
          decoder = ReadDXT5;
          break;
        }
        default:
        {
          /* Unknown FOURCC */
          ThrowReaderException(CorruptImageError, "ImageTypeNotSupported");
        }
      }
    }
  else
    {
      /* Neither compressed nor uncompressed... thus unsupported */
      ThrowReaderException(CorruptImageError, "ImageTypeNotSupported");
    }

  num_images = 1;
  if (cubemap)
    {
      /*
        Determine number of faces defined in the cubemap
      */
      num_images = 0;
      if (dds_info.ddscaps2 & DDSCAPS2_CUBEMAP_POSITIVEX) num_images++;
      if (dds_info.ddscaps2 & DDSCAPS2_CUBEMAP_NEGATIVEX) num_images++;
      if (dds_info.ddscaps2 & DDSCAPS2_CUBEMAP_POSITIVEY) num_images++;
      if (dds_info.ddscaps2 & DDSCAPS2_CUBEMAP_NEGATIVEY) num_images++;
      if (dds_info.ddscaps2 & DDSCAPS2_CUBEMAP_POSITIVEZ) num_images++;
      if (dds_info.ddscaps2 & DDSCAPS2_CUBEMAP_NEGATIVEZ) num_images++;
    }

   if (volume)
     num_images = dds_info.depth;
 
  if (num_images < 1)
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");

   for (n = 0; n < num_images; n++)
   {
     if (n != 0)
      {
        if (EOFBlob(image) != MagickFalse)
          ThrowReaderException(CorruptImageError,"UnexpectedEndOfFile");
        /* Start a new image */
        AcquireNextImage(image_info,image);
        if (GetNextImageInList(image) == (Image *) NULL)
          return(DestroyImageList(image));
        image=SyncNextImageInList(image);
      }
 
    image->matte = matte;
    image->compression = compression;
    image->columns = dds_info.width;
    image->rows = dds_info.height;
    image->storage_class = DirectClass;
    image->endian = LSBEndian;
    image->depth = 8;
    if (image_info->ping != MagickFalse)
      {
        (void) CloseBlob(image);
        return(GetFirstImageInList(image));
      }
    status=SetImageExtent(image,image->columns,image->rows);
    if (status == MagickFalse)
      {
        InheritException(exception,&image->exception);
        return(DestroyImageList(image));
      }

    if ((decoder)(image, &dds_info, exception) != MagickTrue)
      {
        (void) CloseBlob(image);
        return(GetFirstImageInList(image));
      }
  }

  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
