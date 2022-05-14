void PrintWebViewHelper::PrintPage(const ViewMsg_PrintPage_Params& params,
                                   const gfx::Size& canvas_size,
                                   WebFrame* frame) {
  printing::NativeMetafile metafile;

  metafile.CreateDc(NULL, NULL);
  HDC hdc = metafile.hdc();
   DCHECK(hdc);
   skia::PlatformDevice::InitializeDC(hdc);
 
  int page_number = params.page_number;

   double content_width_in_points;
   double content_height_in_points;
  GetPageSizeAndMarginsInPoints(frame, page_number, params.params,
      &content_width_in_points, &content_height_in_points, NULL, NULL, NULL,
      NULL);
 
  float scale_factor = static_cast<float>(params.params.desired_dpi /
                                          params.params.dpi);

  // Since WebKit extends the page width depending on the magical |scale_factor|
  // we make sure the canvas covers the worst case scenario (x2.0 currently).
  // PrintContext will then set the correct clipping region.
  int width = static_cast<int>(content_width_in_points *
                               params.params.max_shrink);
  int height = static_cast<int>(content_height_in_points *
                                params.params.max_shrink);
 #if 0
 
  skia::PlatformCanvas canvas(width, height, true);
   canvas.drawARGB(255, 255, 255, 255, SkXfermode::kSrc_Mode);
  float webkit_scale_factor = frame->printPage(page_number, &canvas);
  if (scale_factor <= 0 || webkit_scale_factor <= 0) {
    NOTREACHED() << "Printing page " << page_number << " failed.";
   } else {
    // Update the dpi adjustment with the "page |scale_factor|" calculated
    // in webkit.
    scale_factor /= webkit_scale_factor;
   }
 
   BITMAPV4HEADER bitmap_header;
  gfx::CreateBitmapV4Header(width, height, &bitmap_header);
   const SkBitmap& src_bmp = canvas.getDevice()->accessBitmap(true);
   SkAutoLockPixels src_lock(src_bmp);
   int retval = StretchDIBits(hdc,
                              0,
                              0,
                             width, height,
                              0, 0,
                             width, height,
                              src_bmp.getPixels(),
                              reinterpret_cast<BITMAPINFO*>(&bitmap_header),
                              DIB_RGB_COLORS,
                              SRCCOPY);
   DCHECK(retval != GDI_ERROR);
 #else
  skia::VectorCanvas canvas(hdc, width, height);
  float webkit_scale_factor = frame->printPage(page_number, &canvas);
  if (scale_factor <= 0 || webkit_scale_factor <= 0) {
    NOTREACHED() << "Printing page " << page_number << " failed.";
   } else {
    // Update the dpi adjustment with the "page scale_factor" calculated
    // in webkit.
    scale_factor /= webkit_scale_factor;
   }
 #endif
 
  if (!metafile.CloseDc()) {
    NOTREACHED() << "metafile failed";
  }
  printing::NativeMetafile* mf = &metafile;
  printing::NativeMetafile metafile2;

  skia::VectorPlatformDevice* platform_device =
    static_cast<skia::VectorPlatformDevice*>(canvas.getDevice());
  if (platform_device->alpha_blend_used() &&
      !params.params.supports_alpha_blend) {
    HDC bitmap_dc = CreateCompatibleDC(GetDC(NULL));
    if (!bitmap_dc) {
      NOTREACHED() << "Bitmap DC creation failed";
    }
     SetGraphicsMode(bitmap_dc, GM_ADVANCED);
     void* bits = NULL;
     BITMAPINFO hdr;
    gfx::CreateBitmapHeader(width, height, &hdr.bmiHeader);
     HBITMAP hbitmap = CreateDIBSection(
         bitmap_dc, &hdr, DIB_RGB_COLORS, &bits, NULL, 0);
     if (!hbitmap) {
       NOTREACHED() << "Raster bitmap creation for printing failed";
     }
 
     HGDIOBJ old_bitmap = SelectObject(bitmap_dc, hbitmap);
    RECT rect = {0, 0, width, height };
     HBRUSH whiteBrush = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
     FillRect(bitmap_dc, &rect, whiteBrush);
 
    metafile2.CreateDc(NULL, NULL);
    HDC hdc = metafile2.hdc();
    DCHECK(hdc);
    skia::PlatformDevice::InitializeDC(hdc);

    RECT metafile_bounds = metafile.GetBounds().ToRECT();
    EnumEnhMetaFile(hdc,
                    metafile.emf(),
                    EnhMetaFileProc,
                    &bitmap_dc,
                    &metafile_bounds);

    SelectObject(bitmap_dc, old_bitmap);

    if (!metafile2.CloseDc()) {
      NOTREACHED() << "metafile failed";
    }
    mf = &metafile2;
  }

  uint32 buf_size = mf->GetDataSize();
  DCHECK_GT(buf_size, 128u);
   ViewHostMsg_DidPrintPage_Params page_params;
   page_params.data_size = 0;
   page_params.metafile_data_handle = NULL;
  page_params.page_number = page_number;
   page_params.document_cookie = params.params.document_cookie;
  page_params.actual_shrink = scale_factor;
  page_params.page_size = params.params.page_size;
  page_params.content_area = gfx::Rect(params.params.margin_left,
      params.params.margin_top, params.params.printable_size.width(),
      params.params.printable_size.height());
  page_params.has_visible_overlays = frame->isPageBoxVisible(page_number);
   base::SharedMemory shared_buf;
 
  if (buf_size < 350*1024*1024) {
    if (shared_buf.CreateAndMapAnonymous(buf_size)) {
      if (mf->GetData(shared_buf.memory(), buf_size)) {
        page_params.metafile_data_handle = shared_buf.handle();
        page_params.data_size = buf_size;
      } else {
        NOTREACHED() << "GetData() failed";
      }
      shared_buf.Unmap();
    } else {
      NOTREACHED() << "Buffer allocation failed";
    }
  } else {
    NOTREACHED() << "Buffer too large: " << buf_size;
  }
  mf->CloseEmf();
  if (Send(new ViewHostMsg_DuplicateSection(
      routing_id(),
      page_params.metafile_data_handle,
      &page_params.metafile_data_handle))) {
    if (!is_preview_) {
      Send(new ViewHostMsg_DidPrintPage(routing_id(), page_params));
    }
  }
}
