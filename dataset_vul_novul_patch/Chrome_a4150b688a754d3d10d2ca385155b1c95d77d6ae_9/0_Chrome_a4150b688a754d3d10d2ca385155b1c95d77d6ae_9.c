void WebGLRenderingContextBase::linkProgram(WebGLProgram* program) {
  if (!ValidateWebGLProgramOrShader("linkProgram", program))
    return;

  if (program->ActiveTransformFeedbackCount() > 0) {
    SynthesizeGLError(
        GL_INVALID_OPERATION, "linkProgram",
        "program being used by one or more active transform feedback objects");
     return;
   }
 
  GLuint query = 0u;
  if (ExtensionEnabled(kKHRParallelShaderCompileName)) {
    ContextGL()->GenQueriesEXT(1, &query);
    ContextGL()->BeginQueryEXT(GL_PROGRAM_COMPLETION_QUERY_CHROMIUM, query);
  }
   ContextGL()->LinkProgram(ObjectOrZero(program));
  if (ExtensionEnabled(kKHRParallelShaderCompileName)) {
    ContextGL()->EndQueryEXT(GL_PROGRAM_COMPLETION_QUERY_CHROMIUM);
    addProgramCompletionQuery(program, query);
  }

   program->IncreaseLinkCount();
 }
