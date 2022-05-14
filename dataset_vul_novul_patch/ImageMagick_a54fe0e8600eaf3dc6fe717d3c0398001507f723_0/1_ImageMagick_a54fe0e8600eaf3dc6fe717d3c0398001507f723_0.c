static inline void CopyPixels(PixelPacket *destination,
  const PixelPacket *source,const MagickSizeType number_pixels)
{
#if !defined(MAGICKCORE_OPENMP_SUPPORT) || (MAGICKCORE_QUANTUM_DEPTH <= 8)
  (void) memcpy(destination,source,(size_t) number_pixels*sizeof(*source));
#else
  {
    register MagickOffsetType
      i;
    if ((number_pixels*sizeof(*source)) < MagickMaxBufferExtent)
      {
        (void) memcpy(destination,source,(size_t) number_pixels*
          sizeof(*source));
        return;
      }
    #pragma omp parallel for
    for (i=0; i < (MagickOffsetType) number_pixels; i++)
      destination[i]=source[i];
  }
#endif
}
