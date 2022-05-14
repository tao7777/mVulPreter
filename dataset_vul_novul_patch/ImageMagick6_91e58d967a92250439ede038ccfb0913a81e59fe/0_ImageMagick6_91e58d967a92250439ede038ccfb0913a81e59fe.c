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
    rows;
 
  rows=MagickMax(GetImageListLength(images),
    (size_t) GetMagickResourceLimit(ThreadResource));
  pixels=(MagickPixelPacket **) AcquireQuantumMemory(rows,sizeof(*pixels));
   if (pixels == (MagickPixelPacket **) NULL)
     return((MagickPixelPacket **) NULL);
   columns=images->columns;
   for (next=images; next != (Image *) NULL; next=next->next)
     columns=MagickMax(next->columns,columns);
  for (i=0; i < (ssize_t) rows; i++)
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
