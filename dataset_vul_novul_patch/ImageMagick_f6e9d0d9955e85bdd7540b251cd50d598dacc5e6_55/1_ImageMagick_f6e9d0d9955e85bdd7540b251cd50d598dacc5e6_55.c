static Image *ReadSCREENSHOTImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=(Image *) NULL;
#if defined(MAGICKCORE_WINGDI32_DELEGATE)
  {
    BITMAPINFO
      bmi;

    DISPLAY_DEVICE
      device;

    HBITMAP
      bitmap,
      bitmapOld;

    HDC
      bitmapDC,
      hDC;

    Image
      *screen;

     int
       i;
 
     register PixelPacket
       *q;
 
    register ssize_t
      x;

    RGBTRIPLE
      *p;

    ssize_t
      y;

    assert(image_info != (const ImageInfo *) NULL);
    i=0;
    device.cb = sizeof(device);
    image=(Image *) NULL;
    while(EnumDisplayDevices(NULL,i,&device,0) && ++i)
    {
      if ((device.StateFlags & DISPLAY_DEVICE_ACTIVE) != DISPLAY_DEVICE_ACTIVE)
        continue;

      hDC=CreateDC(device.DeviceName,device.DeviceName,NULL,NULL);
      if (hDC == (HDC) NULL)
        ThrowReaderException(CoderError,"UnableToCreateDC");

      screen=AcquireImage(image_info);
       screen->columns=(size_t) GetDeviceCaps(hDC,HORZRES);
       screen->rows=(size_t) GetDeviceCaps(hDC,VERTRES);
       screen->storage_class=DirectClass;
       if (image == (Image *) NULL)
         image=screen;
       else
        AppendImageToList(&image,screen);

      bitmapDC=CreateCompatibleDC(hDC);
      if (bitmapDC == (HDC) NULL)
        {
          DeleteDC(hDC);
          ThrowReaderException(CoderError,"UnableToCreateDC");
        }
      (void) ResetMagickMemory(&bmi,0,sizeof(BITMAPINFO));
      bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
      bmi.bmiHeader.biWidth=(LONG) screen->columns;
      bmi.bmiHeader.biHeight=(-1)*(LONG) screen->rows;
      bmi.bmiHeader.biPlanes=1;
      bmi.bmiHeader.biBitCount=24;
      bmi.bmiHeader.biCompression=BI_RGB;
      bitmap=CreateDIBSection(hDC,&bmi,DIB_RGB_COLORS,(void **) &p,NULL,0);
      if (bitmap == (HBITMAP) NULL)
        {
          DeleteDC(hDC);
          DeleteDC(bitmapDC);
          ThrowReaderException(CoderError,"UnableToCreateBitmap");
        }
      bitmapOld=(HBITMAP) SelectObject(bitmapDC,bitmap);
      if (bitmapOld == (HBITMAP) NULL)
        {
          DeleteDC(hDC);
          DeleteDC(bitmapDC);
          DeleteObject(bitmap);
          ThrowReaderException(CoderError,"UnableToCreateBitmap");
        }
      BitBlt(bitmapDC,0,0,(int) screen->columns,(int) screen->rows,hDC,0,0,
        SRCCOPY);
      (void) SelectObject(bitmapDC,bitmapOld);

      for (y=0; y < (ssize_t) screen->rows; y++)
      {
        q=QueueAuthenticPixels(screen,0,y,screen->columns,1,exception);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (ssize_t) screen->columns; x++)
        {
          SetPixelRed(q,ScaleCharToQuantum(p->rgbtRed));
          SetPixelGreen(q,ScaleCharToQuantum(p->rgbtGreen));
          SetPixelBlue(q,ScaleCharToQuantum(p->rgbtBlue));
          SetPixelOpacity(q,OpaqueOpacity);
          p++;
          q++;
        }
        if (SyncAuthenticPixels(screen,exception) == MagickFalse)
          break;
      }

      DeleteDC(hDC);
      DeleteDC(bitmapDC);
      DeleteObject(bitmap);
    }
  }
#elif defined(MAGICKCORE_X11_DELEGATE)
  {
    const char
      *option;

    XImportInfo
      ximage_info;

    (void) exception;
    XGetImportInfo(&ximage_info);
    option=GetImageOption(image_info,"x:screen");
    if (option != (const char *) NULL)
      ximage_info.screen=IsMagickTrue(option);
    option=GetImageOption(image_info,"x:silent");
    if (option != (const char *) NULL)
      ximage_info.silent=IsMagickTrue(option);
    image=XImportImage(image_info,&ximage_info);
  }
#endif
  return(image);
}
