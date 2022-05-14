 GLboolean WebGLRenderingContextBase::isShader(WebGLShader* shader) {
  if (!shader || isContextLost() || !shader->Validate(ContextGroup(), this))
     return 0;
 
   return ContextGL()->IsShader(shader->Object());
 }
