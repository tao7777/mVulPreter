static MagickBooleanType ClonePixelCacheRepository(
  CacheInfo *restrict clone_info,CacheInfo *restrict cache_info,
  ExceptionInfo *exception)
{
#define MaxCacheThreads  2
#define cache_threads(source,destination,chunk) \
  num_threads((chunk) < (16*GetMagickResourceLimit(ThreadResource)) ? 1 : \
    GetMagickResourceLimit(ThreadResource) < MaxCacheThreads ? \
    GetMagickResourceLimit(ThreadResource) : MaxCacheThreads)

  MagickBooleanType
    status;

  NexusInfo
    **restrict cache_nexus,
    **restrict clone_nexus;

  size_t
    length;

  ssize_t
    y;

  assert(cache_info != (CacheInfo *) NULL);
  assert(clone_info != (CacheInfo *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  if (cache_info->type == PingCache)
    return(MagickTrue);
  if (((cache_info->type == MemoryCache) || (cache_info->type == MapCache)) &&
      ((clone_info->type == MemoryCache) || (clone_info->type == MapCache)) &&
      (cache_info->columns == clone_info->columns) &&
      (cache_info->rows == clone_info->rows) &&
      (cache_info->active_index_channel == clone_info->active_index_channel))
    {
       /*
         Identical pixel cache morphology.
       */
      CopyPixels(clone_info->pixels,cache_info->pixels,cache_info->columns*
        cache_info->rows);
       if ((cache_info->active_index_channel != MagickFalse) &&
           (clone_info->active_index_channel != MagickFalse))
         (void) memcpy(clone_info->indexes,cache_info->indexes,
          cache_info->columns*cache_info->rows*sizeof(*cache_info->indexes));
      return(MagickTrue);
    }
  /*
    Mismatched pixel cache morphology.
  */
  cache_nexus=AcquirePixelCacheNexus(MaxCacheThreads);
  clone_nexus=AcquirePixelCacheNexus(MaxCacheThreads);
  if ((cache_nexus == (NexusInfo **) NULL) ||
      (clone_nexus == (NexusInfo **) NULL))
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  length=(size_t) MagickMin(cache_info->columns,clone_info->columns)*
    sizeof(*cache_info->pixels);
  status=MagickTrue;
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static,4) shared(status) \
    cache_threads(cache_info,clone_info,cache_info->rows)
#endif
  for (y=0; y < (ssize_t) cache_info->rows; y++)
  {
    const int
      id = GetOpenMPThreadId();

    PixelPacket
      *pixels;

    RectangleInfo
      region;

    if (status == MagickFalse)
      continue;
    if (y >= (ssize_t) clone_info->rows)
      continue;
    region.width=cache_info->columns;
    region.height=1;
    region.x=0;
    region.y=y;
    pixels=SetPixelCacheNexusPixels(cache_info,ReadMode,&region,MagickTrue,
      cache_nexus[id],exception);
    if (pixels == (PixelPacket *) NULL)
      continue;
    status=ReadPixelCachePixels(cache_info,cache_nexus[id],exception);
    if (status == MagickFalse)
      continue;
    region.width=clone_info->columns;
    pixels=SetPixelCacheNexusPixels(clone_info,WriteMode,&region,MagickTrue,
      clone_nexus[id],exception);
    if (pixels == (PixelPacket *) NULL)
      continue;
    (void) ResetMagickMemory(clone_nexus[id]->pixels,0,(size_t)
      clone_nexus[id]->length);
    (void) memcpy(clone_nexus[id]->pixels,cache_nexus[id]->pixels,length);
    status=WritePixelCachePixels(clone_info,clone_nexus[id],exception);
  }
  if ((cache_info->active_index_channel != MagickFalse) &&
      (clone_info->active_index_channel != MagickFalse))
    {
      /*
        Clone indexes.
      */
      length=(size_t) MagickMin(cache_info->columns,clone_info->columns)*
        sizeof(*cache_info->indexes);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
      #pragma omp parallel for schedule(static,4) shared(status) \
        cache_threads(cache_info,clone_info,cache_info->rows)
#endif
      for (y=0; y < (ssize_t) cache_info->rows; y++)
      {
        const int
          id = GetOpenMPThreadId();

        PixelPacket
          *pixels;

        RectangleInfo
          region;

        if (status == MagickFalse)
          continue;
        if (y >= (ssize_t) clone_info->rows)
          continue;
        region.width=cache_info->columns;
        region.height=1;
        region.x=0;
        region.y=y;
        pixels=SetPixelCacheNexusPixels(cache_info,ReadMode,&region,MagickTrue,
          cache_nexus[id],exception);
        if (pixels == (PixelPacket *) NULL)
          continue;
        status=ReadPixelCacheIndexes(cache_info,cache_nexus[id],exception);
        if (status == MagickFalse)
          continue;
        region.width=clone_info->columns;
        pixels=SetPixelCacheNexusPixels(clone_info,WriteMode,&region,MagickTrue,
          clone_nexus[id],exception);
        if (pixels == (PixelPacket *) NULL)
          continue;
        (void) memcpy(clone_nexus[id]->indexes,cache_nexus[id]->indexes,length);
        status=WritePixelCacheIndexes(clone_info,clone_nexus[id],exception);
      }
    }
  cache_nexus=DestroyPixelCacheNexus(cache_nexus,MaxCacheThreads);
  clone_nexus=DestroyPixelCacheNexus(clone_nexus,MaxCacheThreads);
  if (cache_info->debug != MagickFalse)
    {
      char
        message[MaxTextExtent];

      (void) FormatLocaleString(message,MaxTextExtent,"%s => %s",
        CommandOptionToMnemonic(MagickCacheOptions,(ssize_t) cache_info->type),
        CommandOptionToMnemonic(MagickCacheOptions,(ssize_t) clone_info->type));
      (void) LogMagickEvent(CacheEvent,GetMagickModule(),"%s",message);
    }
  return(status);
}
