DrawingBuffer::ScopedStateRestorer::~ScopedStateRestorer() {
  DCHECK_EQ(drawing_buffer_->state_restorer_, this);
  drawing_buffer_->state_restorer_ = previous_state_restorer_;
  Client* client = drawing_buffer_->client_;
  if (!client)
    return;

  if (clear_state_dirty_) {
     client->DrawingBufferClientRestoreScissorTest();
     client->DrawingBufferClientRestoreMaskAndClearValues();
   }
  if (pixel_pack_alignment_dirty_)
    client->DrawingBufferClientRestorePixelPackAlignment();
   if (texture_binding_dirty_)
     client->DrawingBufferClientRestoreTexture2DBinding();
   if (renderbuffer_binding_dirty_)
    client->DrawingBufferClientRestoreRenderbufferBinding();
  if (framebuffer_binding_dirty_)
     client->DrawingBufferClientRestoreFramebufferBinding();
   if (pixel_unpack_buffer_binding_dirty_)
     client->DrawingBufferClientRestorePixelUnpackBufferBinding();
 }
