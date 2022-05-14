static Image *ReadRLAImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  typedef struct _WindowFrame
  {
    short
      left,
      right,
      bottom,
      top;
  } WindowFrame;

  typedef struct _RLAInfo
  {
    WindowFrame
      window,
      active_window;

    short
      frame,
      storage_type,
      number_channels,
      number_matte_channels,
      number_auxiliary_channels,
      revision;

    char
      gamma[16],
      red_primary[24],
      green_primary[24],
      blue_primary[24],
      white_point[24];

    ssize_t
      job_number;

    char
      name[128],
      description[128],
      program[64],
      machine[32],
      user[32],
      date[20],
      aspect[24],
      aspect_ratio[8],
      chan[32];

    short
      field;

    char
      time[12],
      filter[32];

    short
      bits_per_channel,
      matte_type,
      matte_bits,
      auxiliary_type,
      auxiliary_bits;

    char
      auxiliary[32],
      space[36];

    ssize_t
      next;
  } RLAInfo;

  Image
    *image;

  int
    channel,
    length,
    runlength;

  MagickBooleanType
    status;

  MagickOffsetType
    offset;

  register ssize_t
    i,
    x;

  register PixelPacket
    *q;

  ssize_t
    count,
    *scanlines,
    y;

  RLAInfo
    rla_info;

  unsigned char
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
  rla_info.window.left=(short) ReadBlobMSBShort(image);
  rla_info.window.right=(short) ReadBlobMSBShort(image);
  rla_info.window.bottom=(short) ReadBlobMSBShort(image);
  rla_info.window.top=(short) ReadBlobMSBShort(image);
  rla_info.active_window.left=(short) ReadBlobMSBShort(image);
  rla_info.active_window.right=(short) ReadBlobMSBShort(image);
  rla_info.active_window.bottom=(short) ReadBlobMSBShort(image);
  rla_info.active_window.top=(short) ReadBlobMSBShort(image);
  rla_info.frame=(short) ReadBlobMSBShort(image);
  rla_info.storage_type=(short) ReadBlobMSBShort(image);
  rla_info.number_channels=(short) ReadBlobMSBShort(image);
  rla_info.number_matte_channels=(short) ReadBlobMSBShort(image);
  if (rla_info.number_channels == 0)
    rla_info.number_channels=3;
  rla_info.number_channels+=rla_info.number_matte_channels;
  rla_info.number_auxiliary_channels=(short) ReadBlobMSBShort(image);
  rla_info.revision=(short) ReadBlobMSBShort(image);
  count=ReadBlob(image,16,(unsigned char *) rla_info.gamma);
  count=ReadBlob(image,24,(unsigned char *) rla_info.red_primary);
  count=ReadBlob(image,24,(unsigned char *) rla_info.green_primary);
  count=ReadBlob(image,24,(unsigned char *) rla_info.blue_primary);
  count=ReadBlob(image,24,(unsigned char *) rla_info.white_point);
  rla_info.job_number=(int) ReadBlobMSBLong(image);
  count=ReadBlob(image,128,(unsigned char *) rla_info.name);
  count=ReadBlob(image,128,(unsigned char *) rla_info.description);
  rla_info.description[127]='\0';
  count=ReadBlob(image,64,(unsigned char *) rla_info.program);
  count=ReadBlob(image,32,(unsigned char *) rla_info.machine);
  count=ReadBlob(image,32,(unsigned char *) rla_info.user);
  count=ReadBlob(image,20,(unsigned char *) rla_info.date);
  count=ReadBlob(image,24,(unsigned char *) rla_info.aspect);
  count=ReadBlob(image,8,(unsigned char *) rla_info.aspect_ratio);
  count=ReadBlob(image,32,(unsigned char *) rla_info.chan);
  rla_info.field=(short) ReadBlobMSBShort(image);
  count=ReadBlob(image,12,(unsigned char *) rla_info.time);
  count=ReadBlob(image,32,(unsigned char *) rla_info.filter);
  rla_info.bits_per_channel=(short) ReadBlobMSBShort(image);
  rla_info.matte_type=(short) ReadBlobMSBShort(image);
  rla_info.matte_bits=(short) ReadBlobMSBShort(image);
  rla_info.auxiliary_type=(short) ReadBlobMSBShort(image);
  rla_info.auxiliary_bits=(short) ReadBlobMSBShort(image);
  count=ReadBlob(image,32,(unsigned char *) rla_info.auxiliary);
  count=ReadBlob(image,36,(unsigned char *) rla_info.space);
  if ((size_t) count != 36)
    ThrowReaderException(CorruptImageError,"UnableToReadImageData");
  rla_info.next=(int) ReadBlobMSBLong(image);
  /*
    Initialize image structure.
  */
  image->matte=rla_info.number_matte_channels != 0 ? MagickTrue : MagickFalse;
  image->columns=1UL*rla_info.active_window.right-rla_info.active_window.left+1;
  image->rows=1UL*rla_info.active_window.top-rla_info.active_window.bottom+1;
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
   scanlines=(ssize_t *) AcquireQuantumMemory(image->rows,sizeof(*scanlines));
   if (scanlines == (ssize_t *) NULL)
     ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  if (*rla_info.description != '\0')
    (void) SetImageProperty(image,"comment",(char *) rla_info.description);
  /*
    Read offsets to each scanline data.
  */
  for (i=0; i < (ssize_t) image->rows; i++)
    scanlines[i]=(int) ReadBlobMSBLong(image);
  /*
    Read image data.
  */
  x=0;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    offset=SeekBlob(image,scanlines[image->rows-y-1],SEEK_SET);
    if (offset < 0)
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    for (channel=0; channel < (int) rla_info.number_channels; channel++)
    {
      length=(int) ReadBlobMSBShort(image);
      while (length > 0)
      {
        byte=(unsigned char) ReadBlobByte(image);
        runlength=byte;
        if (byte > 127)
          runlength=byte-256;
        length--;
        if (length == 0)
          break;
        if (runlength < 0)
          {
            while (runlength < 0)
            {
              q=GetAuthenticPixels(image,(ssize_t) (x % image->columns),
                (ssize_t) (y % image->rows),1,1,exception);
              if (q == (PixelPacket *) NULL)
                break;
              byte=(unsigned char) ReadBlobByte(image);
              length--;
              switch (channel)
              {
                case 0:
                {
                  SetPixelRed(q,ScaleCharToQuantum(byte));
                  break;
                }
                case 1:
                {
                  SetPixelGreen(q,ScaleCharToQuantum(byte));
                  break;
                }
                case 2:
                {
                  SetPixelBlue(q,ScaleCharToQuantum(byte));
                  break;
                }
                case 3:
                default:
                {
                  SetPixelAlpha(q,ScaleCharToQuantum(byte));
                  break;
                }
              }
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
              x++;
              runlength++;
            }
            continue;
          }
        byte=(unsigned char) ReadBlobByte(image);
        length--;
        runlength++;
        do
        {
          q=GetAuthenticPixels(image,(ssize_t) (x % image->columns),
            (ssize_t) (y % image->rows),1,1,exception);
          if (q == (PixelPacket *) NULL)
            break;
          switch (channel)
          {
            case 0:
            {
              SetPixelRed(q,ScaleCharToQuantum(byte));
              break;
            }
            case 1:
            {
              SetPixelGreen(q,ScaleCharToQuantum(byte));
              break;
            }
            case 2:
            {
              SetPixelBlue(q,ScaleCharToQuantum(byte));
              break;
            }
            case 3:
            default:
            {
              SetPixelAlpha(q,ScaleCharToQuantum(byte));
              break;
            }
          }
          if (SyncAuthenticPixels(image,exception) == MagickFalse)
            break;
          x++;
          runlength--;
        }
        while (runlength > 0);
      }
    }
    status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  if (EOFBlob(image) != MagickFalse)
    ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
      image->filename);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
