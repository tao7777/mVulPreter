 gfx::Rect GetWindowRect(GdkWindow* window) {
   gint width, height;
   gdk_drawable_get_size(GDK_DRAWABLE(window), &width, &height);
  return gfx::Rect(width, height);
 }
