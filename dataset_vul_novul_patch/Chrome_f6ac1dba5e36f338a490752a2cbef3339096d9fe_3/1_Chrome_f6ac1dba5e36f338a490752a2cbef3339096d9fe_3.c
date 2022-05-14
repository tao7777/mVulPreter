bool DrawingBuffer::Initialize(const IntSize& size, bool use_multisampling) {
  ScopedStateRestorer scoped_state_restorer(this);

  if (gl_->GetGraphicsResetStatusKHR() != GL_NO_ERROR) {
    return false;
  }

  gl_->GetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size_);

  int max_sample_count = 0;
  anti_aliasing_mode_ = kNone;
  if (use_multisampling) {
    gl_->GetIntegerv(GL_MAX_SAMPLES_ANGLE, &max_sample_count);
    anti_aliasing_mode_ = kMSAAExplicitResolve;
    if (extensions_util_->SupportsExtension(
            "GL_EXT_multisampled_render_to_texture")) {
      anti_aliasing_mode_ = kMSAAImplicitResolve;
    } else if (extensions_util_->SupportsExtension(
                   "GL_CHROMIUM_screen_space_antialiasing")) {
      anti_aliasing_mode_ = kScreenSpaceAntialiasing;
    }
  }
   storage_texture_supported_ =
      (web_gl_version_ > kWebGL1 ||
        extensions_util_->SupportsExtension("GL_EXT_texture_storage")) &&
       anti_aliasing_mode_ == kScreenSpaceAntialiasing;
   sample_count_ = std::min(4, max_sample_count);

  state_restorer_->SetFramebufferBindingDirty();
  gl_->GenFramebuffers(1, &fbo_);
  gl_->BindFramebuffer(GL_FRAMEBUFFER, fbo_);
  if (WantExplicitResolve()) {
    gl_->GenFramebuffers(1, &multisample_fbo_);
    gl_->BindFramebuffer(GL_FRAMEBUFFER, multisample_fbo_);
    gl_->GenRenderbuffers(1, &multisample_renderbuffer_);
  }
  if (!ResizeFramebufferInternal(size))
    return false;

  if (depth_stencil_buffer_) {
    DCHECK(WantDepthOrStencil());
    has_implicit_stencil_buffer_ = !want_stencil_;
  }

  if (gl_->GetGraphicsResetStatusKHR() != GL_NO_ERROR) {
    return false;
  }

  return true;
}
