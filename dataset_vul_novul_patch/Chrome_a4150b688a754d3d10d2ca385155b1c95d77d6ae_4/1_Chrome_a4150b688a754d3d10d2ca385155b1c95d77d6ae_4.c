error::Error GLES2DecoderPassthroughImpl::DoBeginQueryEXT(
    GLenum target,
    GLuint id,
    int32_t sync_shm_id,
    uint32_t sync_shm_offset) {
  GLuint service_id = GetQueryServiceID(id, &query_id_map_);
  QueryInfo* query_info = &query_info_map_[service_id];

  scoped_refptr<gpu::Buffer> buffer = GetSharedMemoryBuffer(sync_shm_id);
  if (!buffer)
    return error::kInvalidArguments;
  QuerySync* sync = static_cast<QuerySync*>(
      buffer->GetDataAddress(sync_shm_offset, sizeof(QuerySync)));
   if (!sync)
     return error::kOutOfBounds;
 
   if (IsEmulatedQueryTarget(target)) {
     if (active_queries_.find(target) != active_queries_.end()) {
       InsertError(GL_INVALID_OPERATION, "Query already active on target.");
      return error::kNoError;
    }

    if (id == 0) {
      InsertError(GL_INVALID_OPERATION, "Query id is 0.");
      return error::kNoError;
    }

    if (query_info->type != GL_NONE && query_info->type != target) {
      InsertError(GL_INVALID_OPERATION,
                  "Query type does not match the target.");
      return error::kNoError;
    }
  } else {
    CheckErrorCallbackState();

    api()->glBeginQueryFn(target, service_id);

    if (CheckErrorCallbackState()) {
      return error::kNoError;
    }
  }

  query_info->type = target;

  RemovePendingQuery(service_id);

  ActiveQuery query;
  query.service_id = service_id;
  query.shm = std::move(buffer);
  query.sync = sync;
  active_queries_[target] = std::move(query);

  return error::kNoError;
}
