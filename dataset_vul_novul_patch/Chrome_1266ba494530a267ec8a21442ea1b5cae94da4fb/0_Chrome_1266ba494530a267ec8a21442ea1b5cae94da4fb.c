 bool GrabWindowSnapshot(gfx::NativeWindow window,
                         std::vector<unsigned char>* png_representation,
                         const gfx::Rect& snapshot_bounds) {
#if defined(OS_LINUX)
  // We use XGetImage() for Linux/ChromeOS for performance reasons.
  // See crbug.com/122720
  if (window->GetRootWindow()->GrabSnapshot(
          snapshot_bounds, png_representation))
    return true;
#endif  // OS_LINUX

   ui::Compositor* compositor = window->layer()->GetCompositor();
 
   gfx::Rect read_pixels_bounds = snapshot_bounds;

  read_pixels_bounds.set_origin(
      snapshot_bounds.origin().Add(window->bounds().origin()));
  gfx::Rect read_pixels_bounds_in_pixel =
      ui::ConvertRectToPixel(window->layer(), read_pixels_bounds);

  DCHECK_GE(compositor->size().width(), read_pixels_bounds_in_pixel.right());
  DCHECK_GE(compositor->size().height(), read_pixels_bounds_in_pixel.bottom());
  DCHECK_LE(0, read_pixels_bounds.x());
  DCHECK_LE(0, read_pixels_bounds.y());

  SkBitmap bitmap;
  if (!compositor->ReadPixels(&bitmap, read_pixels_bounds_in_pixel))
    return false;

  unsigned char* pixels = reinterpret_cast<unsigned char*>(bitmap.getPixels());

  gfx::PNGCodec::Encode(pixels, gfx::PNGCodec::FORMAT_BGRA,
                        read_pixels_bounds_in_pixel.size(),
                        bitmap.rowBytes(), true,
                        std::vector<gfx::PNGCodec::Comment>(),
                        png_representation);
  return true;
}
