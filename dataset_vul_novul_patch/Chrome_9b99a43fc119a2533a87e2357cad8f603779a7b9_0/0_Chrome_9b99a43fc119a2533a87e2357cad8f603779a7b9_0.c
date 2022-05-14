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
     MaybeShared<DOMArrayBufferView> pixels) {
  if ((unpack_flip_y_ || unpack_premultiply_alpha_) && pixels) {
    SynthesizeGLError(
        GL_INVALID_OPERATION, "texImage3D",
        "FLIP_Y or PREMULTIPLY_ALPHA isn't allowed for uploading 3D textures");
    return;
  }
   TexImageHelperDOMArrayBufferView(kTexImage3D, target, level, internalformat,
                                    width, height, depth, border, format, type,
                                    0, 0, 0, pixels.View(), kNullAllowed, 0);
}
