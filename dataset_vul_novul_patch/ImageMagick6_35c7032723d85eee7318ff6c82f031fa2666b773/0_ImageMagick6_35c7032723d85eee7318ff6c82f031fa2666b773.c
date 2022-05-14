MagickExport Image *AdaptiveThresholdImage(const Image *image,
  const size_t width,const size_t height,const ssize_t offset,
  ExceptionInfo *exception)
{
#define ThresholdImageTag  "Threshold/Image"

  CacheView
    *image_view,
    *threshold_view;

  Image
    *threshold_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  MagickPixelPacket
    zero;

  MagickRealType
    number_pixels;

  ssize_t
    y;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
   threshold_image=CloneImage(image,0,0,MagickTrue,exception);
   if (threshold_image == (Image *) NULL)
     return((Image *) NULL);
  if ((width == 0) || (height == 0))
     return(threshold_image);
   if (SetImageStorageClass(threshold_image,DirectClass) == MagickFalse)
     {
      InheritException(exception,&threshold_image->exception);
      threshold_image=DestroyImage(threshold_image);
      return((Image *) NULL);
    }
  /*
    Local adaptive threshold.
  */
  status=MagickTrue;
  progress=0;
  GetMagickPixelPacket(image,&zero);
  number_pixels=(MagickRealType) (width*height);
  image_view=AcquireVirtualCacheView(image,exception);
  threshold_view=AcquireAuthenticCacheView(threshold_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,threshold_image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    MagickBooleanType
      sync;

    MagickPixelPacket
      channel_bias,
      channel_sum;

    register const IndexPacket
      *magick_restrict indexes;

    register const PixelPacket
      *magick_restrict p,
      *magick_restrict r;

    register IndexPacket
      *magick_restrict threshold_indexes;

    register PixelPacket
      *magick_restrict q;

    register ssize_t
      x;

    ssize_t
      u,
      v;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,-((ssize_t) width/2L),y-(ssize_t)
      height/2L,image->columns+width,height,exception);
    q=GetCacheViewAuthenticPixels(threshold_view,0,y,threshold_image->columns,1,
      exception);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    indexes=GetCacheViewVirtualIndexQueue(image_view);
    threshold_indexes=GetCacheViewAuthenticIndexQueue(threshold_view);
    channel_bias=zero;
    channel_sum=zero;
    r=p;
    for (v=0; v < (ssize_t) height; v++)
    {
      for (u=0; u < (ssize_t) width; u++)
      {
        if (u == (ssize_t) (width-1))
          {
            channel_bias.red+=r[u].red;
            channel_bias.green+=r[u].green;
            channel_bias.blue+=r[u].blue;
            channel_bias.opacity+=r[u].opacity;
            if (image->colorspace == CMYKColorspace)
              channel_bias.index=(MagickRealType)
                GetPixelIndex(indexes+(r-p)+u);
          }
        channel_sum.red+=r[u].red;
        channel_sum.green+=r[u].green;
        channel_sum.blue+=r[u].blue;
        channel_sum.opacity+=r[u].opacity;
        if (image->colorspace == CMYKColorspace)
          channel_sum.index=(MagickRealType) GetPixelIndex(indexes+(r-p)+u);
      }
      r+=image->columns+width;
    }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      MagickPixelPacket
        mean;

      mean=zero;
      r=p;
      channel_sum.red-=channel_bias.red;
      channel_sum.green-=channel_bias.green;
      channel_sum.blue-=channel_bias.blue;
      channel_sum.opacity-=channel_bias.opacity;
      channel_sum.index-=channel_bias.index;
      channel_bias=zero;
      for (v=0; v < (ssize_t) height; v++)
      {
        channel_bias.red+=r[0].red;
        channel_bias.green+=r[0].green;
        channel_bias.blue+=r[0].blue;
        channel_bias.opacity+=r[0].opacity;
        if (image->colorspace == CMYKColorspace)
          channel_bias.index=(MagickRealType) GetPixelIndex(indexes+x+(r-p)+0);
        channel_sum.red+=r[width-1].red;
        channel_sum.green+=r[width-1].green;
        channel_sum.blue+=r[width-1].blue;
        channel_sum.opacity+=r[width-1].opacity;
        if (image->colorspace == CMYKColorspace)
          channel_sum.index=(MagickRealType) GetPixelIndex(indexes+x+(r-p)+
            width-1);
        r+=image->columns+width;
      }
      mean.red=(MagickRealType) (channel_sum.red/number_pixels+offset);
      mean.green=(MagickRealType) (channel_sum.green/number_pixels+offset);
      mean.blue=(MagickRealType) (channel_sum.blue/number_pixels+offset);
      mean.opacity=(MagickRealType) (channel_sum.opacity/number_pixels+offset);
      if (image->colorspace == CMYKColorspace)
        mean.index=(MagickRealType) (channel_sum.index/number_pixels+offset);
      SetPixelRed(q,((MagickRealType) GetPixelRed(q) <= mean.red) ?
        0 : QuantumRange);
      SetPixelGreen(q,((MagickRealType) GetPixelGreen(q) <= mean.green) ?
        0 : QuantumRange);
      SetPixelBlue(q,((MagickRealType) GetPixelBlue(q) <= mean.blue) ?
        0 : QuantumRange);
      SetPixelOpacity(q,((MagickRealType) GetPixelOpacity(q) <= mean.opacity) ?
        0 : QuantumRange);
      if (image->colorspace == CMYKColorspace)
        SetPixelIndex(threshold_indexes+x,(((MagickRealType) GetPixelIndex(
          threshold_indexes+x) <= mean.index) ? 0 : QuantumRange));
      p++;
      q++;
    }
    sync=SyncCacheViewAuthenticPixels(threshold_view,exception);
    if (sync == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,ThresholdImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  threshold_view=DestroyCacheView(threshold_view);
  image_view=DestroyCacheView(image_view);
  if (status == MagickFalse)
    threshold_image=DestroyImage(threshold_image);
  return(threshold_image);
}
