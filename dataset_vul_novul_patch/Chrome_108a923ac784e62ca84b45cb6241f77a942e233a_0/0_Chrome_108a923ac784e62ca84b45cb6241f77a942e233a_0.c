 bool scoped_pixel_buffer_object::Init(CGLContextObj cgl_context,
                                       int size_in_bytes) {
  // The PBO path is only done on 10.6 (SnowLeopard) and above due to
  // a driver issue that was found on 10.5
  // (specifically on a NVIDIA GeForce 7300 GT).
  // http://crbug.com/87283
  if (base::mac::IsOSLeopardOrEarlier()) {
    return false;
  }
   cgl_context_ = cgl_context;
   CGLContextObj CGL_MACRO_CONTEXT = cgl_context_;
   glGenBuffersARB(1, &pixel_buffer_object_);
  if (glGetError() == GL_NO_ERROR) {
    glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pixel_buffer_object_);
    glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, size_in_bytes, NULL,
                    GL_STREAM_READ_ARB);
    glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
    if (glGetError() != GL_NO_ERROR) {
      Release();
    }
  } else {
    cgl_context_ = NULL;
    pixel_buffer_object_ = 0;
  }
  return pixel_buffer_object_ != 0;
}
