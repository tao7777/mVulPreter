void RenderWidgetHostViewAura::CopyFromCompositingSurface(
    const gfx::Rect& src_subrect,
    const gfx::Size& dst_size,
    const base::Callback<void(bool)>& callback,
    skia::PlatformBitmap* output) {
  base::ScopedClosureRunner scoped_callback_runner(base::Bind(callback, false));

  std::map<uint64, scoped_refptr<ui::Texture> >::iterator it =
      image_transport_clients_.find(current_surface_);
  if (it == image_transport_clients_.end())
    return;

  ui::Texture* container = it->second;
  DCHECK(container);

  gfx::Size dst_size_in_pixel = ConvertSizeToPixel(this, dst_size);
  if (!output->Allocate(
      dst_size_in_pixel.width(), dst_size_in_pixel.height(), true))
    return;

  ImageTransportFactory* factory = ImageTransportFactory::GetInstance();
  GLHelper* gl_helper = factory->GetGLHelper();
  if (!gl_helper)
    return;

  unsigned char* addr = static_cast<unsigned char*>(
       output->GetBitmap().getPixels());
   scoped_callback_runner.Release();
  // own completion handlers (where we can try to free the frontbuffer).
   base::Callback<void(bool)> wrapper_callback = base::Bind(
       &RenderWidgetHostViewAura::CopyFromCompositingSurfaceFinished,
       AsWeakPtr(),
      callback);
  ++pending_thumbnail_tasks_;

  gfx::Rect src_subrect_in_gl = src_subrect;
  src_subrect_in_gl.set_y(GetViewBounds().height() - src_subrect.bottom());

  gfx::Rect src_subrect_in_pixel = ConvertRectToPixel(this, src_subrect_in_gl);
  gl_helper->CropScaleReadbackAndCleanTexture(container->PrepareTexture(),
                                              container->size(),
                                              src_subrect_in_pixel,
                                              dst_size_in_pixel,
                                              addr,
                                              wrapper_callback);
}
