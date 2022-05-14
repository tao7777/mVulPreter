void WebGL2RenderingContextBase::texImage3D(
    GLenum target,
    GLint level,
    GLint internalformat,
    GLsizei width,
    GLsizei height,
    GLsizei depth,
    GLint border,
    GLenum format,
    GLenum type,
    MaybeShared<DOMArrayBufferView> pixels,
    GLuint src_offset) {
  if (isContextLost())
    return;
  if (bound_pixel_unpack_buffer_) {
    SynthesizeGLError(GL_INVALID_OPERATION, "texImage3D",
                       "a buffer is bound to PIXEL_UNPACK_BUFFER");
     return;
   }
   TexImageHelperDOMArrayBufferView(
       kTexImage3D, target, level, internalformat, width, height, depth, border,
       format, type, 0, 0, 0, pixels.View(), kNullNotReachable, src_offset);
}
