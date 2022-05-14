void WebGL2RenderingContextBase::texSubImage2D(GLenum target,
                                               GLint level,
                                               GLint xoffset,
                                               GLint yoffset,
                                               GLsizei width,
                                               GLsizei height,
                                               GLenum format,
                                               GLenum type,
                                               GLintptr offset) {
  if (isContextLost())
    return;
  if (!ValidateTexture2DBinding("texSubImage2D", target))
    return;
  if (!bound_pixel_unpack_buffer_) {
    SynthesizeGLError(GL_INVALID_OPERATION, "texSubImage2D",
                       "no bound PIXEL_UNPACK_BUFFER");
     return;
   }
  if (unpack_flip_y_ || unpack_premultiply_alpha_) {
    SynthesizeGLError(
        GL_INVALID_OPERATION, "texSubImage2D",
        "FLIP_Y or PREMULTIPLY_ALPHA isn't allowed while uploading from PBO");
    return;
  }
   if (!ValidateTexFunc("texSubImage2D", kTexSubImage, kSourceUnpackBuffer,
                        target, level, 0, width, height, 1, 0, format, type,
                        xoffset, yoffset, 0))
    return;
  if (!ValidateValueFitNonNegInt32("texSubImage2D", "offset", offset))
    return;

  ContextGL()->TexSubImage2D(target, level, xoffset, yoffset, width, height,
                             format, type,
                             reinterpret_cast<const void*>(offset));
}
