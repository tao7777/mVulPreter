static Image *ReadMACImage(const ImageInfo *image_info,ExceptionInfo *exception)
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
    x;

  register unsigned char
    *p;

  size_t
    length;

  ssize_t
    offset,
    y;

  unsigned char
    count,
    bit,
    byte,
    *pixels;

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
    Read MAC X image.
  */
  length=ReadBlobLSBShort(image);
  if ((length & 0xff) != 0)
    ThrowReaderException(CorruptImageError,"CorruptImage");
  for (x=0; x < (ssize_t) 638; x++)
    if (ReadBlobByte(image) == EOF)
      ThrowReaderException(CorruptImageError,"CorruptImage");
  image->columns=576;
  image->rows=720;
  image->depth=1;
  if (AcquireImageColormap(image,2) == MagickFalse)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
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
     Convert MAC raster image to pixel packets.
   */
  length=(image->columns+7)/8;
  pixels=(unsigned char *) AcquireQuantumMemory(length+1,sizeof(*pixels));
  if (pixels == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  p=pixels;
  offset=0;
  for (y=0; y < (ssize_t) image->rows; )
  {
    count=(unsigned char) ReadBlobByte(image);
    if (EOFBlob(image) != MagickFalse)
      break;
    if ((count <= 0) || (count >= 128))
      {
        byte=(unsigned char) (~ReadBlobByte(image));
        count=(~count)+2;
        while (count != 0)
        {
          *p++=byte;
          offset++;
          count--;
          if (offset >= (ssize_t) length)
            {
              q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=GetAuthenticIndexQueue(image);
              p=pixels;
              bit=0;
              byte=0;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                if (bit == 0)
                  byte=(*p++);
                SetPixelIndex(indexes+x,((byte & 0x80) != 0 ? 0x01 : 0x00));
                bit++;
                byte<<=1;
                if (bit == 8)
                  bit=0;
              }
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
              offset=0;
              p=pixels;
              y++;
            }
        }
        continue;
      }
    count++;
    while (count != 0)
    {
      byte=(unsigned char) (~ReadBlobByte(image));
      *p++=byte;
      offset++;
      count--;
      if (offset >= (ssize_t) length)
        {
          q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetAuthenticIndexQueue(image);
          p=pixels;
          bit=0;
          byte=0;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            if (bit == 0)
              byte=(*p++);
            SetPixelIndex(indexes+x,((byte & 0x80) != 0 ?  0x01 : 0x00));
            bit++;
            byte<<=1;
            if (bit == 8)
              bit=0;
          }
          if (SyncAuthenticPixels(image,exception) == MagickFalse)
            break;
          offset=0;
          p=pixels;
          y++;
        }
    }
  }
  pixels=(unsigned char *) RelinquishMagickMemory(pixels);
  (void) SyncImage(image);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
