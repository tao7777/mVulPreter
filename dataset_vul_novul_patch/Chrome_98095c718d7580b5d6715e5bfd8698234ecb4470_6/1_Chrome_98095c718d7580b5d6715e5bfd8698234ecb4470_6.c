 GLboolean WebGLRenderingContextBase::isFramebuffer(
     WebGLFramebuffer* framebuffer) {
  if (!framebuffer || isContextLost())
     return 0;
 
   if (!framebuffer->HasEverBeenBound())
    return 0;
  if (framebuffer->IsDeleted())
    return 0;

  return ContextGL()->IsFramebuffer(framebuffer->Object());
 }
