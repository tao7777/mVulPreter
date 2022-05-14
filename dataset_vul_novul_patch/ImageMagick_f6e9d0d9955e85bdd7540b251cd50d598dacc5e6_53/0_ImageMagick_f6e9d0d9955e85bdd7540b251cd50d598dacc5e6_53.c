static Image *ReadRLEImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define SkipLinesOp  0x01
#define SetColorOp  0x02
#define SkipPixelsOp  0x03
#define ByteDataOp  0x05
#define RunDataOp  0x06
#define EOFOp  0x07

  char
    magick[12];

  Image
    *image;

  int
    opcode,
    operand,
    status;

  MagickStatusType
    flags;

  MagickSizeType
    number_pixels;

  MemoryInfo
    *pixel_info;

  register IndexPacket
    *indexes;

  register ssize_t
    x;

  register PixelPacket
    *q;

  register ssize_t
    i;

  register unsigned char
    *p;

  size_t
    bits_per_pixel,
    map_length,
    number_colormaps,
    number_planes,
    one;

  ssize_t
    count,
    y;

  unsigned char
    background_color[256],
    *colormap,
    pixel,
    plane,
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
    Determine if this a RLE file.
  */
  count=ReadBlob(image,2,(unsigned char *) magick);
  if ((count == 0) || (memcmp(magick,"\122\314",2) != 0))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  do
  {
    /*
      Read image header.
    */
    (void) ReadBlobLSBShort(image);
    (void) ReadBlobLSBShort(image);
    image->columns=ReadBlobLSBShort(image);
    image->rows=ReadBlobLSBShort(image);
    flags=(MagickStatusType) ReadBlobByte(image);
    image->matte=flags & 0x04 ? MagickTrue : MagickFalse;
    number_planes=1UL*ReadBlobByte(image);
    bits_per_pixel=1UL*ReadBlobByte(image);
    number_colormaps=1UL*ReadBlobByte(image);
    map_length=(unsigned char) ReadBlobByte(image);
    if (map_length >= 64)
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    one=1;
    map_length=one << map_length;
    if ((number_planes == 0) || (number_planes == 2) || (bits_per_pixel != 8) ||
        (image->columns == 0))
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    if (flags & 0x02)
      {
        /*
          No background color-- initialize to black.
        */
        for (i=0; i < (ssize_t) number_planes; i++)
          background_color[i]=0;
        (void) ReadBlobByte(image);
      }
    else
      {
        /*
          Initialize background color.
        */
        p=background_color;
        for (i=0; i < (ssize_t) number_planes; i++)
          *p++=(unsigned char) ReadBlobByte(image);
      }
    if ((number_planes & 0x01) == 0)
      (void) ReadBlobByte(image);
    colormap=(unsigned char *) NULL;
    if (number_colormaps != 0)
      {
        /*
          Read image colormaps.
        */
        colormap=(unsigned char *) AcquireQuantumMemory(number_colormaps,
          map_length*sizeof(*colormap));
        if (colormap == (unsigned char *) NULL)
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        p=colormap;
        for (i=0; i < (ssize_t) number_colormaps; i++)
          for (x=0; x < (ssize_t) map_length; x++)
            *p++=(unsigned char) ScaleShortToQuantum(ReadBlobLSBShort(image));
      }
    if ((flags & 0x08) != 0)
      {
        char
          *comment;

        size_t
          length;

        /*
          Read image comment.
        */
        length=ReadBlobLSBShort(image);
        if (length != 0)
          {
            comment=(char *) AcquireQuantumMemory(length,sizeof(*comment));
            if (comment == (char *) NULL)
              ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
            count=ReadBlob(image,length-1,(unsigned char *) comment);
            comment[length-1]='\0';
            (void) SetImageProperty(image,"comment",comment);
            comment=DestroyString(comment);
            if ((length & 0x01) == 0)
              (void) ReadBlobByte(image);
          }
      }
     if ((image_info->ping != MagickFalse) && (image_info->number_scenes != 0))
       if (image->scene >= (image_info->scene+image_info->number_scenes-1))
         break;
    status=SetImageExtent(image,image->columns,image->rows);
    if (status == MagickFalse)
      {
        InheritException(exception,&image->exception);
        return(DestroyImageList(image));
      }
     /*
       Allocate RLE pixels.
     */
    if (image->matte != MagickFalse)
      number_planes++;
    number_pixels=(MagickSizeType) image->columns*image->rows;
    if ((number_pixels*number_planes) != (size_t) (number_pixels*number_planes))
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    pixel_info=AcquireVirtualMemory(image->columns,image->rows*number_planes*
      sizeof(*pixels));
    if (pixel_info == (MemoryInfo *) NULL)
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
    if ((flags & 0x01) && !(flags & 0x02))
      {
        ssize_t
          j;

        /*
          Set background color.
        */
        p=pixels;
        for (i=0; i < (ssize_t) number_pixels; i++)
        {
          if (image->matte == MagickFalse)
            for (j=0; j < (ssize_t) number_planes; j++)
              *p++=background_color[j];
          else
            {
              for (j=0; j < (ssize_t) (number_planes-1); j++)
                *p++=background_color[j];
              *p++=0;  /* initialize matte channel */
            }
        }
      }
    /*
      Read runlength-encoded image.
    */
    plane=0;
    x=0;
    y=0;
    opcode=ReadBlobByte(image);
    do
    {
      switch (opcode & 0x3f)
      {
        case SkipLinesOp:
        {
          operand=ReadBlobByte(image);
          if (opcode & 0x40)
            operand=(int) ReadBlobLSBShort(image);
          x=0;
          y+=operand;
          break;
        }
        case SetColorOp:
        {
          operand=ReadBlobByte(image);
          plane=(unsigned char) operand;
          if (plane == 255)
            plane=(unsigned char) (number_planes-1);
          x=0;
          break;
        }
        case SkipPixelsOp:
        {
          operand=ReadBlobByte(image);
          if (opcode & 0x40)
            operand=(int) ReadBlobLSBShort(image);
          x+=operand;
          break;
        }
        case ByteDataOp:
        {
          operand=ReadBlobByte(image);
          if (opcode & 0x40)
            operand=(int) ReadBlobLSBShort(image);
          p=pixels+((image->rows-y-1)*image->columns*number_planes)+
            x*number_planes+plane;
          operand++;
          for (i=0; i < (ssize_t) operand; i++)
          {
            pixel=(unsigned char) ReadBlobByte(image);
            if ((y < (ssize_t) image->rows) &&
                ((x+i) < (ssize_t) image->columns))
              *p=pixel;
            p+=number_planes;
          }
          if (operand & 0x01)
            (void) ReadBlobByte(image);
          x+=operand;
          break;
        }
        case RunDataOp:
        {
          operand=ReadBlobByte(image);
          if (opcode & 0x40)
            operand=(int) ReadBlobLSBShort(image);
          pixel=(unsigned char) ReadBlobByte(image);
          (void) ReadBlobByte(image);
          operand++;
          p=pixels+((image->rows-y-1)*image->columns*number_planes)+
            x*number_planes+plane;
          for (i=0; i < (ssize_t) operand; i++)
          {
            if ((y < (ssize_t) image->rows) &&
                ((x+i) < (ssize_t) image->columns))
              *p=pixel;
            p+=number_planes;
          }
          x+=operand;
          break;
        }
        default:
          break;
      }
      opcode=ReadBlobByte(image);
    } while (((opcode & 0x3f) != EOFOp) && (opcode != EOF));
    if (number_colormaps != 0)
      {
        MagickStatusType
          mask;

        /*
          Apply colormap affineation to image.
        */
        mask=(MagickStatusType) (map_length-1);
        p=pixels;
        if (number_colormaps == 1)
          for (i=0; i < (ssize_t) number_pixels; i++)
          {
            *p=colormap[*p & mask];
            p++;
          }
        else
          if ((number_planes >= 3) && (number_colormaps >= 3))
            for (i=0; i < (ssize_t) number_pixels; i++)
              for (x=0; x < (ssize_t) number_planes; x++)
              {
                *p=colormap[x*map_length+(*p & mask)];
                p++;
              }
      }
    /*
      Initialize image structure.
    */
    if (number_planes >= 3)
      {
        /*
          Convert raster image to DirectClass pixel packets.
        */
        p=pixels;
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            SetPixelRed(q,ScaleCharToQuantum(*p++));
            SetPixelGreen(q,ScaleCharToQuantum(*p++));
            SetPixelBlue(q,ScaleCharToQuantum(*p++));
            if (image->matte != MagickFalse)
              SetPixelAlpha(q,ScaleCharToQuantum(*p++));
            q++;
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
      }
    else
      {
        /*
          Create colormap.
        */
        if (number_colormaps == 0)
          map_length=256;
        if (AcquireImageColormap(image,map_length) == MagickFalse)
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        p=colormap;
        if (number_colormaps == 1)
          for (i=0; i < (ssize_t) image->colors; i++)
          {
            /*
              Pseudocolor.
            */
            image->colormap[i].red=ScaleCharToQuantum((unsigned char) i);
            image->colormap[i].green=ScaleCharToQuantum((unsigned char) i);
            image->colormap[i].blue=ScaleCharToQuantum((unsigned char) i);
          }
        else
          if (number_colormaps > 1)
            for (i=0; i < (ssize_t) image->colors; i++)
            {
              image->colormap[i].red=ScaleCharToQuantum(*p);
              image->colormap[i].green=ScaleCharToQuantum(*(p+map_length));
              image->colormap[i].blue=ScaleCharToQuantum(*(p+map_length*2));
              p++;
            }
        p=pixels;
        if (image->matte == MagickFalse)
          {
            /*
              Convert raster image to PseudoClass pixel packets.
            */
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=GetAuthenticIndexQueue(image);
              for (x=0; x < (ssize_t) image->columns; x++)
                SetPixelIndex(indexes+x,*p++);
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,LoadImageTag,(MagickOffsetType)
                    y,image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            (void) SyncImage(image);
          }
        else
          {
            /*
              Image has a matte channel-- promote to DirectClass.
            */
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
              if (q == (PixelPacket *) NULL)
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                SetPixelRed(q,image->colormap[*p++].red);
                SetPixelGreen(q,image->colormap[*p++].green);
                SetPixelBlue(q,image->colormap[*p++].blue);
                SetPixelAlpha(q,ScaleCharToQuantum(*p++));
                q++;
              }
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,LoadImageTag,(MagickOffsetType)
                    y,image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            image->colormap=(PixelPacket *) RelinquishMagickMemory(
              image->colormap);
            image->storage_class=DirectClass;
            image->colors=0;
          }
      }
    if (number_colormaps != 0)
      colormap=(unsigned char *) RelinquishMagickMemory(colormap);
    pixel_info=RelinquishVirtualMemory(pixel_info);
    if (EOFBlob(image) != MagickFalse)
      {
        ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
          image->filename);
        break;
      }
    /*
      Proceed to next image.
    */
    if (image_info->number_scenes != 0)
      if (image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    (void) ReadBlobByte(image);
    count=ReadBlob(image,2,(unsigned char *) magick);
    if ((count != 0) && (memcmp(magick,"\122\314",2) == 0))
      {
        /*
          Allocate next image structure.
        */
        AcquireNextImage(image_info,image);
        if (GetNextImageInList(image) == (Image *) NULL)
          {
            image=DestroyImageList(image);
            return((Image *) NULL);
          }
        image=SyncNextImageInList(image);
        status=SetImageProgress(image,LoadImagesTag,TellBlob(image),
          GetBlobSize(image));
        if (status == MagickFalse)
          break;
      }
  } while ((count != 0) && (memcmp(magick,"\122\314",2) == 0));
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
