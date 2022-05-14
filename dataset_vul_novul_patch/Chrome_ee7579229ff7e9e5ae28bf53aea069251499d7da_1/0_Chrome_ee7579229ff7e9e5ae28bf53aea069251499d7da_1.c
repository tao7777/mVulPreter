void GLES2DecoderImpl::ClearUnclearedAttachments(
    GLenum target, Framebuffer* framebuffer) {
  if (target == GL_READ_FRAMEBUFFER_EXT) {
    glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
     glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, framebuffer->service_id());
   }
   GLbitfield clear_bits = 0;
  if (framebuffer->HasUnclearedColorAttachments()) {
     glClearColor(
         0.0f, 0.0f, 0.0f,
         (GLES2Util::GetChannelsForFormat(
              framebuffer->GetColorAttachmentFormat()) & 0x0008) != 0 ? 0.0f :
                                                                        1.0f);
     state_.SetDeviceColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
     clear_bits |= GL_COLOR_BUFFER_BIT;
    framebuffer->PrepareDrawBuffersForClear();
   }
 
   if (framebuffer->HasUnclearedAttachment(GL_STENCIL_ATTACHMENT) ||
      framebuffer->HasUnclearedAttachment(GL_DEPTH_STENCIL_ATTACHMENT)) {
    glClearStencil(0);
    state_.SetDeviceStencilMaskSeparate(GL_FRONT, -1);
    state_.SetDeviceStencilMaskSeparate(GL_BACK, -1);
    clear_bits |= GL_STENCIL_BUFFER_BIT;
  }

  if (framebuffer->HasUnclearedAttachment(GL_DEPTH_ATTACHMENT) ||
      framebuffer->HasUnclearedAttachment(GL_DEPTH_STENCIL_ATTACHMENT)) {
    glClearDepth(1.0f);
    state_.SetDeviceDepthMask(GL_TRUE);
    clear_bits |= GL_DEPTH_BUFFER_BIT;
  }

   state_.SetDeviceCapabilityState(GL_SCISSOR_TEST, false);
   glClear(clear_bits);
 
  if ((clear_bits | GL_COLOR_BUFFER_BIT) != 0)
    framebuffer->RestoreDrawBuffersAfterClear();

   framebuffer_manager()->MarkAttachmentsAsCleared(
       framebuffer, renderbuffer_manager(), texture_manager());
 
  RestoreClearState();

  if (target == GL_READ_FRAMEBUFFER_EXT) {
    glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, framebuffer->service_id());
    Framebuffer* draw_framebuffer =
        GetFramebufferInfoForTarget(GL_DRAW_FRAMEBUFFER_EXT);
    GLuint service_id = draw_framebuffer ? draw_framebuffer->service_id() :
                                           GetBackbufferServiceId();
    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, service_id);
  }
}
