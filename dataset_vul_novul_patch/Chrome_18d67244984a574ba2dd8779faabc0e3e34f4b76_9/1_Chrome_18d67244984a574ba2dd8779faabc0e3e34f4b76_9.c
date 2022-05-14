  ImageTransportClientTexture(
      WebKit::WebGraphicsContext3D* host_context,
      const gfx::Size& size,
      float device_scale_factor,
      uint64 surface_id)
          : ui::Texture(true, size, device_scale_factor),
            host_context_(host_context),
            texture_id_(surface_id) {
  }
