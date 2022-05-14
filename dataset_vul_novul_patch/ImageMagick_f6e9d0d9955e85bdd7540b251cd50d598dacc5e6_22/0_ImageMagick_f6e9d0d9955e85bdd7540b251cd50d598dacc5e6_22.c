static Image *ReadHALDImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  MagickBooleanType
    status;

  size_t
    cube_size,
    level;

  ssize_t
    y;

  /*
    Create HALD color lookup table image.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AcquireImage(image_info);
  level=0;
  if (*image_info->filename != '\0')
    level=StringToUnsignedLong(image_info->filename);
  if (level < 2)
    level=8;
  status=MagickTrue;
   cube_size=level*level;
   image->columns=(size_t) (level*cube_size);
   image->rows=(size_t) (level*cube_size);
  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }
   for (y=0; y < (ssize_t) image->rows; y+=(ssize_t) level)
   {
     ssize_t
      blue,
      green,
      red;

    register PixelPacket
      *restrict q;
 
     if (status == MagickFalse)
       continue;
    q=QueueAuthenticPixels(image,0,y,image->columns,(size_t) level,exception);
     if (q == (PixelPacket *) NULL)
       {
         status=MagickFalse;
        continue;
      }
    blue=y/(ssize_t) level;
    for (green=0; green < (ssize_t) cube_size; green++)
    {
      for (red=0; red < (ssize_t) cube_size; red++)
      {
        SetPixelRed(q,ClampToQuantum((MagickRealType)
          (QuantumRange*red/(cube_size-1.0))));
        SetPixelGreen(q,ClampToQuantum((MagickRealType)
          (QuantumRange*green/(cube_size-1.0))));
        SetPixelBlue(q,ClampToQuantum((MagickRealType)
          (QuantumRange*blue/(cube_size-1.0))));
        SetPixelOpacity(q,OpaqueOpacity);
        q++;
      }
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      status=MagickFalse;
  }
  return(GetFirstImageInList(image));
}
