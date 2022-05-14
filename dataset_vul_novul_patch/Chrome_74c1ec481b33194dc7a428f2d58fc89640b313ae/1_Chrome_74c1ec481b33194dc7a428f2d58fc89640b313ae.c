void GLES2DecoderImpl::DoGetFramebufferAttachmentParameteriv(
    GLenum target, GLenum attachment, GLenum pname, GLint* params) {
  if (!bound_framebuffer_) {
    SetGLError(GL_INVALID_OPERATION,
               "glFramebufferAttachmentParameteriv: no framebuffer bound");
     return;
   }
   glGetFramebufferAttachmentParameterivEXT(target, attachment, pname, params);
 }
