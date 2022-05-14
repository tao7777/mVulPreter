 error::Error GLES2DecoderPassthroughImpl::DoLinkProgram(GLuint program) {
   TRACE_EVENT0("gpu", "GLES2DecoderPassthroughImpl::DoLinkProgram");
   SCOPED_UMA_HISTOGRAM_TIMER("GPU.PassthroughDoLinkProgramTime");
  api()->glLinkProgramFn(GetProgramServiceID(program, resources_));
 
   ExitCommandProcessingEarly();
 
   return error::kNoError;
 }
