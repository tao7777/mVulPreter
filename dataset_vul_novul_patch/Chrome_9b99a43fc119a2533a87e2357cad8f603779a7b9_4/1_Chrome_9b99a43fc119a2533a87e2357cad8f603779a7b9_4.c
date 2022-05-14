void WebGL2RenderingContextBase::texSubImage3D(
    GLenum target,
    GLint level,
    GLint xoffset,
    GLint yoffset,
    GLint zoffset,
    GLsizei width,
    GLsizei height,
    GLsizei depth,
    GLenum format,
    GLenum type,
    MaybeShared<DOMArrayBufferView> pixels,
    GLuint src_offset) {
  if (isContextLost())
    return;
  if (bound_pixel_unpack_buffer_) {
    SynthesizeGLError(GL_INVALID_OPERATION, "texSubImage3D",
                       "a buffer is bound to PIXEL_UNPACK_BUFFER");
     return;
   }
   TexImageHelperDOMArrayBufferView(
       kTexSubImage3D, target, level, 0, width, height, depth, 0, format, type,
       xoffset, yoffset, zoffset, pixels.View(), kNullNotReachable, src_offset);
}
