void BackingStoreWin::PaintToBackingStore(
    RenderProcessHost* process,
    TransportDIB::Id bitmap,
    const gfx::Rect& bitmap_rect,
    const std::vector<gfx::Rect>& copy_rects,
    bool* painted_synchronously) {
  *painted_synchronously = true;

  if (!backing_store_dib_) {
    backing_store_dib_ = CreateDIB(hdc_, size().width(),
                                   size().height(), color_depth_);
    if (!backing_store_dib_) {
      NOTREACHED();
      return;
    }
    original_bitmap_ = SelectObject(hdc_, backing_store_dib_);
  }

  TransportDIB* dib = process->GetTransportDIB(bitmap);
  if (!dib)
    return;

   BITMAPINFOHEADER hdr;
   gfx::CreateBitmapHeader(bitmap_rect.width(), bitmap_rect.height(), &hdr);
  gfx::Rect view_rect(0, 0, size().width(), size().height());
 
   for (size_t i = 0; i < copy_rects.size(); i++) {
     gfx::Rect paint_rect = view_rect.Intersect(copy_rects[i]);
    CallStretchDIBits(hdc_,
                      paint_rect.x(),
                      paint_rect.y(),
                      paint_rect.width(),
                      paint_rect.height(),
                      paint_rect.x() - bitmap_rect.x(),
                      paint_rect.y() - bitmap_rect.y(),
                      paint_rect.width(),
                      paint_rect.height(),
                      dib->memory(),
                      reinterpret_cast<BITMAPINFO*>(&hdr));
  }
}
