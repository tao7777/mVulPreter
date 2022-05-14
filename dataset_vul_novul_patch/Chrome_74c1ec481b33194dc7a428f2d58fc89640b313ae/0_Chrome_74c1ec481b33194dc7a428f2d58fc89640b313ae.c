void GLES2DecoderImpl::DoGetFramebufferAttachmentParameteriv(
    GLenum target, GLenum attachment, GLenum pname, GLint* params) {
  if (!bound_framebuffer_) {
    SetGLError(GL_INVALID_OPERATION,
               "glFramebufferAttachmentParameteriv: no framebuffer bound");
     return;
   }
   glGetFramebufferAttachmentParameterivEXT(target, attachment, pname, params);
  if (pname == GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME) {
    GLint type = 0;
    GLuint client_id = 0;
    glGetFramebufferAttachmentParameterivEXT(
        target, attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &type);
    switch (type) {
      case GL_RENDERBUFFER: {
        renderbuffer_manager()->GetClientId(*params, &client_id);
        break;
      }
      case GL_TEXTURE: {
        texture_manager()->GetClientId(*params, &client_id);
        break;
      }
      default:
        break;
    }
    *params = client_id;
  }
 }
