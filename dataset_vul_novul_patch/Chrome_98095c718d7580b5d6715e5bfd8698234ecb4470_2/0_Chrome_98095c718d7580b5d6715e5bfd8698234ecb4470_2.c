void WebGL2RenderingContextBase::framebufferTextureLayer(GLenum target,
                                                         GLenum attachment,
                                                         WebGLTexture* texture,
                                                         GLint level,
                                                         GLint layer) {
  if (isContextLost() || !ValidateFramebufferFuncParameters(
                              "framebufferTextureLayer", target, attachment))
     return;
   if (texture && !texture->Validate(ContextGroup(), this)) {
    SynthesizeGLError(GL_INVALID_OPERATION, "framebufferTextureLayer",
                      "texture does not belong to this context");
     return;
   }
   GLenum textarget = texture ? texture->GetTarget() : 0;
  if (texture) {
    if (textarget != GL_TEXTURE_3D && textarget != GL_TEXTURE_2D_ARRAY) {
      SynthesizeGLError(GL_INVALID_OPERATION, "framebufferTextureLayer",
                        "invalid texture type");
      return;
    }
    if (!ValidateTexFuncLayer("framebufferTextureLayer", textarget, layer))
      return;
    if (!ValidateTexFuncLevel("framebufferTextureLayer", textarget, level))
      return;
  }

  WebGLFramebuffer* framebuffer_binding = GetFramebufferBinding(target);
  if (!framebuffer_binding || !framebuffer_binding->Object()) {
    SynthesizeGLError(GL_INVALID_OPERATION, "framebufferTextureLayer",
                      "no framebuffer bound");
    return;
  }
  if (framebuffer_binding && framebuffer_binding->Opaque()) {
    SynthesizeGLError(GL_INVALID_OPERATION, "framebufferTextureLayer",
                      "opaque framebuffer bound");
    return;
  }
  framebuffer_binding->SetAttachmentForBoundFramebuffer(
      target, attachment, textarget, texture, level, layer);
  ApplyStencilTest();
}
