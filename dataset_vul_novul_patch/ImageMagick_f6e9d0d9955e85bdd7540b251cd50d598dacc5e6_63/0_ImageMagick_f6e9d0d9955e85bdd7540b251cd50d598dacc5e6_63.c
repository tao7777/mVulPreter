static Image *ReadTILEImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image,
    *tile_image;

   ImageInfo
     *read_info;
 
  MagickBooleanType
    status;

   /*
     Initialize Image structure.
   */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  read_info=CloneImageInfo(image_info);
  SetImageInfoBlob(read_info,(void *) NULL,0);
  *read_info->magick='\0';
  tile_image=ReadImage(read_info,exception);
  read_info=DestroyImageInfo(read_info);
  if (tile_image == (Image *) NULL)
    return((Image *) NULL);
   image=AcquireImage(image_info);
   if ((image->columns == 0) || (image->rows == 0))
     ThrowReaderException(OptionError,"MustSpecifyImageSize");
  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }
   if (*image_info->filename == '\0')
     ThrowReaderException(OptionError,"MustSpecifyAnImageName");
   image->colorspace=tile_image->colorspace;
  image->matte=tile_image->matte;
  if (image->matte != MagickFalse)
    (void) SetImageBackgroundColor(image);
  (void) CopyMagickString(image->filename,image_info->filename,MaxTextExtent);
  if (LocaleCompare(tile_image->magick,"PATTERN") == 0)
    {
      tile_image->tile_offset.x=0;
      tile_image->tile_offset.y=0;
    }
  (void) TextureImage(image,tile_image);
  tile_image=DestroyImage(tile_image);
  if (image->colorspace == GRAYColorspace)
    image->type=GrayscaleType;
  return(GetFirstImageInList(image));
}
