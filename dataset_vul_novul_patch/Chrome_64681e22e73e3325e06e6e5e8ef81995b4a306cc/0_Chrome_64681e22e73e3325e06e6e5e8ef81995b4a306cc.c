 void WebGraphicsContext3DCommandBufferImpl::FlipVertically(
     uint8* framebuffer,
     unsigned int width,
     unsigned int height) {
  if (width == 0)
     return;
  scanline_.resize(width * 4);
  uint8* scanline = &scanline_[0];
   unsigned int row_bytes = width * 4;
   unsigned int count = height / 2;
   for (unsigned int i = 0; i < count; i++) {
    uint8* row_a = framebuffer + i * row_bytes;
    uint8* row_b = framebuffer + (height - i - 1) * row_bytes;
    memcpy(scanline, row_b, row_bytes);
    memcpy(row_b, row_a, row_bytes);
    memcpy(row_a, scanline, row_bytes);
  }
}
