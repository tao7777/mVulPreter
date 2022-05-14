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
  if (unpack_flip_y_ || unpack_premultiply_alpha_) {
    DCHECK(pixels);
    SynthesizeGLError(
        GL_INVALID_OPERATION, "texSubImage3D",
        "FLIP_Y or PREMULTIPLY_ALPHA isn't allowed for uploading 3D textures");
    return;
  }

   TexImageHelperDOMArrayBufferView(
       kTexSubImage3D, target, level, 0, width, height, depth, 0, format, type,
       xoffset, yoffset, zoffset, pixels.View(), kNullNotReachable, src_offset);
}
