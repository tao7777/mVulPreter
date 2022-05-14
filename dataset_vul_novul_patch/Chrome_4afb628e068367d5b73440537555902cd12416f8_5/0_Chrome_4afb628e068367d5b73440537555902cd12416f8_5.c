bool GLSurfaceEGLSurfaceControl::Resize(const gfx::Size& size,
                                         float scale_factor,
                                         ColorSpace color_space,
                                         bool has_alpha) {
  window_rect_ = gfx::Rect(0, 0, size.width(), size.height());
   return true;
 }
