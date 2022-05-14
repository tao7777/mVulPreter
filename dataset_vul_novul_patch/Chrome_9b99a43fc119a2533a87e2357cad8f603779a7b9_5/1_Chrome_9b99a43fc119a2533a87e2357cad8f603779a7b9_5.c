void WebGL2RenderingContextBase::texSubImage3D(GLenum target,
                                               GLint level,
                                               GLint xoffset,
                                               GLint yoffset,
                                               GLint zoffset,
                                               GLsizei width,
                                               GLsizei height,
                                               GLsizei depth,
                                               GLenum format,
                                               GLenum type,
                                               GLintptr offset) {
  if (isContextLost())
    return;
  if (!ValidateTexture3DBinding("texSubImage3D", target))
    return;
  if (!bound_pixel_unpack_buffer_) {
    SynthesizeGLError(GL_INVALID_OPERATION, "texSubImage3D",
                       "no bound PIXEL_UNPACK_BUFFER");
     return;
   }
   if (!ValidateTexFunc("texSubImage3D", kTexSubImage, kSourceUnpackBuffer,
                        target, level, 0, width, height, depth, 0, format, type,
                        xoffset, yoffset, zoffset))
    return;
  if (!ValidateValueFitNonNegInt32("texSubImage3D", "offset", offset))
    return;

  ContextGL()->TexSubImage3D(target, level, xoffset, yoffset, zoffset, width,
                             height, depth, format, type,
                             reinterpret_cast<const void*>(offset));
}
