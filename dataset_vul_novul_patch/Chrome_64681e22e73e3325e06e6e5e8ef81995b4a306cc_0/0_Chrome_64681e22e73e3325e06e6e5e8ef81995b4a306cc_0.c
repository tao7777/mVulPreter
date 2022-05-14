void WebGraphicsContext3DCommandBufferImpl::reshape(int width, int height) {
  cached_width_ = width;
   cached_height_ = height;
 
   gl_->ResizeCHROMIUM(width, height);
 }
