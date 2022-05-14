static MagickBooleanType ReadPSDChannelRLE(Image *image,const PSDInfo *psd_info,
  const ssize_t type,MagickOffsetType *sizes,ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  size_t
    length,
    row_size;

  ssize_t
    count,
    y;

  unsigned char
    *compact_pixels,
    *pixels;

  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
       "      layer data is RLE compressed");

  row_size=GetPSDRowSize(image);
  pixels=(unsigned char *) AcquireQuantumMemory(row_size,sizeof(*pixels));
  if (pixels == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      image->filename);

  length=0;
  for (y=0; y < (ssize_t) image->rows; y++)
    if ((MagickOffsetType) length < sizes[y])
      length=(size_t) sizes[y];

   if (length > row_size + 256) // arbitrary number
     {
       pixels=(unsigned char *) RelinquishMagickMemory(pixels);
      ThrowBinaryException(ResourceLimitError,"InvalidLength",image->filename);
     }
 
   compact_pixels=(unsigned char *) AcquireQuantumMemory(length,sizeof(*pixels));
  if (compact_pixels == (unsigned char *) NULL)
    {
      pixels=(unsigned char *) RelinquishMagickMemory(pixels);
      ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
        image->filename);
    }

  (void) ResetMagickMemory(compact_pixels,0,length*sizeof(*compact_pixels));

  status=MagickTrue;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    status=MagickFalse;

    count=ReadBlob(image,(size_t) sizes[y],compact_pixels);
    if (count != (ssize_t) sizes[y])
      break;

    count=DecodePSDPixels((size_t) sizes[y],compact_pixels,
      (ssize_t) (image->depth == 1 ? 123456 : image->depth),row_size,pixels);
    if (count != (ssize_t) row_size)
      break;

    status=ReadPSDChannelPixels(image,psd_info->channels,y,type,pixels,
      exception);
    if (status == MagickFalse)
      break;
  }

  compact_pixels=(unsigned char *) RelinquishMagickMemory(compact_pixels);
  pixels=(unsigned char *) RelinquishMagickMemory(pixels);
  return(status);
}
