static unsigned char *AcquireCompactPixels(const Image *image,
  ExceptionInfo *exception)
{
  size_t
    packet_size;

  unsigned char
    *compact_pixels;

  packet_size=image->depth > 8UL ? 2UL : 1UL;
  compact_pixels=(unsigned char *) AcquireQuantumMemory((9*
    image->columns)+1,packet_size*sizeof(*compact_pixels));
   if (compact_pixels == (unsigned char *) NULL)
     {
       (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",
        image->filename);
     }
   return(compact_pixels);
 }
