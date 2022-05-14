MagickExport void *AcquireQuantumMemory(const size_t count,const size_t quantum)
{
   size_t
     extent;
 
  if (HeapOverflowSanityCheck(count,quantum) != MagickFalse)
     return((void *) NULL);
   extent=count*quantum;
   return(AcquireMagickMemory(extent));
}
