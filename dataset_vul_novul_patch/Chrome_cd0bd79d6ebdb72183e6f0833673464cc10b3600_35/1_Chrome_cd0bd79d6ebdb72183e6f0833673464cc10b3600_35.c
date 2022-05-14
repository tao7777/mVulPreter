void WebPluginProxy::CreateCanvasFromHandle(
     const TransportDIB::Handle& dib_handle,
     const gfx::Rect& window_rect,
     scoped_ptr<skia::PlatformCanvas>* canvas_out) {
  HANDLE section;
  DuplicateHandle(channel_->renderer_handle(), dib_handle, GetCurrentProcess(),
                  &section,
                  STANDARD_RIGHTS_REQUIRED | FILE_MAP_READ | FILE_MAP_WRITE,
                  FALSE, 0);
   scoped_ptr<skia::PlatformCanvas> canvas(new skia::PlatformCanvas);
   if (!canvas->initialize(
           window_rect.width(),
           window_rect.height(),
           true,
          section)) {
     canvas_out->reset();
   }
   canvas_out->reset(canvas.release());
  CloseHandle(section);
 }
