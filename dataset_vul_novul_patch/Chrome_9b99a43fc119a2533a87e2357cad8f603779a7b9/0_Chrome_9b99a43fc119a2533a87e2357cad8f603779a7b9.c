void WebGL2RenderingContextBase::texImage2D(GLenum target,
                                            GLint level,
                                            GLint internalformat,
                                            GLsizei width,
                                            GLsizei height,
                                            GLint border,
                                            GLenum format,
                                            GLenum type,
                                            GLintptr offset) {
  if (isContextLost())
    return;
  if (!ValidateTexture2DBinding("texImage2D", target))
    return;
  if (!bound_pixel_unpack_buffer_) {
    SynthesizeGLError(GL_INVALID_OPERATION, "texImage2D",
                       "no bound PIXEL_UNPACK_BUFFER");
     return;
   }
  if (unpack_flip_y_ || unpack_premultiply_alpha_) {
    SynthesizeGLError(
        GL_INVALID_OPERATION, "texImage2D",
        "FLIP_Y or PREMULTIPLY_ALPHA isn't allowed while uploading from PBO");
    return;
  }
   if (!ValidateTexFunc("texImage2D", kTexImage, kSourceUnpackBuffer, target,
                        level, internalformat, width, height, 1, border, format,
                        type, 0, 0, 0))
    return;
  if (!ValidateValueFitNonNegInt32("texImage2D", "offset", offset))
    return;

  ContextGL()->TexImage2D(
      target, level, ConvertTexInternalFormat(internalformat, type), width,
      height, border, format, type, reinterpret_cast<const void*>(offset));
}
