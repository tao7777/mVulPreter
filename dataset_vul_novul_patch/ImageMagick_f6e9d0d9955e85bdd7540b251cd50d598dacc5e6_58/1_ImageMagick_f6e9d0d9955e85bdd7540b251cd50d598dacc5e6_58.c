static Image *ReadSTEGANOImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
#define GetBit(alpha,i) MagickMin((((size_t) (alpha) >> (size_t) \
  (i)) & 0x01),16)
#define SetBit(indexes,i,set) SetPixelIndex(indexes,((set) != 0 ? \
  (size_t) GetPixelIndex(indexes) | (one << (size_t) (i)) : (size_t) \
  GetPixelIndex(indexes) & ~(one << (size_t) (i))))

  Image
    *image,
    *watermark;

  ImageInfo
    *read_info;

  int
    c;

  MagickBooleanType
    status;

  PixelPacket
    pixel;

  register IndexPacket
    *indexes;

  register PixelPacket
    *q;

  register ssize_t
    x;

  size_t
    depth,
    one;

  ssize_t
    i,
    j,
    k,
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
  one=1;
  image=AcquireImage(image_info);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionError,"MustSpecifyImageSize");
  read_info=CloneImageInfo(image_info);
  SetImageInfoBlob(read_info,(void *) NULL,0);
  *read_info->magick='\0';
  watermark=ReadImage(read_info,exception);
  read_info=DestroyImageInfo(read_info);
  if (watermark == (Image *) NULL)
    return((Image *) NULL);
  watermark->depth=MAGICKCORE_QUANTUM_DEPTH;
  if (AcquireImageColormap(image,MaxColormapSize) == MagickFalse)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  if (image_info->ping != MagickFalse)
    {
       (void) CloseBlob(image);
       return(GetFirstImageInList(image));
     }
   /*
     Get hidden watermark from low-order bits of image.
   */
  c=0;
  i=0;
  j=0;
  i=(ssize_t) (watermark->depth-1);
  depth=watermark->depth;
  for (k=image->offset; (i >= 0) && (j < (ssize_t) depth); i--)
  {
    for (y=0; (y < (ssize_t) image->rows) && (j < (ssize_t) depth); y++)
    {
      x=0;
      for ( ; (x < (ssize_t) image->columns) && (j < (ssize_t) depth); x++)
      {
        if ((k/(ssize_t) watermark->columns) >= (ssize_t) watermark->rows)
          break;
        (void) GetOneVirtualPixel(watermark,k % (ssize_t) watermark->columns,
          k/(ssize_t) watermark->columns,&pixel,exception);
        q=GetAuthenticPixels(image,x,y,1,1,exception);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetAuthenticIndexQueue(image);
        switch (c)
        {
          case 0:
          {
            SetBit(indexes,i,GetBit(pixel.red,j));
            break;
          }
          case 1:
          {
            SetBit(indexes,i,GetBit(pixel.green,j));
            break;
          }
          case 2:
          {
            SetBit(indexes,i,GetBit(pixel.blue,j));
            break;
          }
        }
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
        c++;
        if (c == 3)
          c=0;
        k++;
        if (k == (ssize_t) (watermark->columns*watermark->columns))
          k=0;
        if (k == image->offset)
          j++;
      }
    }
    status=SetImageProgress(image,LoadImagesTag,(MagickOffsetType) i,depth);
    if (status == MagickFalse)
      break;
  }
  watermark=DestroyImage(watermark);
  (void) SyncImage(image);
  return(GetFirstImageInList(image));
}
