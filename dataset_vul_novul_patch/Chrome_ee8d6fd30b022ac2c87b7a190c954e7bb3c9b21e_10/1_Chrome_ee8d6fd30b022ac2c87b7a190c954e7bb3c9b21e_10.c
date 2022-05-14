bool WebPluginDelegatePepper::PrintPage(int page_number,
                                        WebKit::WebCanvas* canvas) {
#if defined(OS_WIN) || defined(OS_LINUX)
  NPPPrintExtensions* print_extensions = GetPrintExtensions();
  if (!print_extensions)
    return false;

  DCHECK(!current_printable_area_.IsEmpty());

  NPRect np_printable_area = {0};
  np_printable_area.left = current_printable_area_.x();
  np_printable_area.top = current_printable_area_.y();
  np_printable_area.right =
      current_printable_area_.x() + current_printable_area_.width();
  np_printable_area.bottom =
      current_printable_area_.y() + current_printable_area_.height();
  gfx::Size size_in_pixels;
  if (!CalculatePrintedPageDimensions(page_number, print_extensions,
                                      &size_in_pixels)) {
    return false;
  }

   scoped_ptr<Graphics2DDeviceContext> g2d(new Graphics2DDeviceContext(this));
   NPDeviceContext2DConfig config;
   NPDeviceContext2D context;
  gfx::Rect surface_rect(gfx::Point(0, 0), size_in_pixels);
   NPError err = g2d->Initialize(surface_rect, &config, &context);
   if (err != NPERR_NO_ERROR) {
     NOTREACHED();
    return false;
  }
  err = print_extensions->printPageRaster(instance()->npp(), page_number,
                                          &context);
  if (err !=  NPERR_NO_ERROR)
    return false;

  SkBitmap committed;
  committed.setConfig(SkBitmap::kARGB_8888_Config, size_in_pixels.width(),
                      size_in_pixels.height());
  committed.allocPixels();
  err = g2d->Flush(&committed, &context, NULL, instance()->npp(), NULL);
  if (err !=  NPERR_NO_ERROR) {
    NOTREACHED();
    return false;
  }
  SkIRect src_rect;
  src_rect.set(0, 0, size_in_pixels.width(), size_in_pixels.height());
  SkRect dest_rect;
  dest_rect.set(SkIntToScalar(current_printable_area_.x()),
                SkIntToScalar(current_printable_area_.y()),
                SkIntToScalar(current_printable_area_.x() +
                              current_printable_area_.width()),
                SkIntToScalar(current_printable_area_.y() +
                              current_printable_area_.height()));
  bool draw_to_canvas = true;
#if defined(OS_WIN)
  static const int kCompressionThreshold = 20 * 1024 * 1024;
  if (committed.getSize() > kCompressionThreshold) {
    DrawJPEGToPlatformDC(committed, current_printable_area_, canvas);
    draw_to_canvas = false;
  }
#endif  // OS_WIN

  if (draw_to_canvas)
    canvas->drawBitmapRect(committed, &src_rect, dest_rect);

  return true;
#else  // defined(OS_WIN) || defined(OS_LINUX)
  NOTIMPLEMENTED();
  return false;
#endif  // defined(OS_WIN) || defined(OS_LINUX)
}
