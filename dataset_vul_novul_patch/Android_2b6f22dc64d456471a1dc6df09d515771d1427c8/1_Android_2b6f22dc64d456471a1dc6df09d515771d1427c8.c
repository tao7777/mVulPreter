void* H264SwDecMalloc(u32 size)
 {
 
 #if defined(CHECK_MEMORY_USAGE)
     /* Note that if the decoder has to free and reallocate some of the buffers
      * the total value will be invalid */
     static u32 numBytes = 0;
    numBytes += size;
     DEBUG(("Allocated %d bytes, total %d\n", size, numBytes));
 #endif
 
    return malloc(size);
 }
