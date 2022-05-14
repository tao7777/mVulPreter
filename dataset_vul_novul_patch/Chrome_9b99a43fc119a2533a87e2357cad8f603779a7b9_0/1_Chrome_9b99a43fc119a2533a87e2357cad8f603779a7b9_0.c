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
   TexImageHelperDOMArrayBufferView(kTexImage3D, target, level, internalformat,
                                    width, height, depth, border, format, type,
                                    0, 0, 0, pixels.View(), kNullAllowed, 0);
}
