 void DrawingBuffer::RestoreAllState() {
   client_->DrawingBufferClientRestoreScissorTest();
   client_->DrawingBufferClientRestoreMaskAndClearValues();
  client_->DrawingBufferClientRestorePixelPackParameters();
   client_->DrawingBufferClientRestoreTexture2DBinding();
   client_->DrawingBufferClientRestoreRenderbufferBinding();
   client_->DrawingBufferClientRestoreFramebufferBinding();
   client_->DrawingBufferClientRestorePixelUnpackBufferBinding();
  client_->DrawingBufferClientRestorePixelPackBufferBinding();
 }
