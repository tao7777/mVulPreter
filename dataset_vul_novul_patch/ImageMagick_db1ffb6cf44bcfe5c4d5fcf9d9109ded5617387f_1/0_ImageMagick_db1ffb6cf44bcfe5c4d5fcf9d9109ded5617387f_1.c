static MagickBooleanType SetGrayscaleImage(Image *image)
{
  CacheView
    *image_view;

  ExceptionInfo
    *exception;

  MagickBooleanType
    status;

  PixelPacket
    *colormap;

  register ssize_t
    i;

  ssize_t
    *colormap_index,
    j,
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->type != GrayscaleType)
    (void) TransformImageColorspace(image,GRAYColorspace);
  colormap_index=(ssize_t *) AcquireQuantumMemory(MaxColormapSize,
    sizeof(*colormap_index));
  if (colormap_index == (ssize_t *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      image->filename);
  if (image->storage_class != PseudoClass)
    {
      ExceptionInfo
        *exception;

       (void) ResetMagickMemory(colormap_index,(-1),MaxColormapSize*
         sizeof(*colormap_index));
       if (AcquireImageColormap(image,MaxColormapSize) == MagickFalse)
        {
          colormap_index=(ssize_t *) RelinquishMagickMemory(colormap_index);
          ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
            image->filename);
        }
       image->colors=0;
       status=MagickTrue;
       exception=(&image->exception);
      image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
      #pragma omp parallel for schedule(static,4) shared(status) \
        magick_threads(image,image,image->rows,1)
#endif
      for (y=0; y < (ssize_t) image->rows; y++)
      {
        register IndexPacket
          *magick_restrict indexes;

        register PixelPacket
          *magick_restrict q;

        register ssize_t
          x;

        if (status == MagickFalse)
          continue;
        q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,
          exception);
        if (q == (PixelPacket *) NULL)
          {
            status=MagickFalse;
            continue;
          }
        indexes=GetCacheViewAuthenticIndexQueue(image_view);
        for (x=0; x < (ssize_t) image->columns; x++)
        {
          register size_t
            intensity;

          intensity=ScaleQuantumToMap(GetPixelRed(q));
          if (colormap_index[intensity] < 0)
            {
#if defined(MAGICKCORE_OPENMP_SUPPORT)
              #pragma omp critical (MagickCore_SetGrayscaleImage)
#endif
              if (colormap_index[intensity] < 0)
                {
                  colormap_index[intensity]=(ssize_t) image->colors;
                  image->colormap[image->colors].red=GetPixelRed(q);
                  image->colormap[image->colors].green=GetPixelGreen(q);
                  image->colormap[image->colors].blue=GetPixelBlue(q);
                  image->colors++;
               }
            }
          SetPixelIndex(indexes+x,colormap_index[intensity]);
          q++;
        }
        if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
          status=MagickFalse;
      }
      image_view=DestroyCacheView(image_view);
    }
  for (i=0; i < (ssize_t) image->colors; i++)
    image->colormap[i].opacity=(unsigned short) i;
  qsort((void *) image->colormap,image->colors,sizeof(PixelPacket),
    IntensityCompare);
  colormap=(PixelPacket *) AcquireQuantumMemory(image->colors,
    sizeof(*colormap));
  if (colormap == (PixelPacket *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      image->filename);
  j=0;
  colormap[j]=image->colormap[0];
  for (i=0; i < (ssize_t) image->colors; i++)
  {
    if (IsSameColor(image,&colormap[j],&image->colormap[i]) == MagickFalse)
      {
        j++;
        colormap[j]=image->colormap[i];
      }
    colormap_index[(ssize_t) image->colormap[i].opacity]=j;
  }
  image->colors=(size_t) (j+1);
  image->colormap=(PixelPacket *) RelinquishMagickMemory(image->colormap);
  image->colormap=colormap;
  status=MagickTrue;
  exception=(&image->exception);
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static,4) shared(status) \
    magick_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register IndexPacket
      *magick_restrict indexes;

    register const PixelPacket
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    indexes=GetCacheViewAuthenticIndexQueue(image_view);
    for (x=0; x < (ssize_t) image->columns; x++)
      SetPixelIndex(indexes+x,colormap_index[ScaleQuantumToMap(GetPixelIndex(
        indexes+x))]);
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
  }
  image_view=DestroyCacheView(image_view);
  colormap_index=(ssize_t *) RelinquishMagickMemory(colormap_index);
  image->type=GrayscaleType;
  if (SetImageMonochrome(image,&image->exception) != MagickFalse)
    image->type=BilevelType;
  return(status);
}
