static MagickBooleanType WritePNMImage(const ImageInfo *image_info,Image *image)
{
  char
    buffer[MaxTextExtent],
    format,
    magick[MaxTextExtent];

  const char
    *value;

  IndexPacket
    index;

  MagickBooleanType
    status;

  MagickOffsetType
    scene;

  QuantumAny
    pixel;

  QuantumInfo
    *quantum_info;

  QuantumType
    quantum_type;

  register unsigned char
    *pixels,
    *q;

  size_t
    extent,
    imageListLength,
    packet_size;

  ssize_t
    count,
    y;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFalse)
    return(status);
  scene=0;
  imageListLength=GetImageListLength(image);
  do
  {
    QuantumAny
      max_value;

    /*
      Write PNM file header.
    */
    max_value=GetQuantumRange(image->depth);
    packet_size=3;
    quantum_type=RGBQuantum;
    (void) CopyMagickString(magick,image_info->magick,MaxTextExtent);
    switch (magick[1])
    {
      case 'A':
      case 'a':
      {
        format='7';
        break;
      }
      case 'B':
      case 'b':
      {
        format='4';
        if (image_info->compression == NoCompression)
          format='1';
        break;
      }
      case 'F':
      case 'f':
      {
        format='F';
        if (SetImageGray(image,&image->exception) != MagickFalse)
          format='f';
        break;
      }
      case 'G':
      case 'g':
      {
        format='5';
        if (image_info->compression == NoCompression)
          format='2';
        break;
      }
      case 'N':
      case 'n':
      {
        if ((image_info->type != TrueColorType) &&
            (SetImageGray(image,&image->exception) != MagickFalse))
          {
            format='5';
            if (image_info->compression == NoCompression)
              format='2';
            if (SetImageMonochrome(image,&image->exception) != MagickFalse)
              {
                format='4';
                if (image_info->compression == NoCompression)
                  format='1';
              }
            break;
          }
      }
      default:
      {
        format='6';
        if (image_info->compression == NoCompression)
          format='3';
        break;
      }
    }
    (void) FormatLocaleString(buffer,MaxTextExtent,"P%c\n",format);
    (void) WriteBlobString(image,buffer);
    value=GetImageProperty(image,"comment");
    if (value != (const char *) NULL)
      {
        register const char
          *p;

        /*
          Write comments to file.
        */
        (void) WriteBlobByte(image,'#');
        for (p=value; *p != '\0'; p++)
        {
          (void) WriteBlobByte(image,(unsigned char) *p);
          if ((*p == '\n') || (*p == '\r'))
            (void) WriteBlobByte(image,'#');
        }
        (void) WriteBlobByte(image,'\n');
      }
    if (format != '7')
      {
        (void) FormatLocaleString(buffer,MaxTextExtent,"%.20g %.20g\n",
          (double) image->columns,(double) image->rows);
        (void) WriteBlobString(image,buffer);
      }
    else
      {
        char
          type[MaxTextExtent];

        /*
          PAM header.
        */
        (void) FormatLocaleString(buffer,MaxTextExtent,
          "WIDTH %.20g\nHEIGHT %.20g\n",(double) image->columns,(double)
          image->rows);
        (void) WriteBlobString(image,buffer);
        quantum_type=GetQuantumType(image,&image->exception);
        switch (quantum_type)
        {
          case CMYKQuantum:
          case CMYKAQuantum:
          {
            packet_size=4;
            (void) CopyMagickString(type,"CMYK",MaxTextExtent);
            break;
          }
          case GrayQuantum:
          case GrayAlphaQuantum:
          {
            packet_size=1;
            (void) CopyMagickString(type,"GRAYSCALE",MaxTextExtent);
            if (IdentifyImageMonochrome(image,&image->exception) != MagickFalse)
              (void) CopyMagickString(type,"BLACKANDWHITE",MaxTextExtent);
            break;
          }
          default:
          {
            quantum_type=RGBQuantum;
            if (image->matte != MagickFalse)
              quantum_type=RGBAQuantum;
            packet_size=3;
            (void) CopyMagickString(type,"RGB",MaxTextExtent);
            break;
          }
        }
        if (image->matte != MagickFalse)
          {
            packet_size++;
            (void) ConcatenateMagickString(type,"_ALPHA",MaxTextExtent);
          }
        if (image->depth > 32)
          image->depth=32;
        (void) FormatLocaleString(buffer,MaxTextExtent,
          "DEPTH %.20g\nMAXVAL %.20g\n",(double) packet_size,(double)
          ((MagickOffsetType) GetQuantumRange(image->depth)));
        (void) WriteBlobString(image,buffer);
        (void) FormatLocaleString(buffer,MaxTextExtent,"TUPLTYPE %s\nENDHDR\n",
          type);
        (void) WriteBlobString(image,buffer);
      }
    /*
      Convert to PNM raster pixels.
    */
    switch (format)
    {
      case '1':
      {
        unsigned char
          pixels[2048];

        /*
          Convert image to a PBM image.
        */
        (void) SetImageType(image,BilevelType);
        q=pixels;
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          register const PixelPacket
            *magick_restrict p;

          register ssize_t
            x;

          p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            *q++=(unsigned char) (GetPixelLuma(image,p) >= (QuantumRange/2.0) ?
              '0' : '1');
            *q++=' ';
            if ((q-pixels+1) >= (ssize_t) sizeof(pixels))
              {
                *q++='\n';
                (void) WriteBlob(image,q-pixels,pixels);
                q=pixels;
              }
            p++;
          }
          *q++='\n';
          (void) WriteBlob(image,q-pixels,pixels);
          q=pixels;
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        if (q != pixels)
          {
            *q++='\n';
            (void) WriteBlob(image,q-pixels,pixels);
          }
        break;
      }
      case '2':
      {
        unsigned char
          pixels[2048];

        /*
          Convert image to a PGM image.
        */
        if (image->depth <= 8)
          (void) WriteBlobString(image,"255\n");
        else
          if (image->depth <= 16)
            (void) WriteBlobString(image,"65535\n");
          else
            (void) WriteBlobString(image,"4294967295\n");
        q=pixels;
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          register const PixelPacket
            *magick_restrict p;

          register ssize_t
            x;

          p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            index=ClampToQuantum(GetPixelLuma(image,p));
            if (image->depth <= 8)
              count=(ssize_t) FormatLocaleString(buffer,MaxTextExtent,"%u ",
                ScaleQuantumToChar(index));
            else
              if (image->depth <= 16)
                count=(ssize_t) FormatLocaleString(buffer,MaxTextExtent,"%u ",
                  ScaleQuantumToShort(index));
              else
                 count=(ssize_t) FormatLocaleString(buffer,MaxTextExtent,"%u ",
                   ScaleQuantumToLong(index));
             extent=(size_t) count;
            if ((q-pixels+extent+1) >= sizeof(pixels))
               {
                 *q++='\n';
                 (void) WriteBlob(image,q-pixels,pixels);
                 q=pixels;
               }
            (void) strncpy((char *) q,buffer,extent);
            q+=extent;
             p++;
           }
           *q++='\n';
          (void) WriteBlob(image,q-pixels,pixels);
          q=pixels;
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        if (q != pixels)
          {
            *q++='\n';
            (void) WriteBlob(image,q-pixels,pixels);
          }
        break;
      }
      case '3':
      {
        unsigned char
          pixels[2048];

        /*
          Convert image to a PNM image.
        */
        (void) TransformImageColorspace(image,sRGBColorspace);
        if (image->depth <= 8)
          (void) WriteBlobString(image,"255\n");
        else
          if (image->depth <= 16)
            (void) WriteBlobString(image,"65535\n");
          else
            (void) WriteBlobString(image,"4294967295\n");
        q=pixels;
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          register const PixelPacket
            *magick_restrict p;

          register ssize_t
            x;

          p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            if (image->depth <= 8)
              count=(ssize_t) FormatLocaleString(buffer,MaxTextExtent,
                "%u %u %u ",ScaleQuantumToChar(GetPixelRed(p)),
                ScaleQuantumToChar(GetPixelGreen(p)),
                ScaleQuantumToChar(GetPixelBlue(p)));
            else
              if (image->depth <= 16)
                count=(ssize_t) FormatLocaleString(buffer,MaxTextExtent,
                  "%u %u %u ",ScaleQuantumToShort(GetPixelRed(p)),
                  ScaleQuantumToShort(GetPixelGreen(p)),
                  ScaleQuantumToShort(GetPixelBlue(p)));
              else
                count=(ssize_t) FormatLocaleString(buffer,MaxTextExtent,
                  "%u %u %u ",ScaleQuantumToLong(GetPixelRed(p)),
                   ScaleQuantumToLong(GetPixelGreen(p)),
                   ScaleQuantumToLong(GetPixelBlue(p)));
             extent=(size_t) count;
            if ((q-pixels+extent+1) >= sizeof(pixels))
               {
                 *q++='\n';
                 (void) WriteBlob(image,q-pixels,pixels);
                 q=pixels;
               }
            (void) strncpy((char *) q,buffer,extent);
            q+=extent;
             p++;
           }
           *q++='\n';
          (void) WriteBlob(image,q-pixels,pixels);
          q=pixels;
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        if (q != pixels)
          {
            *q++='\n';
            (void) WriteBlob(image,q-pixels,pixels);
          }
        break;
      }
      case '4':
      {
        /*
          Convert image to a PBM image.
        */
        (void) SetImageType(image,BilevelType);
        image->depth=1;
        quantum_info=AcquireQuantumInfo(image_info,image);
        if (quantum_info == (QuantumInfo *) NULL)
          ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
        (void) SetQuantumEndian(image,quantum_info,MSBEndian);
        quantum_info->min_is_white=MagickTrue;
        pixels=GetQuantumPixels(quantum_info);
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          register const PixelPacket
            *magick_restrict p;

          p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          extent=ExportQuantumPixels(image,(const CacheView *) NULL,
            quantum_info,GrayQuantum,pixels,&image->exception);
          count=WriteBlob(image,extent,pixels);
          if (count != (ssize_t) extent)
            break;
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        quantum_info=DestroyQuantumInfo(quantum_info);
        break;
      }
      case '5':
      {
        /*
          Convert image to a PGM image.
        */
        if (image->depth > 32)
          image->depth=32;
        (void) FormatLocaleString(buffer,MaxTextExtent,"%.20g\n",(double)
          ((MagickOffsetType) GetQuantumRange(image->depth)));
        (void) WriteBlobString(image,buffer);
        quantum_info=AcquireQuantumInfo(image_info,image);
        if (quantum_info == (QuantumInfo *) NULL)
          ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
        (void) SetQuantumEndian(image,quantum_info,MSBEndian);
        quantum_info->min_is_white=MagickTrue;
        pixels=GetQuantumPixels(quantum_info);
        extent=GetQuantumExtent(image,quantum_info,GrayQuantum);
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          register const PixelPacket
            *magick_restrict p;

          register ssize_t
            x;

          p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          q=pixels;
          switch (image->depth)
          {
            case 8:
            case 16:
            case 32:
            {
              extent=ExportQuantumPixels(image,(const CacheView *) NULL,
                quantum_info,GrayQuantum,pixels,&image->exception);
              break;
            }
            default:
            {
              if (image->depth <= 8)
                {
                  for (x=0; x < (ssize_t) image->columns; x++)
                  {
                    if (IsGrayPixel(p) == MagickFalse)
                      pixel=ScaleQuantumToAny(ClampToQuantum(
                        GetPixelLuma(image,p)),max_value);
                    else
                      {
                        if (image->depth == 8)
                          pixel=ScaleQuantumToChar(GetPixelRed(p));
                        else
                          pixel=ScaleQuantumToAny(GetPixelRed(p),max_value);
                      }
                    q=PopCharPixel((unsigned char) pixel,q);
                    p++;
                  }
                  extent=(size_t) (q-pixels);
                  break;
                }
              if (image->depth <= 16)
                {
                  for (x=0; x < (ssize_t) image->columns; x++)
                  {
                    if (IsGrayPixel(p) == MagickFalse)
                      pixel=ScaleQuantumToAny(ClampToQuantum(
                        GetPixelLuma(image,p)),max_value);
                    else
                      {
                        if (image->depth == 16)
                          pixel=ScaleQuantumToShort(GetPixelRed(p));
                        else
                          pixel=ScaleQuantumToAny(GetPixelRed(p),max_value);
                      }
                    q=PopShortPixel(MSBEndian,(unsigned short) pixel,q);
                    p++;
                  }
                  extent=(size_t) (q-pixels);
                  break;
                }
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                if (IsGrayPixel(p) == MagickFalse)
                  pixel=ScaleQuantumToAny(ClampToQuantum(
                    GetPixelLuma(image,p)),max_value);
                else
                  {
                    if (image->depth == 32)
                      pixel=ScaleQuantumToLong(GetPixelRed(p));
                    else
                      pixel=ScaleQuantumToAny(GetPixelRed(p),max_value);
                  }
                q=PopLongPixel(MSBEndian,(unsigned int) pixel,q);
                p++;
              }
              extent=(size_t) (q-pixels);
              break;
            }
          }
          count=WriteBlob(image,extent,pixels);
          if (count != (ssize_t) extent)
            break;
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        quantum_info=DestroyQuantumInfo(quantum_info);
        break;
      }
      case '6':
      {
        /*
          Convert image to a PNM image.
        */
        (void) TransformImageColorspace(image,sRGBColorspace);
        if (image->depth > 32)
          image->depth=32;
        (void) FormatLocaleString(buffer,MaxTextExtent,"%.20g\n",(double)
          ((MagickOffsetType) GetQuantumRange(image->depth)));
        (void) WriteBlobString(image,buffer);
        quantum_info=AcquireQuantumInfo(image_info,image);
        if (quantum_info == (QuantumInfo *) NULL)
          ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
        (void) SetQuantumEndian(image,quantum_info,MSBEndian);
        (void) SetQuantumEndian(image,quantum_info,MSBEndian);
        pixels=GetQuantumPixels(quantum_info);
        extent=GetQuantumExtent(image,quantum_info,quantum_type);
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          register const PixelPacket
            *magick_restrict p;

          register ssize_t
            x;

          p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          q=pixels;
          switch (image->depth)
          {
            case 8:
            case 16:
            case 32:
            {
              extent=ExportQuantumPixels(image,(const CacheView *) NULL,
                quantum_info,quantum_type,pixels,&image->exception);
              break;
            }
            default:
            {
              if (image->depth <= 8)
                {
                  for (x=0; x < (ssize_t) image->columns; x++)
                  {
                    pixel=ScaleQuantumToAny(GetPixelRed(p),max_value);
                    q=PopCharPixel((unsigned char) pixel,q);
                    pixel=ScaleQuantumToAny(GetPixelGreen(p),max_value);
                    q=PopCharPixel((unsigned char) pixel,q);
                    pixel=ScaleQuantumToAny(GetPixelBlue(p),max_value);
                    q=PopCharPixel((unsigned char) pixel,q);
                    p++;
                  }
                  extent=(size_t) (q-pixels);
                  break;
                }
              if (image->depth <= 16)
                {
                  for (x=0; x < (ssize_t) image->columns; x++)
                  {
                    pixel=ScaleQuantumToAny(GetPixelRed(p),max_value);
                    q=PopShortPixel(MSBEndian,(unsigned short) pixel,q);
                    pixel=ScaleQuantumToAny(GetPixelGreen(p),max_value);
                    q=PopShortPixel(MSBEndian,(unsigned short) pixel,q);
                    pixel=ScaleQuantumToAny(GetPixelBlue(p),max_value);
                    q=PopShortPixel(MSBEndian,(unsigned short) pixel,q);
                    p++;
                  }
                  extent=(size_t) (q-pixels);
                  break;
                }
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                pixel=ScaleQuantumToAny(GetPixelRed(p),max_value);
                q=PopLongPixel(MSBEndian,(unsigned short) pixel,q);
                pixel=ScaleQuantumToAny(GetPixelGreen(p),max_value);
                q=PopLongPixel(MSBEndian,(unsigned short) pixel,q);
                pixel=ScaleQuantumToAny(GetPixelBlue(p),max_value);
                q=PopLongPixel(MSBEndian,(unsigned short) pixel,q);
                p++;
              }
              extent=(size_t) (q-pixels);
              break;
            }
          }
          count=WriteBlob(image,extent,pixels);
          if (count != (ssize_t) extent)
            break;
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        quantum_info=DestroyQuantumInfo(quantum_info);
        break;
      }
      case '7':
      {
        /*
          Convert image to a PAM.
        */
        if (image->depth > 32)
          image->depth=32;
        quantum_info=AcquireQuantumInfo(image_info,image);
        if (quantum_info == (QuantumInfo *) NULL)
          ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
        (void) SetQuantumEndian(image,quantum_info,MSBEndian);
        pixels=GetQuantumPixels(quantum_info);
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          register const IndexPacket
            *magick_restrict indexes;

          register const PixelPacket
            *magick_restrict p;

          register ssize_t
            x;

          p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          indexes=GetVirtualIndexQueue(image);
          q=pixels;
          switch (image->depth)
          {
            case 8:
            case 16:
            case 32:
            {
              extent=ExportQuantumPixels(image,(const CacheView *) NULL,
                quantum_info,quantum_type,pixels,&image->exception);
              break;
            }
            default:
            {
              switch (quantum_type)
              {
                case GrayQuantum:
                case GrayAlphaQuantum:
                {
                  if (image->depth <= 8)
                    {
                      for (x=0; x < (ssize_t) image->columns; x++)
                      {
                        pixel=ScaleQuantumToAny(ClampToQuantum(
                          GetPixelLuma(image,p)),max_value);
                        q=PopCharPixel((unsigned char) pixel,q);
                        if (image->matte != MagickFalse)
                          {
                            pixel=(unsigned char) ScaleQuantumToAny(
                              GetPixelOpacity(p),max_value);
                            q=PopCharPixel((unsigned char) pixel,q);
                          }
                        p++;
                      }
                      break;
                    }
                  if (image->depth <= 16)
                    {
                      for (x=0; x < (ssize_t) image->columns; x++)
                      {
                        pixel=ScaleQuantumToAny(ClampToQuantum(
                          GetPixelLuma(image,p)),max_value);
                        q=PopShortPixel(MSBEndian,(unsigned short) pixel,q);
                        if (image->matte != MagickFalse)
                          {
                            pixel=(unsigned char) ScaleQuantumToAny(
                              GetPixelOpacity(p),max_value);
                            q=PopShortPixel(MSBEndian,(unsigned short) pixel,q);
                          }
                        p++;
                      }
                      break;
                    }
                  for (x=0; x < (ssize_t) image->columns; x++)
                  {
                    pixel=ScaleQuantumToAny(ClampToQuantum(
                      GetPixelLuma(image,p)),max_value);
                    q=PopLongPixel(MSBEndian,(unsigned int) pixel,q);
                    if (image->matte != MagickFalse)
                      {
                        pixel=(unsigned char) ScaleQuantumToAny(
                          GetPixelOpacity(p),max_value);
                        q=PopLongPixel(MSBEndian,(unsigned int) pixel,q);
                      }
                    p++;
                  }
                  break;
                }
                case CMYKQuantum:
                case CMYKAQuantum:
                {
                  if (image->depth <= 8)
                    {
                      for (x=0; x < (ssize_t) image->columns; x++)
                      {
                        pixel=ScaleQuantumToAny(GetPixelRed(p),max_value);
                        q=PopCharPixel((unsigned char) pixel,q);
                        pixel=ScaleQuantumToAny(GetPixelGreen(p),max_value);
                        q=PopCharPixel((unsigned char) pixel,q);
                        pixel=ScaleQuantumToAny(GetPixelBlue(p),max_value);
                        q=PopCharPixel((unsigned char) pixel,q);
                        pixel=ScaleQuantumToAny(GetPixelIndex(indexes+x),
                          max_value);
                        q=PopCharPixel((unsigned char) pixel,q);
                        if (image->matte != MagickFalse)
                          {
                            pixel=ScaleQuantumToAny((Quantum) (QuantumRange-
                              GetPixelOpacity(p)),max_value);
                            q=PopCharPixel((unsigned char) pixel,q);
                          }
                        p++;
                      }
                      break;
                    }
                  if (image->depth <= 16)
                    {
                      for (x=0; x < (ssize_t) image->columns; x++)
                      {
                        pixel=ScaleQuantumToAny(GetPixelRed(p),max_value);
                        q=PopShortPixel(MSBEndian,(unsigned short) pixel,q);
                        pixel=ScaleQuantumToAny(GetPixelGreen(p),max_value);
                        q=PopShortPixel(MSBEndian,(unsigned short) pixel,q);
                        pixel=ScaleQuantumToAny(GetPixelBlue(p),max_value);
                        q=PopShortPixel(MSBEndian,(unsigned short) pixel,q);
                        pixel=ScaleQuantumToAny(GetPixelIndex(indexes+x),
                          max_value);
                        q=PopShortPixel(MSBEndian,(unsigned short) pixel,q);
                        if (image->matte != MagickFalse)
                          {
                            pixel=ScaleQuantumToAny((Quantum) (QuantumRange-
                              GetPixelOpacity(p)),max_value);
                            q=PopShortPixel(MSBEndian,(unsigned short) pixel,q);
                          }
                        p++;
                      }
                      break;
                    }
                  for (x=0; x < (ssize_t) image->columns; x++)
                  {
                    pixel=ScaleQuantumToAny(GetPixelRed(p),max_value);
                    q=PopLongPixel(MSBEndian,(unsigned int) pixel,q);
                    pixel=ScaleQuantumToAny(GetPixelGreen(p),max_value);
                    q=PopLongPixel(MSBEndian,(unsigned int) pixel,q);
                    pixel=ScaleQuantumToAny(GetPixelBlue(p),max_value);
                    q=PopLongPixel(MSBEndian,(unsigned int) pixel,q);
                    pixel=ScaleQuantumToAny(GetPixelIndex(indexes+x),max_value);
                    q=PopLongPixel(MSBEndian,(unsigned int) pixel,q);
                    if (image->matte != MagickFalse)
                      {
                        pixel=ScaleQuantumToAny((Quantum) (QuantumRange-
                          GetPixelOpacity(p)),max_value);
                        q=PopLongPixel(MSBEndian,(unsigned int) pixel,q);
                      }
                    p++;
                  }
                  break;
                }
                default:
                {
                  if (image->depth <= 8)
                    {
                      for (x=0; x < (ssize_t) image->columns; x++)
                      {
                        pixel=ScaleQuantumToAny(GetPixelRed(p),max_value);
                        q=PopCharPixel((unsigned char) pixel,q);
                        pixel=ScaleQuantumToAny(GetPixelGreen(p),max_value);
                        q=PopCharPixel((unsigned char) pixel,q);
                        pixel=ScaleQuantumToAny(GetPixelBlue(p),max_value);
                        q=PopCharPixel((unsigned char) pixel,q);
                        if (image->matte != MagickFalse)
                          {
                            pixel=ScaleQuantumToAny((Quantum) (QuantumRange-
                              GetPixelOpacity(p)),max_value);
                            q=PopCharPixel((unsigned char) pixel,q);
                          }
                        p++;
                      }
                      break;
                    }
                  if (image->depth <= 16)
                    {
                      for (x=0; x < (ssize_t) image->columns; x++)
                      {
                        pixel=ScaleQuantumToAny(GetPixelRed(p),max_value);
                        q=PopShortPixel(MSBEndian,(unsigned short) pixel,q);
                        pixel=ScaleQuantumToAny(GetPixelGreen(p),max_value);
                        q=PopShortPixel(MSBEndian,(unsigned short) pixel,q);
                        pixel=ScaleQuantumToAny(GetPixelBlue(p),max_value);
                        q=PopShortPixel(MSBEndian,(unsigned short) pixel,q);
                        if (image->matte != MagickFalse)
                          {
                            pixel=ScaleQuantumToAny((Quantum) (QuantumRange-
                              GetPixelOpacity(p)),max_value);
                            q=PopShortPixel(MSBEndian,(unsigned short) pixel,q);
                          }
                        p++;
                      }
                      break;
                    }
                  for (x=0; x < (ssize_t) image->columns; x++)
                  {
                    pixel=ScaleQuantumToAny(GetPixelRed(p),max_value);
                    q=PopLongPixel(MSBEndian,(unsigned int) pixel,q);
                    pixel=ScaleQuantumToAny(GetPixelGreen(p),max_value);
                    q=PopLongPixel(MSBEndian,(unsigned int) pixel,q);
                    pixel=ScaleQuantumToAny(GetPixelBlue(p),max_value);
                    q=PopLongPixel(MSBEndian,(unsigned int) pixel,q);
                    if (image->matte != MagickFalse)
                      {
                        pixel=ScaleQuantumToAny((Quantum) (QuantumRange-
                          GetPixelOpacity(p)),max_value);
                        q=PopLongPixel(MSBEndian,(unsigned int) pixel,q);
                      }
                    p++;
                  }
                  break;
                }
              }
              extent=(size_t) (q-pixels);
              break;
            }
          }
          count=WriteBlob(image,extent,pixels);
          if (count != (ssize_t) extent)
            break;
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        quantum_info=DestroyQuantumInfo(quantum_info);
        break;
      }
      case 'F':
      case 'f':
      {
        (void) WriteBlobString(image,image->endian == LSBEndian ? "-1.0\n" :
          "1.0\n");
        image->depth=32;
        quantum_type=format == 'f' ? GrayQuantum : RGBQuantum;
        quantum_info=AcquireQuantumInfo((const ImageInfo *) NULL,image);
        if (quantum_info == (QuantumInfo *) NULL)
          ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
        status=SetQuantumFormat(image,quantum_info,FloatingPointQuantumFormat);
        if (status == MagickFalse)
          ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
        pixels=GetQuantumPixels(quantum_info);
        for (y=(ssize_t) image->rows-1; y >= 0; y--)
        {
          register const PixelPacket
            *magick_restrict p;

          p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          extent=ExportQuantumPixels(image,(const CacheView *) NULL,
            quantum_info,quantum_type,pixels,&image->exception);
          (void) WriteBlob(image,extent,pixels);
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        quantum_info=DestroyQuantumInfo(quantum_info);
        break;
      }
    }
    if (GetNextImageInList(image) == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=SetImageProgress(image,SaveImagesTag,scene++,imageListLength);
    if (status == MagickFalse)
      break;
  } while (image_info->adjoin != MagickFalse);
  (void) CloseBlob(image);
  return(MagickTrue);
}
