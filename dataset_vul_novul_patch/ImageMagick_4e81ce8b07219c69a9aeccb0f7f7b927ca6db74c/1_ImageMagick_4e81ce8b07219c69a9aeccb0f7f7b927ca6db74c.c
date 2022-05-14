static inline void AllocatePixelCachePixels(CacheInfo *cache_info)
{
  cache_info->mapped=MagickFalse;
  cache_info->pixels=(PixelPacket *) MagickAssumeAligned(
    AcquireAlignedMemory(1,(size_t) cache_info->length));
  if (cache_info->pixels == (PixelPacket *) NULL)
    {
      cache_info->mapped=MagickTrue;
      cache_info->pixels=(PixelPacket *) MapBlob(-1,IOMode,0,(size_t)
        cache_info->length);
    }
}
