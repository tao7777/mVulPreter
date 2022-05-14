static MagickPixelPacket **AcquirePixelThreadSet(const Image *image)
static MagickPixelPacket **AcquirePixelThreadSet(const Image *images)
 {
  const Image
    *next;

   MagickPixelPacket
     **pixels;
 
  register ssize_t
    i,
     j;
 
   size_t
    columns,
     number_threads;
 
   number_threads=(size_t) GetMagickResourceLimit(ThreadResource);
  pixels=(MagickPixelPacket **) AcquireQuantumMemory(number_threads,
    sizeof(*pixels));
   if (pixels == (MagickPixelPacket **) NULL)
     return((MagickPixelPacket **) NULL);
   (void) memset(pixels,0,number_threads*sizeof(*pixels));
  columns=images->columns;
  for (next=images; next != (Image *) NULL; next=next->next)
    columns=MagickMax(next->columns,columns);
   for (i=0; i < (ssize_t) number_threads; i++)
   {
    pixels[i]=(MagickPixelPacket *) AcquireQuantumMemory(columns,
       sizeof(**pixels));
     if (pixels[i] == (MagickPixelPacket *) NULL)
       return(DestroyPixelThreadSet(pixels));
    for (j=0; j < (ssize_t) columns; j++)
      GetMagickPixelPacket(images,&pixels[i][j]);
   }
   return(pixels);
 }
