static Image *ReadTGAImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  IndexPacket
    index;

  MagickBooleanType
    status;

  PixelPacket
    pixel;

  register IndexPacket
    *indexes;

  register PixelPacket
    *q;

  register ssize_t
    i,
    x;

  size_t
    base,
    flag,
    offset,
    real,
    skip;

  ssize_t
    count,
    y;

  TGAInfo
    tga_info;

  unsigned char
    j,
    k,
    pixels[4],
    runlength;

  unsigned int
    alpha_bits;

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
    Read TGA header information.
  */
  count=ReadBlob(image,1,&tga_info.id_length);
  tga_info.colormap_type=(unsigned char) ReadBlobByte(image);
  tga_info.image_type=(TGAImageType) ReadBlobByte(image);
  if ((count != 1) ||
      ((tga_info.image_type != TGAColormap) &&
       (tga_info.image_type != TGARGB) &&
       (tga_info.image_type != TGAMonochrome) &&
       (tga_info.image_type != TGARLEColormap) &&
       (tga_info.image_type != TGARLERGB) &&
       (tga_info.image_type != TGARLEMonochrome)) ||
      (((tga_info.image_type == TGAColormap) ||
       (tga_info.image_type == TGARLEColormap)) &&
       (tga_info.colormap_type == 0)))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  tga_info.colormap_index=ReadBlobLSBShort(image);
  tga_info.colormap_length=ReadBlobLSBShort(image);
  tga_info.colormap_size=(unsigned char) ReadBlobByte(image);
  tga_info.x_origin=ReadBlobLSBShort(image);
  tga_info.y_origin=ReadBlobLSBShort(image);
  tga_info.width=(unsigned short) ReadBlobLSBShort(image);
  tga_info.height=(unsigned short) ReadBlobLSBShort(image);
  tga_info.bits_per_pixel=(unsigned char) ReadBlobByte(image);
  tga_info.attributes=(unsigned char) ReadBlobByte(image);
  if (EOFBlob(image) != MagickFalse)
    ThrowReaderException(CorruptImageError,"UnableToReadImageData");
  if ((((tga_info.bits_per_pixel <= 1) || (tga_info.bits_per_pixel >= 17)) &&
       (tga_info.bits_per_pixel != 24) && (tga_info.bits_per_pixel != 32)))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  /*
    Initialize image structure.
  */
  image->columns=tga_info.width;
  image->rows=tga_info.height;
  alpha_bits=(tga_info.attributes & 0x0FU);
  image->matte=(alpha_bits > 0) || (tga_info.bits_per_pixel == 32) ||
    (tga_info.colormap_size == 32) ?  MagickTrue : MagickFalse;
  if ((tga_info.image_type != TGAColormap) &&
      (tga_info.image_type != TGARLEColormap))
    image->depth=(size_t) ((tga_info.bits_per_pixel <= 8) ? 8 :
      (tga_info.bits_per_pixel <= 16) ? 5 :
      (tga_info.bits_per_pixel == 24) ? 8 :
      (tga_info.bits_per_pixel == 32) ? 8 : 8);
  else
    image->depth=(size_t) ((tga_info.colormap_size <= 8) ? 8 :
      (tga_info.colormap_size <= 16) ? 5 :
      (tga_info.colormap_size == 24) ? 8 :
      (tga_info.colormap_size == 32) ? 8 : 8);
  if ((tga_info.image_type == TGAColormap) ||
      (tga_info.image_type == TGAMonochrome) ||
      (tga_info.image_type == TGARLEColormap) ||
      (tga_info.image_type == TGARLEMonochrome))
    image->storage_class=PseudoClass;
  image->compression=NoCompression;
  if ((tga_info.image_type == TGARLEColormap) ||
      (tga_info.image_type == TGARLEMonochrome))
    image->compression=RLECompression;
  if (image->storage_class == PseudoClass)
    {
      if (tga_info.colormap_type != 0)
        image->colors=tga_info.colormap_length;
      else
        {
          size_t
            one;

          one=1;
          image->colors=one << tga_info.bits_per_pixel;
          if (AcquireImageColormap(image,image->colors) == MagickFalse)
            ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        }
    }
  if (tga_info.id_length != 0)
    {
      char
        *comment;

      size_t
        length;

      /*
        TGA image comment.
      */
      length=(size_t) tga_info.id_length;
      comment=(char *) NULL;
      if (~length >= (MaxTextExtent-1))
        comment=(char *) AcquireQuantumMemory(length+MaxTextExtent,
          sizeof(*comment));
      if (comment == (char *) NULL)
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
      count=ReadBlob(image,tga_info.id_length,(unsigned char *) comment);
      comment[tga_info.id_length]='\0';
      (void) SetImageProperty(image,"comment",comment);
      comment=DestroyString(comment);
    }
  if (image_info->ping != MagickFalse)
    {
       (void) CloseBlob(image);
       return(image);
     }
  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }
   (void) ResetMagickMemory(&pixel,0,sizeof(pixel));
   pixel.opacity=(Quantum) OpaqueOpacity;
   if (tga_info.colormap_type != 0)
    {
      /*
        Read TGA raster colormap.
      */
      if (AcquireImageColormap(image,image->colors) == MagickFalse)
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
      for (i=0; i < (ssize_t) image->colors; i++)
      {
        switch (tga_info.colormap_size)
        {
          case 8:
          default:
          {
            /*
              Gray scale.
            */
            pixel.red=ScaleCharToQuantum((unsigned char) ReadBlobByte(image));
            pixel.green=pixel.red;
            pixel.blue=pixel.red;
            break;
          }
          case 15:
          case 16:
          {
            QuantumAny
              range;

            /*
              5 bits each of red green and blue.
            */
            j=(unsigned char) ReadBlobByte(image);
            k=(unsigned char) ReadBlobByte(image);
            range=GetQuantumRange(5UL);
            pixel.red=ScaleAnyToQuantum(1UL*(k & 0x7c) >> 2,range);
            pixel.green=ScaleAnyToQuantum((1UL*(k & 0x03) << 3)+
              (1UL*(j & 0xe0) >> 5),range);
            pixel.blue=ScaleAnyToQuantum(1UL*(j & 0x1f),range);
            break;
          }
          case 24:
          {
            /*
              8 bits each of blue, green and red.
            */
            pixel.blue=ScaleCharToQuantum((unsigned char) ReadBlobByte(image));
            pixel.green=ScaleCharToQuantum((unsigned char) ReadBlobByte(image));
            pixel.red=ScaleCharToQuantum((unsigned char) ReadBlobByte(image));
            break;
          }
          case 32:
          {
            /*
              8 bits each of blue, green, red, and alpha.
            */
            pixel.blue=ScaleCharToQuantum((unsigned char) ReadBlobByte(image));
            pixel.green=ScaleCharToQuantum((unsigned char) ReadBlobByte(image));
            pixel.red=ScaleCharToQuantum((unsigned char) ReadBlobByte(image));
            pixel.opacity=(Quantum) (QuantumRange-ScaleCharToQuantum(
              (unsigned char) ReadBlobByte(image)));
            break;
          }
        }
        image->colormap[i]=pixel;
      }
    }
  /*
    Convert TGA pixels to pixel packets.
  */
  base=0;
  flag=0;
  skip=MagickFalse;
  real=0;
  index=(IndexPacket) 0;
  runlength=0;
  offset=0;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    real=offset;
    if (((unsigned char) (tga_info.attributes & 0x20) >> 5) == 0)
      real=image->rows-real-1;
    q=QueueAuthenticPixels(image,0,(ssize_t) real,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetAuthenticIndexQueue(image);
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if ((tga_info.image_type == TGARLEColormap) ||
          (tga_info.image_type == TGARLERGB) ||
          (tga_info.image_type == TGARLEMonochrome))
        {
          if (runlength != 0)
            {
              runlength--;
              skip=flag != 0;
            }
          else
            {
              count=ReadBlob(image,1,&runlength);
              if (count == 0)
                ThrowReaderException(CorruptImageError,"UnableToReadImageData");
              flag=runlength & 0x80;
              if (flag != 0)
                runlength-=128;
              skip=MagickFalse;
            }
        }
      if (skip == MagickFalse)
        switch (tga_info.bits_per_pixel)
        {
          case 8:
          default:
          {
            /*
              Gray scale.
            */
            index=(IndexPacket) ReadBlobByte(image);
            if (tga_info.colormap_type != 0)
              pixel=image->colormap[(ssize_t) ConstrainColormapIndex(image,
                1UL*index)];
            else
              {
                pixel.red=ScaleCharToQuantum((unsigned char) index);
                pixel.green=ScaleCharToQuantum((unsigned char) index);
                pixel.blue=ScaleCharToQuantum((unsigned char) index);
              }
            break;
          }
          case 15:
          case 16:
          {
            QuantumAny
              range;

            /*
              5 bits each of RGB;
            */
            if (ReadBlob(image,2,pixels) != 2)
              ThrowReaderException(CorruptImageError,"UnableToReadImageData");
            j=pixels[0];
            k=pixels[1];
            range=GetQuantumRange(5UL);
            pixel.red=ScaleAnyToQuantum(1UL*(k & 0x7c) >> 2,range);
            pixel.green=ScaleAnyToQuantum((1UL*(k & 0x03) << 3)+
              (1UL*(j & 0xe0) >> 5),range);
            pixel.blue=ScaleAnyToQuantum(1UL*(j & 0x1f),range);
            if (image->matte != MagickFalse)
              pixel.opacity=(k & 0x80) == 0 ? (Quantum) OpaqueOpacity :
                (Quantum) TransparentOpacity;
            if (image->storage_class == PseudoClass)
              index=ConstrainColormapIndex(image,((size_t) k << 8)+j);
            break;
          }
          case 24:
          {
            /*
              BGR pixels.
            */
            if (ReadBlob(image,3,pixels) != 3)
              ThrowReaderException(CorruptImageError,"UnableToReadImageData");
            pixel.blue=ScaleCharToQuantum(pixels[0]);
            pixel.green=ScaleCharToQuantum(pixels[1]);
            pixel.red=ScaleCharToQuantum(pixels[2]);
            break;
          }
          case 32:
          {
            /*
              BGRA pixels.
            */
            if (ReadBlob(image,4,pixels) != 4)
              ThrowReaderException(CorruptImageError,"UnableToReadImageData");
            pixel.blue=ScaleCharToQuantum(pixels[0]);
            pixel.green=ScaleCharToQuantum(pixels[1]);
            pixel.red=ScaleCharToQuantum(pixels[2]);
            pixel.opacity=(Quantum) (QuantumRange-ScaleCharToQuantum(
              pixels[3]));
            break;
          }
        }
      if (status == MagickFalse)
        ThrowReaderException(CorruptImageError,"UnableToReadImageData");
      if (image->storage_class == PseudoClass)
        SetPixelIndex(indexes+x,index);
      SetPixelRed(q,pixel.red);
      SetPixelGreen(q,pixel.green);
      SetPixelBlue(q,pixel.blue);
      if (image->matte != MagickFalse)
        SetPixelOpacity(q,pixel.opacity);
      q++;
    }
    if (((tga_info.attributes & 0xc0) >> 6) == 4)
      offset+=4;
    else
      if (((tga_info.attributes & 0xc0) >> 6) == 2)
        offset+=2;
      else
        offset++;
    if (offset >= image->rows)
      {
        base++;
        offset=base;
      }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
    if (image->previous == (Image *) NULL)
      {
        status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
          image->rows);
        if (status == MagickFalse)
          break;
      }
  }
  if (EOFBlob(image) != MagickFalse)
    ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
      image->filename);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
