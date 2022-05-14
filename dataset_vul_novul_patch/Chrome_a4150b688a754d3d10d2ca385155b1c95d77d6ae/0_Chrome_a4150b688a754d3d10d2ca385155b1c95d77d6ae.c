void GLES2Implementation::BeginQueryEXT(GLenum target, GLuint id) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] BeginQueryEXT("
                     << GLES2Util::GetStringQueryTarget(target) << ", " << id
                     << ")");

  switch (target) {
    case GL_COMMANDS_ISSUED_CHROMIUM:
     case GL_LATENCY_QUERY_CHROMIUM:
     case GL_ASYNC_PIXEL_PACK_COMPLETED_CHROMIUM:
     case GL_GET_ERROR_QUERY_CHROMIUM:
    case GL_PROGRAM_COMPLETION_QUERY_CHROMIUM:
       break;
     case GL_READBACK_SHADOW_COPIES_UPDATED_CHROMIUM:
     case GL_COMMANDS_COMPLETED_CHROMIUM:
      if (!capabilities_.sync_query) {
        SetGLError(GL_INVALID_OPERATION, "glBeginQueryEXT",
                   "not enabled for commands completed queries");
        return;
      }
      break;
    case GL_SAMPLES_PASSED_ARB:
      if (!capabilities_.occlusion_query) {
        SetGLError(GL_INVALID_OPERATION, "glBeginQueryEXT",
                   "not enabled for occlusion queries");
        return;
      }
      break;
    case GL_ANY_SAMPLES_PASSED:
    case GL_ANY_SAMPLES_PASSED_CONSERVATIVE:
      if (!capabilities_.occlusion_query_boolean) {
        SetGLError(GL_INVALID_OPERATION, "glBeginQueryEXT",
                   "not enabled for boolean occlusion queries");
        return;
      }
      break;
    case GL_TIME_ELAPSED_EXT:
      if (!capabilities_.timer_queries) {
        SetGLError(GL_INVALID_OPERATION, "glBeginQueryEXT",
                   "not enabled for timing queries");
        return;
      }
      break;
    case GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN:
      if (capabilities_.major_version >= 3)
        break;
      FALLTHROUGH;
    default:
      SetGLError(GL_INVALID_ENUM, "glBeginQueryEXT", "unknown query target");
      return;
  }

  if (query_tracker_->GetCurrentQuery(target)) {
    SetGLError(GL_INVALID_OPERATION, "glBeginQueryEXT",
               "query already in progress");
    return;
  }

  if (id == 0) {
    SetGLError(GL_INVALID_OPERATION, "glBeginQueryEXT", "id is 0");
    return;
  }

  if (!GetIdAllocator(IdNamespaces::kQueries)->InUse(id)) {
    SetGLError(GL_INVALID_OPERATION, "glBeginQueryEXT", "invalid id");
    return;
  }

  switch (target) {
    case GL_TIME_ELAPSED_EXT:
      if (!query_tracker_->SetDisjointSync(this)) {
        SetGLError(GL_OUT_OF_MEMORY, "glBeginQueryEXT",
                   "buffer allocation failed");
        return;
      }
      break;
    default:
      break;
  }

  if (query_tracker_->BeginQuery(id, target, this))
    CheckGLError();

  if (target == GL_READBACK_SHADOW_COPIES_UPDATED_CHROMIUM) {
    AllocateShadowCopiesForReadback();
  }
}
