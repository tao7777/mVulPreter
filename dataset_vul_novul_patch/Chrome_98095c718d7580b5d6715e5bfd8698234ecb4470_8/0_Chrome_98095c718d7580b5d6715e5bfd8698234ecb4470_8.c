 GLboolean WebGLRenderingContextBase::isRenderbuffer(
     WebGLRenderbuffer* renderbuffer) {
  if (!renderbuffer || isContextLost() ||
      !renderbuffer->Validate(ContextGroup(), this))
     return 0;
 
   if (!renderbuffer->HasEverBeenBound())
    return 0;
  if (renderbuffer->IsDeleted())
    return 0;

  return ContextGL()->IsRenderbuffer(renderbuffer->Object());
 }
