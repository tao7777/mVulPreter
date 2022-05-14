PassRefPtr<DrawingBuffer> DrawingBuffer::Create(
    std::unique_ptr<WebGraphicsContext3DProvider> context_provider,
    Client* client,
    const IntSize& size,
    bool premultiplied_alpha,
    bool want_alpha_channel,
    bool want_depth_buffer,
     bool want_stencil_buffer,
     bool want_antialiasing,
     PreserveDrawingBuffer preserve,
    WebGLVersion webgl_version,
     ChromiumImageUsage chromium_image_usage,
     const CanvasColorParams& color_params) {
   DCHECK(context_provider);

  if (g_should_fail_drawing_buffer_creation_for_testing) {
    g_should_fail_drawing_buffer_creation_for_testing = false;
    return nullptr;
  }

  std::unique_ptr<Extensions3DUtil> extensions_util =
      Extensions3DUtil::Create(context_provider->ContextGL());
  if (!extensions_util->IsValid()) {
    return nullptr;
  }
  DCHECK(extensions_util->SupportsExtension("GL_OES_packed_depth_stencil"));
  extensions_util->EnsureExtensionEnabled("GL_OES_packed_depth_stencil");
  bool multisample_supported =
      want_antialiasing &&
      (extensions_util->SupportsExtension(
           "GL_CHROMIUM_framebuffer_multisample") ||
       extensions_util->SupportsExtension(
           "GL_EXT_multisampled_render_to_texture")) &&
      extensions_util->SupportsExtension("GL_OES_rgb8_rgba8");
  if (multisample_supported) {
    extensions_util->EnsureExtensionEnabled("GL_OES_rgb8_rgba8");
    if (extensions_util->SupportsExtension(
            "GL_CHROMIUM_framebuffer_multisample"))
      extensions_util->EnsureExtensionEnabled(
          "GL_CHROMIUM_framebuffer_multisample");
    else
      extensions_util->EnsureExtensionEnabled(
          "GL_EXT_multisampled_render_to_texture");
  }
  bool discard_framebuffer_supported =
      extensions_util->SupportsExtension("GL_EXT_discard_framebuffer");
  if (discard_framebuffer_supported)
    extensions_util->EnsureExtensionEnabled("GL_EXT_discard_framebuffer");

   RefPtr<DrawingBuffer> drawing_buffer = AdoptRef(new DrawingBuffer(
       std::move(context_provider), std::move(extensions_util), client,
       discard_framebuffer_supported, want_alpha_channel, premultiplied_alpha,
      preserve, webgl_version, want_depth_buffer, want_stencil_buffer,
       chromium_image_usage, color_params));
   if (!drawing_buffer->Initialize(size, multisample_supported)) {
     drawing_buffer->BeginDestruction();
    return PassRefPtr<DrawingBuffer>();
  }
  return drawing_buffer;
}
