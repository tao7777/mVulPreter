error::Error GLES2DecoderImpl::HandleBeginQueryEXT(
    uint32_t immediate_data_size,
    const volatile void* cmd_data) {
  const volatile gles2::cmds::BeginQueryEXT& c =
      *static_cast<const volatile gles2::cmds::BeginQueryEXT*>(cmd_data);
  GLenum target = static_cast<GLenum>(c.target);
  GLuint client_id = static_cast<GLuint>(c.id);
  int32_t sync_shm_id = static_cast<int32_t>(c.sync_data_shm_id);
  uint32_t sync_shm_offset = static_cast<uint32_t>(c.sync_data_shm_offset);

  switch (target) {
    case GL_COMMANDS_ISSUED_CHROMIUM:
    case GL_LATENCY_QUERY_CHROMIUM:
    case GL_ASYNC_PIXEL_PACK_COMPLETED_CHROMIUM:
    case GL_GET_ERROR_QUERY_CHROMIUM:
      break;
    case GL_READBACK_SHADOW_COPIES_UPDATED_CHROMIUM:
    case GL_COMMANDS_COMPLETED_CHROMIUM:
      if (!features().chromium_sync_query) {
        LOCAL_SET_GL_ERROR(
            GL_INVALID_OPERATION, "glBeginQueryEXT",
            "not enabled for commands completed queries");
         return error::kNoError;
       }
       break;
     case GL_SAMPLES_PASSED_ARB:
       if (!features().occlusion_query) {
         LOCAL_SET_GL_ERROR(
            GL_INVALID_OPERATION, "glBeginQueryEXT",
            "not enabled for occlusion queries");
        return error::kNoError;
      }
      break;
    case GL_ANY_SAMPLES_PASSED:
    case GL_ANY_SAMPLES_PASSED_CONSERVATIVE:
      if (!features().occlusion_query_boolean) {
        LOCAL_SET_GL_ERROR(
            GL_INVALID_OPERATION, "glBeginQueryEXT",
            "not enabled for boolean occlusion queries");
        return error::kNoError;
      }
      break;
    case GL_TIME_ELAPSED:
      if (!query_manager_->GPUTimingAvailable()) {
        LOCAL_SET_GL_ERROR(
            GL_INVALID_OPERATION, "glBeginQueryEXT",
            "not enabled for timing queries");
        return error::kNoError;
      }
      break;
    case GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN:
      if (feature_info_->IsWebGL2OrES3Context()) {
        break;
      }
      FALLTHROUGH;
    default:
      LOCAL_SET_GL_ERROR(
          GL_INVALID_ENUM, "glBeginQueryEXT",
          "unknown query target");
      return error::kNoError;
  }

  if (query_manager_->GetActiveQuery(target)) {
    LOCAL_SET_GL_ERROR(
        GL_INVALID_OPERATION, "glBeginQueryEXT", "query already in progress");
    return error::kNoError;
  }

  if (client_id == 0) {
    LOCAL_SET_GL_ERROR(GL_INVALID_OPERATION, "glBeginQueryEXT", "id is 0");
    return error::kNoError;
  }

  scoped_refptr<gpu::Buffer> buffer = GetSharedMemoryBuffer(sync_shm_id);
  if (!buffer)
    return error::kInvalidArguments;
  QuerySync* sync = static_cast<QuerySync*>(
      buffer->GetDataAddress(sync_shm_offset, sizeof(QuerySync)));
  if (!sync)
    return error::kOutOfBounds;

  QueryManager::Query* query = query_manager_->GetQuery(client_id);
  if (!query) {
    if (!query_manager_->IsValidQuery(client_id)) {
      LOCAL_SET_GL_ERROR(GL_INVALID_OPERATION,
                         "glBeginQueryEXT",
                         "id not made by glGenQueriesEXT");
      return error::kNoError;
    }

    query =
        query_manager_->CreateQuery(target, client_id, std::move(buffer), sync);
  } else {
    if (query->target() != target) {
      LOCAL_SET_GL_ERROR(GL_INVALID_OPERATION, "glBeginQueryEXT",
                         "target does not match");
      return error::kNoError;
    } else if (query->sync() != sync) {
      DLOG(ERROR) << "Shared memory used by query not the same as before";
      return error::kInvalidArguments;
    }
  }

  query_manager_->BeginQuery(query);
  return error::kNoError;
}
