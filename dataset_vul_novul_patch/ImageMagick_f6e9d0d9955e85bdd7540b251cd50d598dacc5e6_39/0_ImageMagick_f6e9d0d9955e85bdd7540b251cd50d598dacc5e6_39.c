static Image *ReadNULLImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
   Image
     *image;
 
  MagickBooleanType
    status;

   MagickPixelPacket
     background;
 
  register IndexPacket
    *indexes;

  register ssize_t
    x;

  register PixelPacket
    *q;

  ssize_t
    y;

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
  image=AcquireImage(image_info);
  if (image->columns == 0)
     image->columns=1;
   if (image->rows == 0)
     image->rows=1;
  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }
   image->matte=MagickTrue;
   GetMagickPixelPacket(image,&background);
   background.opacity=(MagickRealType) TransparentOpacity;
  if (image->colorspace == CMYKColorspace)
    ConvertRGBToCMYK(&background);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetAuthenticIndexQueue(image);
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      SetPixelPacket(image,&background,q,indexes);
      q++;
      indexes++;
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
  }
  return(GetFirstImageInList(image));
}
