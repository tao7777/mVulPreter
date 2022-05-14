static Image *ReadEMFImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  BITMAPINFO
    DIBinfo;

  HBITMAP
    hBitmap,
    hOldBitmap;

  HDC
    hDC;

  HENHMETAFILE
    hemf;

  Image
    *image;

  RECT
    rect;

  register ssize_t
    x;

  register PixelPacket
    *q;

  RGBQUAD
    *pBits,
    *ppBits;

  ssize_t
    height,
    width,
    y;

  image=AcquireImage(image_info);
  hemf=ReadEnhMetaFile(image_info->filename,&width,&height);
  if (hemf == (HENHMETAFILE) NULL)
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  if ((image->columns == 0) || (image->rows == 0))
    {
      double
        y_resolution,
        x_resolution;

      y_resolution=DefaultResolution;
      x_resolution=DefaultResolution;
      if (image->y_resolution > 0)
        {
          y_resolution=image->y_resolution;
          if (image->units == PixelsPerCentimeterResolution)
            y_resolution*=CENTIMETERS_INCH;
        }
      if (image->x_resolution > 0)
        {
          x_resolution=image->x_resolution;
          if (image->units == PixelsPerCentimeterResolution)
            x_resolution*=CENTIMETERS_INCH;
        }
      image->rows=(size_t) ((height/1000.0/CENTIMETERS_INCH)*y_resolution+0.5);
      image->columns=(size_t) ((width/1000.0/CENTIMETERS_INCH)*
        x_resolution+0.5);
    }
  if (image_info->size != (char *) NULL)
    {
      ssize_t
        x;

      image->columns=width;
      image->rows=height;
      x=0;
       y=0;
       (void) GetGeometry(image_info->size,&x,&y,&image->columns,&image->rows);
     }
  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }
   if (image_info->page != (char *) NULL)
     {
       char
        *geometry;

      register char
        *p;

      MagickStatusType
        flags;

      ssize_t
        sans;

      geometry=GetPageGeometry(image_info->page);
      p=strchr(geometry,'>');
      if (p == (char *) NULL)
        {
          flags=ParseMetaGeometry(geometry,&sans,&sans,&image->columns,
            &image->rows);
          if (image->x_resolution != 0.0)
            image->columns=(size_t) floor((image->columns*image->x_resolution)+
              0.5);
          if (image->y_resolution != 0.0)
            image->rows=(size_t) floor((image->rows*image->y_resolution)+0.5);
        }
      else
        {
          *p='\0';
          flags=ParseMetaGeometry(geometry,&sans,&sans,&image->columns,
            &image->rows);
          if (image->x_resolution != 0.0)
            image->columns=(size_t) floor(((image->columns*image->x_resolution)/
              DefaultResolution)+0.5);
          if (image->y_resolution != 0.0)
            image->rows=(size_t) floor(((image->rows*image->y_resolution)/
              DefaultResolution)+0.5);
        }
      (void) flags;
      geometry=DestroyString(geometry);
    }
  hDC=GetDC(NULL);
  if (hDC == (HDC) NULL)
    {
      DeleteEnhMetaFile(hemf);
      ThrowReaderException(ResourceLimitError,"UnableToCreateADC");
    }
  /*
    Initialize the bitmap header info.
  */
  (void) ResetMagickMemory(&DIBinfo,0,sizeof(BITMAPINFO));
  DIBinfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
  DIBinfo.bmiHeader.biWidth=(LONG) image->columns;
  DIBinfo.bmiHeader.biHeight=(-1)*(LONG) image->rows;
  DIBinfo.bmiHeader.biPlanes=1;
  DIBinfo.bmiHeader.biBitCount=32;
  DIBinfo.bmiHeader.biCompression=BI_RGB;
  hBitmap=CreateDIBSection(hDC,&DIBinfo,DIB_RGB_COLORS,(void **) &ppBits,NULL,
    0);
  ReleaseDC(NULL,hDC);
  if (hBitmap == (HBITMAP) NULL)
    {
      DeleteEnhMetaFile(hemf);
      ThrowReaderException(ResourceLimitError,"UnableToCreateBitmap");
    }
  hDC=CreateCompatibleDC(NULL);
  if (hDC == (HDC) NULL)
    {
      DeleteEnhMetaFile(hemf);
      DeleteObject(hBitmap);
      ThrowReaderException(ResourceLimitError,"UnableToCreateADC");
    }
  hOldBitmap=(HBITMAP) SelectObject(hDC,hBitmap);
  if (hOldBitmap == (HBITMAP) NULL)
    {
      DeleteEnhMetaFile(hemf);
      DeleteDC(hDC);
      DeleteObject(hBitmap);
      ThrowReaderException(ResourceLimitError,"UnableToCreateBitmap");
    }
  /*
    Initialize the bitmap to the image background color.
  */
  pBits=ppBits;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      pBits->rgbRed=ScaleQuantumToChar(image->background_color.red);
      pBits->rgbGreen=ScaleQuantumToChar(image->background_color.green);
      pBits->rgbBlue=ScaleQuantumToChar(image->background_color.blue);
      pBits++;
    }
  }
  rect.top=0;
  rect.left=0;
  rect.right=(LONG) image->columns;
  rect.bottom=(LONG) image->rows;
  /*
    Convert metafile pixels.
  */
  PlayEnhMetaFile(hDC,hemf,&rect);
  pBits=ppBits;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      SetPixelRed(q,ScaleCharToQuantum(pBits->rgbRed));
      SetPixelGreen(q,ScaleCharToQuantum(pBits->rgbGreen));
      SetPixelBlue(q,ScaleCharToQuantum(pBits->rgbBlue));
      SetPixelOpacity(q,OpaqueOpacity);
      pBits++;
      q++;
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
  }
  DeleteEnhMetaFile(hemf);
  SelectObject(hDC,hOldBitmap);
  DeleteDC(hDC);
  DeleteObject(hBitmap);
  return(GetFirstImageInList(image));
}
