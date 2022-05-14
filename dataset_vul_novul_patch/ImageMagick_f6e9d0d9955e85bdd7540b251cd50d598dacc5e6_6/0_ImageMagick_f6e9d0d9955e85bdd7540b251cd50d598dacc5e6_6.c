static Image *ReadCMYKImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *canvas_image,
    *image;

  MagickBooleanType
    status;

  MagickOffsetType
    scene;

  QuantumInfo
    *quantum_info;

  QuantumType
    quantum_type;

  register ssize_t
    i;

  size_t
    length;

  ssize_t
    count,
    y;

  unsigned char
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
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionError,"MustSpecifyImageSize");
  SetImageColorspace(image,CMYKColorspace);
  if (image_info->interlace != PartitionInterlace)
    {
      status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
      if (status == MagickFalse)
        {
          image=DestroyImageList(image);
          return((Image *) NULL);
        }
      if (DiscardBlobBytes(image,image->offset) == MagickFalse)
        ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
          image->filename);
    }
  /*
    Create virtual canvas to support cropping (i.e. image.cmyk[100x100+10+20]).
  */
  canvas_image=CloneImage(image,image->extract_info.width,1,MagickFalse,
    exception);
  (void) SetImageVirtualPixelMethod(canvas_image,BlackVirtualPixelMethod);
  quantum_info=AcquireQuantumInfo(image_info,canvas_image);
  if (quantum_info == (QuantumInfo *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  pixels=GetQuantumPixels(quantum_info);
  quantum_type=CMYKQuantum;
  if (LocaleCompare(image_info->magick,"CMYKA") == 0)
    {
      quantum_type=CMYKAQuantum;
      image->matte=MagickTrue;
    }
  if (image_info->number_scenes != 0)
    while (image->scene < image_info->scene)
    {
      /*
        Skip to next image.
      */
      image->scene++;
      length=GetQuantumExtent(canvas_image,quantum_info,quantum_type);
      for (y=0; y < (ssize_t) image->rows; y++)
      {
        count=ReadBlob(image,length,pixels);
        if (count != (ssize_t) length)
          break;
      }
    }
  count=0;
  length=0;
  scene=0;
  do
  {
    /*
      Read pixels to virtual canvas image then push to image.
    */
     if ((image_info->ping != MagickFalse) && (image_info->number_scenes != 0))
       if (image->scene >= (image_info->scene+image_info->number_scenes-1))
         break;
    status=SetImageExtent(image,image->columns,image->rows);
    if (status == MagickFalse)
      {
        InheritException(exception,&image->exception);
        return(DestroyImageList(image));
      }
     SetImageColorspace(image,CMYKColorspace);
     switch (image_info->interlace)
     {
      case NoInterlace:
      default:
      {
        /*
          No interlacing:  CMYKCMYKCMYKCMYKCMYKCMYK...
        */
        if (scene == 0)
          {
            length=GetQuantumExtent(canvas_image,quantum_info,quantum_type);
            count=ReadBlob(image,length,pixels);
          }
        for (y=0; y < (ssize_t) image->extract_info.height; y++)
        {
          register const IndexPacket
            *restrict canvas_indexes;

          register const PixelPacket
            *restrict p;

          register IndexPacket
            *restrict indexes;

          register PixelPacket
            *restrict q;

          register ssize_t
            x;

          if (count != (ssize_t) length)
            {
              ThrowFileException(exception,CorruptImageError,
                "UnexpectedEndOfFile",image->filename);
              break;
            }
          q=GetAuthenticPixels(canvas_image,0,0,canvas_image->columns,1,
            exception);
          if (q == (PixelPacket *) NULL)
            break;
          length=ImportQuantumPixels(canvas_image,(CacheView *) NULL,
            quantum_info,quantum_type,pixels,exception);
          if (SyncAuthenticPixels(canvas_image,exception) == MagickFalse)
            break;
          if (((y-image->extract_info.y) >= 0) &&
              ((y-image->extract_info.y) < (ssize_t) image->rows))
            {
              p=GetVirtualPixels(canvas_image,canvas_image->extract_info.x,0,
                canvas_image->columns,1,exception);
              q=QueueAuthenticPixels(image,0,y-image->extract_info.y,
                image->columns,1,exception);
              if ((p == (const PixelPacket *) NULL) ||
                  (q == (PixelPacket *) NULL))
                break;
              canvas_indexes=GetVirtualIndexQueue(canvas_image);
              indexes=GetAuthenticIndexQueue(image);
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                SetPixelRed(q,GetPixelRed(p));
                SetPixelGreen(q,GetPixelGreen(p));
                SetPixelBlue(q,GetPixelBlue(p));
                SetPixelBlack(indexes+x,GetPixelBlack(
                  canvas_indexes+image->extract_info.x+x));
                SetPixelOpacity(q,OpaqueOpacity);
                if (image->matte != MagickFalse)
                  SetPixelOpacity(q,GetPixelOpacity(p));
                p++;
                q++;
              }
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
            }
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
          count=ReadBlob(image,length,pixels);
        }
        break;
      }
      case LineInterlace:
      {
        static QuantumType
          quantum_types[5] =
          {
            CyanQuantum,
            MagentaQuantum,
            YellowQuantum,
            BlackQuantum,
            OpacityQuantum
          };

        /*
          Line interlacing:  CCC...MMM...YYY...KKK...CCC...MMM...YYY...KKK...
        */
        if (scene == 0)
          {
            length=GetQuantumExtent(canvas_image,quantum_info,CyanQuantum);
            count=ReadBlob(image,length,pixels);
          }
        for (y=0; y < (ssize_t) image->extract_info.height; y++)
        {
          register const IndexPacket
            *restrict canvas_indexes;

          register const PixelPacket
            *restrict p;

          register IndexPacket
            *restrict indexes;

          register PixelPacket
            *restrict q;

          register ssize_t
            x;

          if (count != (ssize_t) length)
            {
              ThrowFileException(exception,CorruptImageError,
                "UnexpectedEndOfFile",image->filename);
              break;
            }
          for (i=0; i < (image->matte != MagickFalse ? 5 : 4); i++)
          {
            quantum_type=quantum_types[i];
            q=GetAuthenticPixels(canvas_image,0,0,canvas_image->columns,1,
              exception);
            if (q == (PixelPacket *) NULL)
              break;
            length=ImportQuantumPixels(canvas_image,(CacheView *) NULL,
              quantum_info,quantum_type,pixels,exception);
            if (SyncAuthenticPixels(canvas_image,exception) == MagickFalse)
              break;
            if (((y-image->extract_info.y) >= 0) &&
                ((y-image->extract_info.y) < (ssize_t) image->rows))
              {
                p=GetVirtualPixels(canvas_image,canvas_image->extract_info.x,
                  0,canvas_image->columns,1,exception);
                q=GetAuthenticPixels(image,0,y-image->extract_info.y,
                  image->columns,1,exception);
                if ((p == (const PixelPacket *) NULL) ||
                    (q == (PixelPacket *) NULL))
                  break;
                canvas_indexes=GetVirtualIndexQueue(canvas_image);
                indexes=GetAuthenticIndexQueue(image);
                for (x=0; x < (ssize_t) image->columns; x++)
                {
                  switch (quantum_type)
                  {
                    case CyanQuantum:
                    {
                      SetPixelCyan(q,GetPixelCyan(p));
                      break;
                    }
                    case MagentaQuantum:
                    {
                      SetPixelMagenta(q,GetPixelMagenta(p));
                      break;
                    }
                    case YellowQuantum:
                    {
                      SetPixelYellow(q,GetPixelYellow(p));
                      break;
                    }
                    case BlackQuantum:
                    {
                      SetPixelIndex(indexes+x,GetPixelIndex(
                        canvas_indexes+image->extract_info.x+x));
                      break;
                    }
                    case OpacityQuantum:
                    {
                      SetPixelOpacity(q,GetPixelOpacity(p));
                      break;
                    }
                    default:
                      break;
                  }
                  p++;
                  q++;
                }
                if (SyncAuthenticPixels(image,exception) == MagickFalse)
                  break;
              }
            count=ReadBlob(image,length,pixels);
          }
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        break;
      }
      case PlaneInterlace:
      {
        /*
          Plane interlacing:  CCCCCC...MMMMMM...YYYYYY...KKKKKK...
        */
        if (scene == 0)
          {
            length=GetQuantumExtent(canvas_image,quantum_info,CyanQuantum);
            count=ReadBlob(image,length,pixels);
          }
        for (y=0; y < (ssize_t) image->extract_info.height; y++)
        {
          register const PixelPacket
            *restrict p;

          register PixelPacket
            *restrict q;

          register ssize_t
            x;

          if (count != (ssize_t) length)
            {
              ThrowFileException(exception,CorruptImageError,
                "UnexpectedEndOfFile",image->filename);
              break;
            }
          q=GetAuthenticPixels(canvas_image,0,0,canvas_image->columns,1,
            exception);
          if (q == (PixelPacket *) NULL)
            break;
          length=ImportQuantumPixels(canvas_image,(CacheView *) NULL,
            quantum_info,CyanQuantum,pixels,exception);
          if (SyncAuthenticPixels(canvas_image,exception) == MagickFalse)
            break;
          if (((y-image->extract_info.y) >= 0) &&
              ((y-image->extract_info.y) < (ssize_t) image->rows))
            {
              p=GetVirtualPixels(canvas_image,canvas_image->extract_info.x,0,
                canvas_image->columns,1,exception);
              q=GetAuthenticPixels(image,0,y-image->extract_info.y,
                image->columns,1,exception);
              if ((p == (const PixelPacket *) NULL) ||
                  (q == (PixelPacket *) NULL))
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                SetPixelRed(q,GetPixelRed(p));
                p++;
                q++;
              }
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
            }
          count=ReadBlob(image,length,pixels);
        }
        if (image->previous == (Image *) NULL)
          {
            status=SetImageProgress(image,LoadImageTag,1,6);
            if (status == MagickFalse)
              break;
          }
        for (y=0; y < (ssize_t) image->extract_info.height; y++)
        {
          register const PixelPacket
            *restrict p;

          register PixelPacket
            *restrict q;

          register ssize_t
            x;

          if (count != (ssize_t) length)
            {
              ThrowFileException(exception,CorruptImageError,
                "UnexpectedEndOfFile",image->filename);
              break;
            }
          q=GetAuthenticPixels(canvas_image,0,0,canvas_image->columns,1,
            exception);
          if (q == (PixelPacket *) NULL)
            break;
          length=ImportQuantumPixels(canvas_image,(CacheView *) NULL,
            quantum_info,MagentaQuantum,pixels,exception);
          if (SyncAuthenticPixels(canvas_image,exception) == MagickFalse)
            break;
          if (((y-image->extract_info.y) >= 0) &&
              ((y-image->extract_info.y) < (ssize_t) image->rows))
            {
              p=GetVirtualPixels(canvas_image,canvas_image->extract_info.x,0,
                canvas_image->columns,1,exception);
              q=GetAuthenticPixels(image,0,y-image->extract_info.y,
                image->columns,1,exception);
              if ((p == (const PixelPacket *) NULL) ||
                  (q == (PixelPacket *) NULL))
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                SetPixelGreen(q,GetPixelGreen(p));
                p++;
                q++;
              }
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
           }
          count=ReadBlob(image,length,pixels);
        }
        if (image->previous == (Image *) NULL)
          {
            status=SetImageProgress(image,LoadImageTag,2,6);
            if (status == MagickFalse)
              break;
          }
        for (y=0; y < (ssize_t) image->extract_info.height; y++)
        {
          register const PixelPacket
            *restrict p;

          register PixelPacket
            *restrict q;

          register ssize_t
            x;

          if (count != (ssize_t) length)
            {
              ThrowFileException(exception,CorruptImageError,
                "UnexpectedEndOfFile",image->filename);
              break;
            }
          q=GetAuthenticPixels(canvas_image,0,0,canvas_image->columns,1,
            exception);
          if (q == (PixelPacket *) NULL)
            break;
          length=ImportQuantumPixels(canvas_image,(CacheView *) NULL,
            quantum_info,YellowQuantum,pixels,exception);
          if (SyncAuthenticPixels(canvas_image,exception) == MagickFalse)
            break;
          if (((y-image->extract_info.y) >= 0) &&
              ((y-image->extract_info.y) < (ssize_t) image->rows))
            {
              p=GetVirtualPixels(canvas_image,canvas_image->extract_info.x,0,
                canvas_image->columns,1,exception);
              q=GetAuthenticPixels(image,0,y-image->extract_info.y,
                image->columns,1,exception);
              if ((p == (const PixelPacket *) NULL) ||
                  (q == (PixelPacket *) NULL))
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                SetPixelBlue(q,GetPixelBlue(p));
                p++;
                q++;
              }
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
            }
          count=ReadBlob(image,length,pixels);
        }
        if (image->previous == (Image *) NULL)
          {
            status=SetImageProgress(image,LoadImageTag,3,6);
            if (status == MagickFalse)
              break;
          }
        for (y=0; y < (ssize_t) image->extract_info.height; y++)
        {
          register const IndexPacket
            *restrict canvas_indexes;

          register const PixelPacket
            *restrict p;

          register IndexPacket
            *restrict indexes;

          register PixelPacket
            *restrict q;

          register ssize_t
            x;

          if (count != (ssize_t) length)
            {
              ThrowFileException(exception,CorruptImageError,
                "UnexpectedEndOfFile",image->filename);
              break;
            }
          q=GetAuthenticPixels(canvas_image,0,0,canvas_image->columns,1,
            exception);
          if (q == (PixelPacket *) NULL)
            break;
          length=ImportQuantumPixels(canvas_image,(CacheView *) NULL,
            quantum_info,BlackQuantum,pixels,exception);
          if (SyncAuthenticPixels(canvas_image,exception) == MagickFalse)
            break;
          if (((y-image->extract_info.y) >= 0) &&
              ((y-image->extract_info.y) < (ssize_t) image->rows))
            {
              p=GetVirtualPixels(canvas_image,canvas_image->extract_info.x,0,
                canvas_image->columns,1,exception);
              q=GetAuthenticPixels(image,0,y-image->extract_info.y,
                image->columns,1,exception);
              if ((p == (const PixelPacket *) NULL) ||
                  (q == (PixelPacket *) NULL))
                break;
              canvas_indexes=GetVirtualIndexQueue(canvas_image);
              indexes=GetAuthenticIndexQueue(image);
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                SetPixelIndex(indexes+x,GetPixelIndex(
                  canvas_indexes+image->extract_info.x+x));
                p++;
                q++;
              }
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
            }
          count=ReadBlob(image,length,pixels);
        }
        if (image->previous == (Image *) NULL)
          {
            status=SetImageProgress(image,LoadImageTag,4,6);
            if (status == MagickFalse)
              break;
          }
        if (image->matte != MagickFalse)
          {
            for (y=0; y < (ssize_t) image->extract_info.height; y++)
            {
              register const PixelPacket
                *restrict p;

              register PixelPacket
                *restrict q;

              register ssize_t
                x;

              if (count != (ssize_t) length)
                {
                  ThrowFileException(exception,CorruptImageError,
                    "UnexpectedEndOfFile",image->filename);
                  break;
                }
              q=GetAuthenticPixels(canvas_image,0,0,canvas_image->columns,1,
                exception);
              if (q == (PixelPacket *) NULL)
                break;
              length=ImportQuantumPixels(canvas_image,(CacheView *) NULL,
                quantum_info,AlphaQuantum,pixels,exception);
              if (SyncAuthenticPixels(canvas_image,exception) == MagickFalse)
                break;
              if (((y-image->extract_info.y) >= 0) &&
                  ((y-image->extract_info.y) < (ssize_t) image->rows))
                {
                  p=GetVirtualPixels(canvas_image,
                    canvas_image->extract_info.x,0,canvas_image->columns,1,
                    exception);
                  q=GetAuthenticPixels(image,0,y-image->extract_info.y,
                    image->columns,1,exception);
                  if ((p == (const PixelPacket *) NULL) ||
                      (q == (PixelPacket *) NULL))
                    break;
                  for (x=0; x < (ssize_t) image->columns; x++)
                  {
                    SetPixelOpacity(q,GetPixelOpacity(p));
                    p++;
                    q++;
                  }
                  if (SyncAuthenticPixels(image,exception) == MagickFalse)
                    break;
                }
              count=ReadBlob(image,length,pixels);
            }
            if (image->previous == (Image *) NULL)
              {
                status=SetImageProgress(image,LoadImageTag,5,6);
                if (status == MagickFalse)
                  break;
              }
          }
        if (image->previous == (Image *) NULL)
          {
            status=SetImageProgress(image,LoadImageTag,6,6);
            if (status == MagickFalse)
              break;
          }
        break;
      }
      case PartitionInterlace:
      {
        /*
          Partition interlacing:  CCCCCC..., MMMMMM..., YYYYYY..., KKKKKK...
        */
        AppendImageFormat("C",image->filename);
        status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
        if (status == MagickFalse)
          {
            canvas_image=DestroyImageList(canvas_image);
            image=DestroyImageList(image);
            return((Image *) NULL);
          }
        if (DiscardBlobBytes(image,image->offset) == MagickFalse)
          ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
            image->filename);
        length=GetQuantumExtent(canvas_image,quantum_info,CyanQuantum);
        for (i=0; i < (ssize_t) scene; i++)
          for (y=0; y < (ssize_t) image->extract_info.height; y++)
            if (ReadBlob(image,length,pixels) != (ssize_t) length)
              {
                ThrowFileException(exception,CorruptImageError,
                  "UnexpectedEndOfFile",image->filename);
                break;
              }
        count=ReadBlob(image,length,pixels);
        for (y=0; y < (ssize_t) image->extract_info.height; y++)
        {
          register const PixelPacket
            *restrict p;

          register PixelPacket
            *restrict q;

          register ssize_t
            x;

          if (count != (ssize_t) length)
            {
              ThrowFileException(exception,CorruptImageError,
                "UnexpectedEndOfFile",image->filename);
              break;
            }
          q=GetAuthenticPixels(canvas_image,0,0,canvas_image->columns,1,
            exception);
          if (q == (PixelPacket *) NULL)
            break;
          length=ImportQuantumPixels(canvas_image,(CacheView *) NULL,
            quantum_info,CyanQuantum,pixels,exception);
          if (SyncAuthenticPixels(canvas_image,exception) == MagickFalse)
            break;
          if (((y-image->extract_info.y) >= 0) &&
              ((y-image->extract_info.y) < (ssize_t) image->rows))
            {
              p=GetVirtualPixels(canvas_image,canvas_image->extract_info.x,0,
                canvas_image->columns,1,exception);
              q=GetAuthenticPixels(image,0,y-image->extract_info.y,
                image->columns,1,exception);
              if ((p == (const PixelPacket *) NULL) ||
                  (q == (PixelPacket *) NULL))
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                SetPixelRed(q,GetPixelRed(p));
                p++;
                q++;
              }
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
            }
          count=ReadBlob(image,length,pixels);
        }
        if (image->previous == (Image *) NULL)
          {
            status=SetImageProgress(image,LoadImageTag,1,5);
            if (status == MagickFalse)
              break;
          }
        (void) CloseBlob(image);
        AppendImageFormat("M",image->filename);
        status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
        if (status == MagickFalse)
          {
            canvas_image=DestroyImageList(canvas_image);
            image=DestroyImageList(image);
            return((Image *) NULL);
          }
        length=GetQuantumExtent(canvas_image,quantum_info,MagentaQuantum);
        for (i=0; i < (ssize_t) scene; i++)
          for (y=0; y < (ssize_t) image->extract_info.height; y++)
            if (ReadBlob(image,length,pixels) != (ssize_t) length)
              {
                ThrowFileException(exception,CorruptImageError,
                  "UnexpectedEndOfFile",image->filename);
                break;
              }
        count=ReadBlob(image,length,pixels);
        for (y=0; y < (ssize_t) image->extract_info.height; y++)
        {
          register const PixelPacket
            *restrict p;

          register PixelPacket
            *restrict q;

          register ssize_t
            x;

          if (count != (ssize_t) length)
            {
              ThrowFileException(exception,CorruptImageError,
                "UnexpectedEndOfFile",image->filename);
              break;
            }
          q=GetAuthenticPixels(canvas_image,0,0,canvas_image->columns,1,
            exception);
          if (q == (PixelPacket *) NULL)
            break;
          length=ImportQuantumPixels(canvas_image,(CacheView *) NULL,
            quantum_info,MagentaQuantum,pixels,exception);
          if (SyncAuthenticPixels(canvas_image,exception) == MagickFalse)
            break;
          if (((y-image->extract_info.y) >= 0) &&
              ((y-image->extract_info.y) < (ssize_t) image->rows))
            {
              p=GetVirtualPixels(canvas_image,canvas_image->extract_info.x,0,
                canvas_image->columns,1,exception);
              q=GetAuthenticPixels(image,0,y-image->extract_info.y,
                image->columns,1,exception);
              if ((p == (const PixelPacket *) NULL) ||
                  (q == (PixelPacket *) NULL))
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                SetPixelGreen(q,GetPixelGreen(p));
                p++;
                q++;
              }
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
           }
          count=ReadBlob(image,length,pixels);
        }
        if (image->previous == (Image *) NULL)
          {
            status=SetImageProgress(image,LoadImageTag,2,5);
            if (status == MagickFalse)
              break;
          }
        (void) CloseBlob(image);
        AppendImageFormat("Y",image->filename);
        status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
        if (status == MagickFalse)
          {
            canvas_image=DestroyImageList(canvas_image);
            image=DestroyImageList(image);
            return((Image *) NULL);
          }
        length=GetQuantumExtent(canvas_image,quantum_info,YellowQuantum);
        for (i=0; i < (ssize_t) scene; i++)
          for (y=0; y < (ssize_t) image->extract_info.height; y++)
            if (ReadBlob(image,length,pixels) != (ssize_t) length)
              {
                ThrowFileException(exception,CorruptImageError,
                  "UnexpectedEndOfFile",image->filename);
                break;
              }
        count=ReadBlob(image,length,pixels);
        for (y=0; y < (ssize_t) image->extract_info.height; y++)
        {
          register const PixelPacket
            *restrict p;

          register PixelPacket
            *restrict q;

          register ssize_t
            x;

          if (count != (ssize_t) length)
            {
              ThrowFileException(exception,CorruptImageError,
                "UnexpectedEndOfFile",image->filename);
              break;
            }
          q=GetAuthenticPixels(canvas_image,0,0,canvas_image->columns,1,
            exception);
          if (q == (PixelPacket *) NULL)
            break;
          length=ImportQuantumPixels(canvas_image,(CacheView *) NULL,
            quantum_info,YellowQuantum,pixels,exception);
          if (SyncAuthenticPixels(canvas_image,exception) == MagickFalse)
            break;
          if (((y-image->extract_info.y) >= 0) &&
              ((y-image->extract_info.y) < (ssize_t) image->rows))
            {
              p=GetVirtualPixels(canvas_image,canvas_image->extract_info.x,0,
                canvas_image->columns,1,exception);
              q=GetAuthenticPixels(image,0,y-image->extract_info.y,
                image->columns,1,exception);
              if ((p == (const PixelPacket *) NULL) ||
                  (q == (PixelPacket *) NULL))
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                SetPixelBlue(q,GetPixelBlue(p));
                p++;
                q++;
              }
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
           }
          count=ReadBlob(image,length,pixels);
        }
        if (image->previous == (Image *) NULL)
          {
            status=SetImageProgress(image,LoadImageTag,3,5);
            if (status == MagickFalse)
              break;
          }
        (void) CloseBlob(image);
        AppendImageFormat("K",image->filename);
        status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
        if (status == MagickFalse)
          {
            canvas_image=DestroyImageList(canvas_image);
            image=DestroyImageList(image);
            return((Image *) NULL);
          }
        length=GetQuantumExtent(canvas_image,quantum_info,BlackQuantum);
        for (i=0; i < (ssize_t) scene; i++)
          for (y=0; y < (ssize_t) image->extract_info.height; y++)
            if (ReadBlob(image,length,pixels) != (ssize_t) length)
              {
                ThrowFileException(exception,CorruptImageError,
                  "UnexpectedEndOfFile",image->filename);
                break;
              }
        count=ReadBlob(image,length,pixels);
        for (y=0; y < (ssize_t) image->extract_info.height; y++)
        {
          register const IndexPacket
            *restrict canvas_indexes;

          register const PixelPacket
            *restrict p;

          register IndexPacket
            *restrict indexes;

          register PixelPacket
            *restrict q;

          register ssize_t
            x;

          if (count != (ssize_t) length)
            {
              ThrowFileException(exception,CorruptImageError,
                "UnexpectedEndOfFile",image->filename);
              break;
            }
          q=GetAuthenticPixels(canvas_image,0,0,canvas_image->columns,1,
            exception);
          if (q == (PixelPacket *) NULL)
            break;
          length=ImportQuantumPixels(canvas_image,(CacheView *) NULL,
            quantum_info,BlackQuantum,pixels,exception);
          if (SyncAuthenticPixels(canvas_image,exception) == MagickFalse)
            break;
          if (((y-image->extract_info.y) >= 0) &&
              ((y-image->extract_info.y) < (ssize_t) image->rows))
            {
              p=GetVirtualPixels(canvas_image,canvas_image->extract_info.x,0,
                canvas_image->columns,1,exception);
              q=GetAuthenticPixels(image,0,y-image->extract_info.y,
                image->columns,1,exception);
              if ((p == (const PixelPacket *) NULL) ||
                  (q == (PixelPacket *) NULL))
                break;
              canvas_indexes=GetVirtualIndexQueue(canvas_image);
              indexes=GetAuthenticIndexQueue(image);
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                SetPixelIndex(indexes+x,GetPixelIndex(
                  canvas_indexes+image->extract_info.x+x));
                p++;
                q++;
              }
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
           }
          count=ReadBlob(image,length,pixels);
        }
        if (image->previous == (Image *) NULL)
          {
            status=SetImageProgress(image,LoadImageTag,3,5);
            if (status == MagickFalse)
              break;
          }
        if (image->matte != MagickFalse)
          {
            (void) CloseBlob(image);
            AppendImageFormat("A",image->filename);
            status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
            if (status == MagickFalse)
              {
                canvas_image=DestroyImageList(canvas_image);
                image=DestroyImageList(image);
                return((Image *) NULL);
              }
            length=GetQuantumExtent(canvas_image,quantum_info,AlphaQuantum);
            for (i=0; i < (ssize_t) scene; i++)
              for (y=0; y < (ssize_t) image->extract_info.height; y++)
                if (ReadBlob(image,length,pixels) != (ssize_t) length)
                  {
                    ThrowFileException(exception,CorruptImageError,
                      "UnexpectedEndOfFile",image->filename);
                    break;
                  }
            count=ReadBlob(image,length,pixels);
            for (y=0; y < (ssize_t) image->extract_info.height; y++)
            {
              register const PixelPacket
                *restrict p;

              register PixelPacket
                *restrict q;

              register ssize_t
                x;

              if (count != (ssize_t) length)
                {
                  ThrowFileException(exception,CorruptImageError,
                    "UnexpectedEndOfFile",image->filename);
                  break;
                }
              q=GetAuthenticPixels(canvas_image,0,0,canvas_image->columns,1,
                exception);
              if (q == (PixelPacket *) NULL)
                break;
              length=ImportQuantumPixels(canvas_image,(CacheView *) NULL,
                quantum_info,YellowQuantum,pixels,exception);
              if (SyncAuthenticPixels(canvas_image,exception) == MagickFalse)
                break;
              if (((y-image->extract_info.y) >= 0) &&
                  ((y-image->extract_info.y) < (ssize_t) image->rows))
                {
                  p=GetVirtualPixels(canvas_image,canvas_image->extract_info.x,
                    0,canvas_image->columns,1,exception);
                  q=GetAuthenticPixels(image,0,y-image->extract_info.y,
                    image->columns,1,exception);
                  if ((p == (const PixelPacket *) NULL) ||
                      (q == (PixelPacket *) NULL))
                    break;
                  for (x=0; x < (ssize_t) image->columns; x++)
                  {
                    SetPixelOpacity(q,GetPixelOpacity(p));
                    p++;
                    q++;
                  }
                  if (SyncAuthenticPixels(image,exception) == MagickFalse)
                    break;
               }
              count=ReadBlob(image,length,pixels);
            }
            if (image->previous == (Image *) NULL)
              {
                status=SetImageProgress(image,LoadImageTag,4,5);
                if (status == MagickFalse)
                  break;
              }
          }
        if (image->previous == (Image *) NULL)
          {
            status=SetImageProgress(image,LoadImageTag,5,5);
            if (status == MagickFalse)
              break;
          }
        break;
      }
    }
    SetQuantumImageType(image,quantum_type);
    /*
      Proceed to next image.
    */
    if (image_info->number_scenes != 0)
      if (image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    if (count == (ssize_t) length)
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
    scene++;
  } while (count == (ssize_t) length);
  quantum_info=DestroyQuantumInfo(quantum_info);
  InheritException(&image->exception,&canvas_image->exception);
  canvas_image=DestroyImage(canvas_image);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
