static MagickPixelPacket **AcquirePixelThreadSet(const Image *image)
 {
   MagickPixelPacket
     **pixels;
 
  register ssize_t
    i,
     j;
 
   size_t
     number_threads;
 
   number_threads=(size_t) GetMagickResourceLimit(ThreadResource);
  pixels=(MagickPixelPacket **) AcquireQuantumMemory(number_threads,
    sizeof(*pixels));
   if (pixels == (MagickPixelPacket **) NULL)
     return((MagickPixelPacket **) NULL);
   (void) memset(pixels,0,number_threads*sizeof(*pixels));
   for (i=0; i < (ssize_t) number_threads; i++)
   {
    pixels[i]=(MagickPixelPacket *) AcquireQuantumMemory(image->columns,
       sizeof(**pixels));
     if (pixels[i] == (MagickPixelPacket *) NULL)
       return(DestroyPixelThreadSet(pixels));
    for (j=0; j < (ssize_t) image->columns; j++)
      GetMagickPixelPacket(image,&pixels[i][j]);
   }
   return(pixels);
 }
