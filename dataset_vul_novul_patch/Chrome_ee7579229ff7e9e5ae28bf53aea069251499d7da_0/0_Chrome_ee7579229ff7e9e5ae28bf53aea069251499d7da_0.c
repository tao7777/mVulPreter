bool GLES2DecoderImpl::CheckFramebufferValid(
    Framebuffer* framebuffer,
    GLenum target, const char* func_name) {
  if (!framebuffer) {
    if (backbuffer_needs_clear_bits_) {
      glClearColor(0, 0, 0, (GLES2Util::GetChannelsForFormat(
          offscreen_target_color_format_) & 0x0008) != 0 ? 0 : 1);
      state_.SetDeviceColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glClearStencil(0);
      state_.SetDeviceStencilMaskSeparate(GL_FRONT, -1);
      state_.SetDeviceStencilMaskSeparate(GL_BACK, -1);
       glClearDepth(1.0f);
       state_.SetDeviceDepthMask(GL_TRUE);
       state_.SetDeviceCapabilityState(GL_SCISSOR_TEST, false);
      bool reset_draw_buffer = false;
      if ((backbuffer_needs_clear_bits_ | GL_COLOR_BUFFER_BIT) != 0 &&
          group_->draw_buffer() == GL_NONE) {
        reset_draw_buffer = true;
        GLenum buf = GL_BACK;
        if (GetBackbufferServiceId() != 0)  // emulated backbuffer
          buf = GL_COLOR_ATTACHMENT0;
        glDrawBuffersARB(1, &buf);
      }
       glClear(backbuffer_needs_clear_bits_);
      if (reset_draw_buffer) {
        GLenum buf = GL_NONE;
        glDrawBuffersARB(1, &buf);
      }
       backbuffer_needs_clear_bits_ = 0;
       RestoreClearState();
     }
    return true;
  }

  if (framebuffer_manager()->IsComplete(framebuffer)) {
    return true;
  }

  GLenum completeness = framebuffer->IsPossiblyComplete();
  if (completeness != GL_FRAMEBUFFER_COMPLETE) {
    LOCAL_SET_GL_ERROR(
        GL_INVALID_FRAMEBUFFER_OPERATION, func_name, "framebuffer incomplete");
    return false;
  }

  if (renderbuffer_manager()->HaveUnclearedRenderbuffers() ||
      texture_manager()->HaveUnclearedMips()) {
    if (!framebuffer->IsCleared()) {
      if (framebuffer->GetStatus(texture_manager(), target) !=
          GL_FRAMEBUFFER_COMPLETE) {
        LOCAL_SET_GL_ERROR(
            GL_INVALID_FRAMEBUFFER_OPERATION, func_name,
            "framebuffer incomplete (clear)");
        return false;
      }
      ClearUnclearedAttachments(target, framebuffer);
    }
  }

  if (!framebuffer_manager()->IsComplete(framebuffer)) {
    if (framebuffer->GetStatus(texture_manager(), target) !=
        GL_FRAMEBUFFER_COMPLETE) {
      LOCAL_SET_GL_ERROR(
          GL_INVALID_FRAMEBUFFER_OPERATION, func_name,
          "framebuffer incomplete (check)");
      return false;
    }
    framebuffer_manager()->MarkAsComplete(framebuffer);
  }

  return true;
}
