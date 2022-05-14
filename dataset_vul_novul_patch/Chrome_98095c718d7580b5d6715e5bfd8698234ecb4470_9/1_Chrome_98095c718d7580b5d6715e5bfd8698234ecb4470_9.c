 GLboolean WebGLRenderingContextBase::isShader(WebGLShader* shader) {
  if (!shader || isContextLost())
     return 0;
 
   return ContextGL()->IsShader(shader->Object());
 }
