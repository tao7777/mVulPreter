void WebGLRenderingContextBase::DrawingBufferClientRestorePixelPackAlignment() {
   if (!ContextGL())
     return;
   ContextGL()->PixelStorei(GL_PACK_ALIGNMENT, pack_alignment_);
}
