error::Error GLES2DecoderImpl::HandleGetMultipleIntegervCHROMIUM(
    uint32 immediate_data_size, const gles2::GetMultipleIntegervCHROMIUM& c) {
  GLuint count = c.count;
  uint32 pnames_size;
  if (!SafeMultiplyUint32(count, sizeof(GLenum), &pnames_size)) {
    return error::kOutOfBounds;
  }
  const GLenum* pnames = GetSharedMemoryAs<const GLenum*>(
      c.pnames_shm_id, c.pnames_shm_offset, pnames_size);
  if (pnames == NULL) {
    return error::kOutOfBounds;
  }

  scoped_array<GLenum> enums(new GLenum[count]);
  memcpy(enums.get(), pnames, pnames_size);

  uint32 num_results = 0;
  for (GLuint ii = 0; ii < count; ++ii) {
    uint32 num = util_.GLGetNumValuesReturned(enums[ii]);
    if (num == 0) {
      SetGLErrorInvalidEnum("glGetMulitpleCHROMIUM", enums[ii], "pname");
      return error::kNoError;
     }
     DCHECK_LE(num, 4u);
    if (!SafeAdd(num_results, num, &num_results)) {
       return error::kOutOfBounds;
     }
   }

  uint32 result_size = 0;
  if (!SafeMultiplyUint32(num_results, sizeof(GLint), &result_size)) {
    return error::kOutOfBounds;
  }

  if (result_size != static_cast<uint32>(c.size)) {
    SetGLError(GL_INVALID_VALUE,
               "glGetMulitpleCHROMIUM", "bad size GL_INVALID_VALUE");
    return error::kNoError;
  }

  GLint* results = GetSharedMemoryAs<GLint*>(
      c.results_shm_id, c.results_shm_offset, result_size);
  if (results == NULL) {
    return error::kOutOfBounds;
  }

  for (uint32 ii = 0; ii < num_results; ++ii) {
    if (results[ii]) {
      return error::kInvalidArguments;
    }
  }

  GLint* start = results;
  for (GLuint ii = 0; ii < count; ++ii) {
    GLsizei num_written = 0;
    if (!GetHelper(enums[ii], results, &num_written)) {
      glGetIntegerv(enums[ii], results);
    }
    results += num_written;
  }

  if (static_cast<uint32>(results - start) != num_results) {
    return error::kOutOfBounds;
  }

  return error::kNoError;
}
