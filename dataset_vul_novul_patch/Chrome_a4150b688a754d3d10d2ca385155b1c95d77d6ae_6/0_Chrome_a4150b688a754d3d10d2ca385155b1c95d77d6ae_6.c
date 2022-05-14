 error::Error GLES2DecoderPassthroughImpl::DoLinkProgram(GLuint program) {
   TRACE_EVENT0("gpu", "GLES2DecoderPassthroughImpl::DoLinkProgram");
   SCOPED_UMA_HISTOGRAM_TIMER("GPU.PassthroughDoLinkProgramTime");
  GLuint program_service_id = GetProgramServiceID(program, resources_);
  api()->glLinkProgramFn(program_service_id);
 
   ExitCommandProcessingEarly();
 
  linking_program_service_id_ = program_service_id;

   return error::kNoError;
 }
