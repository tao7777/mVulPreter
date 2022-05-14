static Image *ReadRGFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  MagickBooleanType
    status;

  register IndexPacket
    *indexes;

  register PixelPacket
    *q;

  register ssize_t
    i,
    x;

  register unsigned char
    *p;

  size_t
    bit,
    byte;

  ssize_t
    y;

  unsigned char
    *data;


  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AcquireImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Read RGF header.
  */
  image->columns = (unsigned long) ReadBlobByte(image);
  image->rows = (unsigned long) ReadBlobByte(image);
  image->depth=8;
  image->storage_class=PseudoClass;
  image->colors=2;
  /*
    Initialize image structure.
  */
  if (AcquireImageColormap(image,image->colors) == MagickFalse)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  /*
    Initialize colormap.
  */
  image->colormap[0].red=QuantumRange;
  image->colormap[0].green=QuantumRange;
  image->colormap[0].blue=QuantumRange;
  image->colormap[1].red=(Quantum) 0;
  image->colormap[1].green=(Quantum) 0;
  image->colormap[1].blue=(Quantum) 0;
  if (image_info->ping != MagickFalse)
    {
       (void) CloseBlob(image);
       return(GetFirstImageInList(image));
     }
  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }
   /*
     Read hex image data.
   */
  data=(unsigned char *) AcquireQuantumMemory(image->rows,image->columns*
    sizeof(*data));
  if (data == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  p=data;
  for (i=0; i < (ssize_t) (image->columns * image->rows); i++) 
    {
      *p++=ReadBlobByte(image);
    }

  /*
    Convert RGF image to pixel packets.
  */
  p=data;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetAuthenticIndexQueue(image);
    bit=0;
    byte=0;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if (bit == 0)
        byte=(size_t) (*p++);
      SetPixelIndex(indexes+x,(Quantum) ((byte & 0x01) != 0 ? 0x01 : 0x00));
      bit++;
      byte>>=1;
      if (bit == 8)
        bit=0;
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
    status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  data=(unsigned char *) RelinquishMagickMemory(data);
  (void) SyncImage(image);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
