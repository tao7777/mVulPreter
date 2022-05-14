MagickExport void *ResizeQuantumMemory(void *memory,const size_t count,
  const size_t quantum)
{
   size_t
     extent;
 
  if (HeapOverflowSanityCheck(count,quantum) != MagickFalse)
     {
       memory=RelinquishMagickMemory(memory);
       return((void *) NULL);
    }
  extent=count*quantum;
  return(ResizeMagickMemory(memory,extent));
}
