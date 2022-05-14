void WebGraphicsContext3DCommandBufferImpl::reshape(int width, int height) {
  cached_width_ = width;
   cached_height_ = height;
 
   gl_->ResizeCHROMIUM(width, height);
#ifdef FLIP_FRAMEBUFFER_VERTICALLY
  scanline_.reset(new uint8[width * 4]);
#endif  // FLIP_FRAMEBUFFER_VERTICALLY
 }
