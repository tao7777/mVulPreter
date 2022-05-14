 GLboolean WebGLRenderingContextBase::isProgram(WebGLProgram* program) {
  if (!program || isContextLost())
     return 0;
 
   return ContextGL()->IsProgram(program->Object());
 }
