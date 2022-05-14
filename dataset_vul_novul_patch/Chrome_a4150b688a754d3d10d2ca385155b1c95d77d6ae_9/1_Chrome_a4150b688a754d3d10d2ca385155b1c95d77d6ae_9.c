void WebGLRenderingContextBase::linkProgram(WebGLProgram* program) {
  if (!ValidateWebGLProgramOrShader("linkProgram", program))
    return;

  if (program->ActiveTransformFeedbackCount() > 0) {
    SynthesizeGLError(
        GL_INVALID_OPERATION, "linkProgram",
        "program being used by one or more active transform feedback objects");
     return;
   }
 
   ContextGL()->LinkProgram(ObjectOrZero(program));
   program->IncreaseLinkCount();
 }
