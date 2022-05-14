 GLboolean WebGLRenderingContextBase::isTexture(WebGLTexture* texture) {
  if (!texture || isContextLost())
     return 0;
 
   if (!texture->HasEverBeenBound())
    return 0;
  if (texture->IsDeleted())
    return 0;

  return ContextGL()->IsTexture(texture->Object());
}
