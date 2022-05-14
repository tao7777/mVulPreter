void PrintWebViewHelper::PrintPage(const ViewMsg_PrintPage_Params& params,
                                   const gfx::Size& canvas_size,
                                   WebFrame* frame) {
  printing::NativeMetafile metafile;

  metafile.CreateDc(NULL, NULL);
  HDC hdc = metafile.hdc();
   DCHECK(hdc);
   skia::PlatformDevice::InitializeDC(hdc);
 
   double content_width_in_points;
   double content_height_in_points;
  double margin_top_in_points;
  double margin_right_in_points;
  double margin_bottom_in_points;
  double margin_left_in_points;
  GetPageSizeAndMarginsInPoints(frame,
                                params.page_number,
                                params.params,
                                &content_width_in_points,
                                &content_height_in_points,
                                &margin_top_in_points,
                                &margin_right_in_points,
                                &margin_bottom_in_points,
                                &margin_left_in_points);
 
  int size_x = static_cast<int>(content_width_in_points *
                                params.params.max_shrink);
  int size_y = static_cast<int>(content_height_in_points *
                                params.params.max_shrink);
  float shrink = static_cast<float>(params.params.desired_dpi /
                                    params.params.dpi);
 #if 0
 
  skia::PlatformCanvas canvas(size_x, size_y, true);
   canvas.drawARGB(255, 255, 255, 255, SkXfermode::kSrc_Mode);
  float webkit_shrink = frame->printPage(params.page_number, &canvas);
  if (shrink <= 0 || webkit_shrink <= 0) {
    NOTREACHED() << "Printing page " << params.page_number << " failed.";
   } else {
    shrink /= webkit_shrink;
   }
 
   BITMAPV4HEADER bitmap_header;
  gfx::CreateBitmapV4Header(size_x, size_y, &bitmap_header);
   const SkBitmap& src_bmp = canvas.getDevice()->accessBitmap(true);
   SkAutoLockPixels src_lock(src_bmp);
   int retval = StretchDIBits(hdc,
                              0,
                              0,
                             size_x, size_y,
                              0, 0,
                             size_x, size_y,
                              src_bmp.getPixels(),
                              reinterpret_cast<BITMAPINFO*>(&bitmap_header),
                              DIB_RGB_COLORS,
                              SRCCOPY);
   DCHECK(retval != GDI_ERROR);
 #else
  skia::VectorCanvas canvas(hdc, size_x, size_y);
  float webkit_shrink = frame->printPage(params.page_number, &canvas);
  if (shrink <= 0 || webkit_shrink <= 0) {
    NOTREACHED() << "Printing page " << params.page_number << " failed.";
   } else {
    shrink /= webkit_shrink;
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
    gfx::CreateBitmapHeader(size_x, size_y, &hdr.bmiHeader);
     HBITMAP hbitmap = CreateDIBSection(
         bitmap_dc, &hdr, DIB_RGB_COLORS, &bits, NULL, 0);
     if (!hbitmap) {
       NOTREACHED() << "Raster bitmap creation for printing failed";
     }
 
     HGDIOBJ old_bitmap = SelectObject(bitmap_dc, hbitmap);
    RECT rect = {0, 0, size_x, size_y };
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
  page_params.page_number = params.page_number;
   page_params.document_cookie = params.params.document_cookie;
  page_params.actual_shrink = shrink;
  page_params.page_size = gfx::Size(
      static_cast<int>(ConvertUnitDouble(
          content_width_in_points +
          margin_left_in_points + margin_right_in_points,
          kPointsPerInch, params.params.dpi)),
      static_cast<int>(ConvertUnitDouble(
          content_height_in_points +
          margin_top_in_points + margin_bottom_in_points,
          kPointsPerInch, params.params.dpi)));
  page_params.content_area = gfx::Rect(
      static_cast<int>(ConvertUnitDouble(
          margin_left_in_points, kPointsPerInch, params.params.dpi)),
      static_cast<int>(ConvertUnitDouble(
          margin_top_in_points, kPointsPerInch, params.params.dpi)),
      static_cast<int>(ConvertUnitDouble(
          content_width_in_points, kPointsPerInch, params.params.dpi)),
      static_cast<int>(ConvertUnitDouble(
          content_height_in_points, kPointsPerInch, params.params.dpi)));
  page_params.has_visible_overlays =
      frame->isPageBoxVisible(params.page_number);
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
