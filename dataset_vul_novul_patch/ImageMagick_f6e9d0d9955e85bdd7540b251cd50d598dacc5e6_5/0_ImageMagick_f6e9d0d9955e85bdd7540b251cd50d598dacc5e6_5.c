static Image *ReadCLIPBOARDImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  register ssize_t
    x;

  register PixelPacket
    *q;

  ssize_t
    y;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AcquireImage(image_info);
  {
    HBITMAP
      bitmapH;

    HPALETTE
      hPal;

    OpenClipboard(NULL);
     bitmapH=(HBITMAP) GetClipboardData(CF_BITMAP);
     hPal=(HPALETTE) GetClipboardData(CF_PALETTE);
     CloseClipboard();
    if (bitmapH == NULL)
       ThrowReaderException(CoderError,"NoBitmapOnClipboard");
     {
       BITMAPINFO
        DIBinfo;

      BITMAP
        bitmap;

      HBITMAP
        hBitmap,
        hOldBitmap;

      HDC
        hDC,
        hMemDC;

      RGBQUAD
        *pBits,
        *ppBits;

      /* create an offscreen DC for the source */
      hMemDC=CreateCompatibleDC(NULL);
      hOldBitmap=(HBITMAP) SelectObject(hMemDC,bitmapH);
       GetObject(bitmapH,sizeof(BITMAP),(LPSTR) &bitmap);
       if ((image->columns == 0) || (image->rows == 0))
         {
           image->columns=bitmap.bmWidth;
          image->rows=bitmap.bmHeight;
        }
      status=SetImageExtent(image,image->columns,image->rows);
      if (status == MagickFalse)
        {
          InheritException(exception,&image->exception);
          return(DestroyImageList(image));
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
      hDC=GetDC(NULL);
      if (hDC == 0)
        ThrowReaderException(CoderError,"UnableToCreateADC");
      hBitmap=CreateDIBSection(hDC,&DIBinfo,DIB_RGB_COLORS,(void **) &ppBits,
        NULL,0);
      ReleaseDC(NULL,hDC);
      if (hBitmap == 0)
        ThrowReaderException(CoderError,"UnableToCreateBitmap");
      /* create an offscreen DC */
      hDC=CreateCompatibleDC(NULL);
      if (hDC == 0)
        {
          DeleteObject(hBitmap);
          ThrowReaderException(CoderError,"UnableToCreateADC");
        }
      hOldBitmap=(HBITMAP) SelectObject(hDC,hBitmap);
      if (hOldBitmap == 0)
        {
          DeleteDC(hDC);
          DeleteObject(hBitmap);
          ThrowReaderException(CoderError,"UnableToCreateBitmap");
        }
      if (hPal != NULL)
      {
        /* Kenichi Masuko says this needed */
        SelectPalette(hDC, hPal, FALSE);
        RealizePalette(hDC);
      }
      /* bitblt from the memory to the DIB-based one */
      BitBlt(hDC,0,0,(int) image->columns,(int) image->rows,hMemDC,0,0,SRCCOPY);
      /* finally copy the pixels! */
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
      DeleteDC(hDC);
      DeleteObject(hBitmap);
    }
  }
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
