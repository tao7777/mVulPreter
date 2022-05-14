 GLboolean WebGLRenderingContextBase::isBuffer(WebGLBuffer* buffer) {
  if (!buffer || isContextLost() || !buffer->Validate(ContextGroup(), this))
     return 0;
 
   if (!buffer->HasEverBeenBound())
    return 0;
  if (buffer->IsDeleted())
    return 0;

  return ContextGL()->IsBuffer(buffer->Object());
}
