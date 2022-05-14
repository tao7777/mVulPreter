bool GLES2DecoderPassthroughImpl::IsEmulatedQueryTarget(GLenum target) const {
  switch (target) {
    case GL_COMMANDS_COMPLETED_CHROMIUM:
    case GL_READBACK_SHADOW_COPIES_UPDATED_CHROMIUM:
    case GL_COMMANDS_ISSUED_CHROMIUM:
     case GL_LATENCY_QUERY_CHROMIUM:
     case GL_ASYNC_PIXEL_PACK_COMPLETED_CHROMIUM:
     case GL_GET_ERROR_QUERY_CHROMIUM:
    case GL_PROGRAM_COMPLETION_QUERY_CHROMIUM:
       return true;
 
     default:
       return false;
   }
 }
