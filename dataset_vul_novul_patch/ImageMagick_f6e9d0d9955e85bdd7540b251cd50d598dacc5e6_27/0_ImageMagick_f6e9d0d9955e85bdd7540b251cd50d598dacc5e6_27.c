static Image *ReadJBIGImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  IndexPacket
    index;

  MagickStatusType
    status;

  register IndexPacket
    *indexes;

  register ssize_t
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

  ssize_t
    length,
    y;

  struct jbg_dec_state
    jbig_info;

  unsigned char
    bit,
    *buffer,
    byte;

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
    Initialize JBIG toolkit.
  */
  jbg_dec_init(&jbig_info);
  jbg_dec_maxsize(&jbig_info,(unsigned long) image->columns,(unsigned long)
    image->rows);
  image->columns=jbg_dec_getwidth(&jbig_info);
  image->rows=jbg_dec_getheight(&jbig_info);
  image->depth=8;
  image->storage_class=PseudoClass;
  image->colors=2;
  /*
    Read JBIG file.
  */
  buffer=(unsigned char *) AcquireQuantumMemory(MagickMaxBufferExtent,
    sizeof(*buffer));
  if (buffer == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  status=JBG_EAGAIN;
  do
  {
    length=(ssize_t) ReadBlob(image,MagickMaxBufferExtent,buffer);
    if (length == 0)
      break;
    p=buffer;
    while ((length > 0) && ((status == JBG_EAGAIN) || (status == JBG_EOK)))
    {
      size_t
        count;

      status=jbg_dec_in(&jbig_info,p,length,&count);
      p+=count;
      length-=(ssize_t) count;
    }
  } while ((status == JBG_EAGAIN) || (status == JBG_EOK));
  /*
    Create colormap.
  */
  image->columns=jbg_dec_getwidth(&jbig_info);
  image->rows=jbg_dec_getheight(&jbig_info);
  image->compression=JBIG2Compression;
  if (AcquireImageColormap(image,2) == MagickFalse)
    {
      buffer=(unsigned char *) RelinquishMagickMemory(buffer);
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    }
  image->colormap[0].red=0;
  image->colormap[0].green=0;
  image->colormap[0].blue=0;
  image->colormap[1].red=QuantumRange;
  image->colormap[1].green=QuantumRange;
  image->colormap[1].blue=QuantumRange;
  image->x_resolution=300;
  image->y_resolution=300;
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
     Convert X bitmap image to pixel packets.
   */
  p=jbg_dec_getimage(&jbig_info,0);
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
        byte=(*p++);
      index=(byte & 0x80) ? 0 : 1;
      bit++;
      byte<<=1;
      if (bit == 8)
        bit=0;
      SetPixelIndex(indexes+x,index);
      SetPixelRGBO(q,image->colormap+(ssize_t) index);
      q++;
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
    status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  /*
    Free scale resource.
  */
  jbg_dec_free(&jbig_info);
  buffer=(unsigned char *) RelinquishMagickMemory(buffer);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
