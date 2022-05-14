bool DrawingBuffer::FinishPrepareTextureMailboxGpu(
     viz::TextureMailbox* out_mailbox,
     std::unique_ptr<cc::SingleReleaseCallback>* out_release_callback) {
   DCHECK(state_restorer_);
  if (web_gl_version_ > kWebGL1) {
     state_restorer_->SetPixelUnpackBufferBindingDirty();
     gl_->BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
   }

  RefPtr<ColorBuffer> color_buffer_for_mailbox;
  if (preserve_drawing_buffer_ == kDiscard) {
    color_buffer_for_mailbox = back_color_buffer_;
    back_color_buffer_ = CreateOrRecycleColorBuffer();
    AttachColorBufferToReadFramebuffer();

    if (discard_framebuffer_supported_) {
      const GLenum kAttachments[3] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT,
                                      GL_STENCIL_ATTACHMENT};
      state_restorer_->SetFramebufferBindingDirty();
      gl_->BindFramebuffer(GL_FRAMEBUFFER, fbo_);
      gl_->DiscardFramebufferEXT(GL_FRAMEBUFFER, 3, kAttachments);
    }
  } else {
    color_buffer_for_mailbox = CreateOrRecycleColorBuffer();
    gl_->CopySubTextureCHROMIUM(back_color_buffer_->texture_id, 0,
                                color_buffer_for_mailbox->parameters.target,
                                color_buffer_for_mailbox->texture_id, 0, 0, 0,
                                0, 0, size_.Width(), size_.Height(), GL_FALSE,
                                GL_FALSE, GL_FALSE);
  }

  {
    gl_->ProduceTextureDirectCHROMIUM(
        color_buffer_for_mailbox->texture_id,
        color_buffer_for_mailbox->parameters.target,
        color_buffer_for_mailbox->mailbox.name);
    const GLuint64 fence_sync = gl_->InsertFenceSyncCHROMIUM();
#if defined(OS_MACOSX)
    gl_->DescheduleUntilFinishedCHROMIUM();
#endif
    gl_->Flush();
    gl_->GenSyncTokenCHROMIUM(
        fence_sync, color_buffer_for_mailbox->produce_sync_token.GetData());
  }

  {
    bool is_overlay_candidate = color_buffer_for_mailbox->image_id != 0;
    bool secure_output_only = false;
    *out_mailbox = viz::TextureMailbox(
        color_buffer_for_mailbox->mailbox,
        color_buffer_for_mailbox->produce_sync_token,
        color_buffer_for_mailbox->parameters.target, gfx::Size(size_),
        is_overlay_candidate, secure_output_only);
    out_mailbox->set_color_space(color_space_);

    auto func =
        WTF::Bind(&DrawingBuffer::MailboxReleasedGpu,
                  RefPtr<DrawingBuffer>(this), color_buffer_for_mailbox);
    *out_release_callback = cc::SingleReleaseCallback::Create(
        ConvertToBaseCallback(std::move(func)));
  }

  front_color_buffer_ = color_buffer_for_mailbox;

  contents_changed_ = false;
  SetBufferClearNeeded(true);
  return true;
}
