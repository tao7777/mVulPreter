void WebPluginProxy::CreateCanvasFromHandle(
     const TransportDIB::Handle& dib_handle,
     const gfx::Rect& window_rect,
     scoped_ptr<skia::PlatformCanvas>* canvas_out) {
   scoped_ptr<skia::PlatformCanvas> canvas(new skia::PlatformCanvas);
   if (!canvas->initialize(
           window_rect.width(),
           window_rect.height(),
           true,
          dib_handle)) {
     canvas_out->reset();
   }
   canvas_out->reset(canvas.release());
  CloseHandle(dib_handle);
 }
