void GpuProcessHostUIShim::OnResizeView(int32 surface_id,
                                        int32 route_id,
                                        gfx::Size size) {
  ScopedSendOnIOThread delayed_send(
      host_id_,
      new AcceleratedSurfaceMsg_ResizeViewACK(route_id));

  RenderWidgetHostViewPort* view =
      GetRenderWidgetHostViewFromSurfaceID(surface_id);
  if (!view)
    return;

  gfx::GLSurfaceHandle surface = view->GetCompositingSurface();

#if defined(TOOLKIT_GTK)
  GdkWindow* window = reinterpret_cast<GdkWindow*>(
      gdk_xid_table_lookup(surface.handle));
  if (window) {
    Display* display = GDK_WINDOW_XDISPLAY(window);
    gdk_window_resize(window, size.width(), size.height());
    XSync(display, False);
  }
#elif defined(OS_WIN)
  SetWindowPos(surface.handle,
      NULL,
      0, 0,
       std::max(1, size.width()),
       std::max(1, size.height()),
       SWP_NOSENDCHANGING | SWP_NOCOPYBITS | SWP_NOZORDER |
          SWP_NOACTIVATE | SWP_DEFERERASE);
 #endif
 }
