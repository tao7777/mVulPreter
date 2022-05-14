 GLboolean WebGLRenderingContextBase::isProgram(WebGLProgram* program) {
  if (!program || isContextLost() || !program->Validate(ContextGroup(), this))
     return 0;
 
   return ContextGL()->IsProgram(program->Object());
 }
