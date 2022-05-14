 void RenderWidgetHostViewAura::AcceleratedSurfacePostSubBuffer(
     const GpuHostMsg_AcceleratedSurfacePostSubBuffer_Params& params_in_pixel,
     int gpu_host_id) {
  const gfx::Rect surface_rect =
      gfx::Rect(gfx::Point(), params_in_pixel.surface_size);
  gfx::Rect damage_rect(params_in_pixel.x,
                        params_in_pixel.y,
                        params_in_pixel.width,
                        params_in_pixel.height);
  BufferPresentedParams ack_params(
      params_in_pixel.route_id, gpu_host_id, params_in_pixel.surface_handle);
  if (!SwapBuffersPrepare(surface_rect, damage_rect, &ack_params))
     return;
 
  SkRegion damage(RectToSkIRect(damage_rect));
  if (!skipped_damage_.isEmpty()) {
    damage.op(skipped_damage_, SkRegion::kUnion_Op);
    skipped_damage_.setEmpty();
   }
 
  DCHECK(surface_rect.Contains(SkIRectToRect(damage.getBounds())));
  ui::Texture* current_texture = image_transport_clients_[current_surface_];
 
  const gfx::Size surface_size_in_pixel = params_in_pixel.surface_size;
  DLOG_IF(ERROR, ack_params.texture_to_produce &&
      ack_params.texture_to_produce->size() != current_texture->size() &&
      SkIRectToRect(damage.getBounds()) != surface_rect) <<
      "Expected full damage rect after size change";
  if (ack_params.texture_to_produce && !previous_damage_.isEmpty() &&
      ack_params.texture_to_produce->size() == current_texture->size()) {
    ImageTransportFactory* factory = ImageTransportFactory::GetInstance();
    GLHelper* gl_helper = factory->GetGLHelper();
    gl_helper->CopySubBufferDamage(
        current_texture->PrepareTexture(),
        ack_params.texture_to_produce->PrepareTexture(),
        damage,
        previous_damage_);
  }
  previous_damage_ = damage;
 
  ui::Compositor* compositor = GetCompositor();
  if (compositor) {
     gfx::Rect rect_to_paint = ConvertRectToDIP(this, gfx::Rect(
         params_in_pixel.x,
         surface_size_in_pixel.height() - params_in_pixel.y -
        params_in_pixel.height,
         params_in_pixel.width,
         params_in_pixel.height));
 
    rect_to_paint.Inset(-1, -1);
     rect_to_paint.Intersect(window_->bounds());
 
     window_->SchedulePaintInRect(rect_to_paint);
   }

  SwapBuffersCompleted(ack_params);
 }
