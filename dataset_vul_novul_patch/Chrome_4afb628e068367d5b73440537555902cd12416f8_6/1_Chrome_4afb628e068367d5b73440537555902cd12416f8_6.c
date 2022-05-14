bool GLSurfaceEGLSurfaceControl::ScheduleOverlayPlane(
    int z_order,
    gfx::OverlayTransform transform,
    GLImage* image,
    const gfx::Rect& bounds_rect,
    const gfx::RectF& crop_rect,
    bool enable_blend,
    std::unique_ptr<gfx::GpuFence> gpu_fence) {
  if (!pending_transaction_)
    pending_transaction_.emplace();

  bool uninitialized = false;
  if (pending_surfaces_count_ == surface_list_.size()) {
    uninitialized = true;
    surface_list_.emplace_back(*root_surface_);
  }
  pending_surfaces_count_++;
  auto& surface_state = surface_list_.at(pending_surfaces_count_ - 1);

  if (uninitialized || surface_state.z_order != z_order) {
    surface_state.z_order = z_order;
    pending_transaction_->SetZOrder(*surface_state.surface, z_order);
  }

  AHardwareBuffer* hardware_buffer = nullptr;
  base::ScopedFD fence_fd;
  auto scoped_hardware_buffer = image->GetAHardwareBuffer();
  if (scoped_hardware_buffer) {
    hardware_buffer = scoped_hardware_buffer->buffer();
    fence_fd = scoped_hardware_buffer->TakeFence();

    auto* a_surface = surface_state.surface->surface();
    DCHECK_EQ(pending_frame_resources_.count(a_surface), 0u);

    auto& resource_ref = pending_frame_resources_[a_surface];
    resource_ref.surface = surface_state.surface;
     resource_ref.scoped_buffer = std::move(scoped_hardware_buffer);
   }
 
  if (uninitialized || surface_state.hardware_buffer != hardware_buffer) {
     surface_state.hardware_buffer = hardware_buffer;
 
     if (!fence_fd.is_valid() && gpu_fence && surface_state.hardware_buffer) {
      auto fence_handle =
          gfx::CloneHandleForIPC(gpu_fence->GetGpuFenceHandle());
      DCHECK(!fence_handle.is_null());
      fence_fd = base::ScopedFD(fence_handle.native_fd.fd);
    }

    pending_transaction_->SetBuffer(*surface_state.surface,
                                    surface_state.hardware_buffer,
                                    std::move(fence_fd));
  }

  if (hardware_buffer) {
    gfx::Rect dst = bounds_rect;

    gfx::Size buffer_size = GetBufferSize(hardware_buffer);
    gfx::RectF scaled_rect =
        gfx::RectF(crop_rect.x() * buffer_size.width(),
                   crop_rect.y() * buffer_size.height(),
                   crop_rect.width() * buffer_size.width(),
                   crop_rect.height() * buffer_size.height());
    gfx::Rect src = gfx::ToEnclosedRect(scaled_rect);

    if (uninitialized || surface_state.src != src || surface_state.dst != dst ||
        surface_state.transform != transform) {
      surface_state.src = src;
      surface_state.dst = dst;
      surface_state.transform = transform;
      pending_transaction_->SetGeometry(*surface_state.surface, src, dst,
                                        transform);
    }
  }

  bool opaque = !enable_blend;
  if (uninitialized || surface_state.opaque != opaque) {
    surface_state.opaque = opaque;
    pending_transaction_->SetOpaque(*surface_state.surface, opaque);
  }

  return true;
}
