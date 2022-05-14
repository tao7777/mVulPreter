bool GLES2DecoderImpl::SimulateFixedAttribs(
    GLuint max_vertex_accessed, bool* simulated) {
  DCHECK(simulated);
  *simulated = false;
  if (gfx::GetGLImplementation() == gfx::kGLImplementationEGLGLES2)
    return true;

  if (!vertex_attrib_manager_.HaveFixedAttribs()) {
    return true;
  }

 
  GLuint num_vertices = max_vertex_accessed + 1;
  if (num_vertices == 0) {
    SetGLError(GL_OUT_OF_MEMORY, "glDrawXXX: Simulating attrib 0");
    return false;
  }

  GLuint elements_needed = 0;
   const VertexAttribManager::VertexAttribInfoList& infos =
       vertex_attrib_manager_.GetEnabledVertexAttribInfos();
   for (VertexAttribManager::VertexAttribInfoList::const_iterator it =
       infos.begin(); it != infos.end(); ++it) {
    const VertexAttribManager::VertexAttribInfo* info = *it;
    const ProgramManager::ProgramInfo::VertexAttribInfo* attrib_info =
        current_program_->GetAttribInfoByLocation(info->index());
     if (attrib_info &&
         info->CanAccess(max_vertex_accessed) &&
         info->type() == GL_FIXED) {
      GLuint elements_used = 0;
      if (!SafeMultiply(num_vertices,
                        static_cast<GLuint>(info->size()), &elements_used) ||
           !SafeAdd(elements_needed, elements_used, &elements_needed)) {
         SetGLError(GL_OUT_OF_MEMORY, "glDrawXXX: simulating GL_FIXED attribs");
         return false;
       }
     }
   }
 
  const GLuint kSizeOfFloat = sizeof(float);  // NOLINT
  GLuint size_needed = 0;
  if (!SafeMultiply(elements_needed, kSizeOfFloat, &size_needed) ||
      size_needed > 0x7FFFFFFFU) {
     SetGLError(GL_OUT_OF_MEMORY, "glDrawXXX: simulating GL_FIXED attribs");
     return false;
   }
 
  CopyRealGLErrorsToWrapper();
 
   glBindBuffer(GL_ARRAY_BUFFER, fixed_attrib_buffer_id_);
  if (static_cast<GLsizei>(size_needed) > fixed_attrib_buffer_size_) {
     glBufferData(GL_ARRAY_BUFFER, size_needed, NULL, GL_DYNAMIC_DRAW);
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
      SetGLError(GL_OUT_OF_MEMORY, "glDrawXXX: simulating GL_FIXED attribs");
      return false;
    }
   }
 
  GLintptr offset = 0;
  for (VertexAttribManager::VertexAttribInfoList::const_iterator it =
           infos.begin(); it != infos.end(); ++it) {
    const VertexAttribManager::VertexAttribInfo* info = *it;
    const ProgramManager::ProgramInfo::VertexAttribInfo* attrib_info =
        current_program_->GetAttribInfoByLocation(info->index());
    if (attrib_info &&
        info->CanAccess(max_vertex_accessed) &&
        info->type() == GL_FIXED) {
      int num_elements = info->size() * kSizeOfFloat;
      int size = num_elements * num_vertices;
      scoped_array<float> data(new float[size]);
      const int32* src = reinterpret_cast<const int32 *>(
          info->buffer()->GetRange(info->offset(), size));
      const int32* end = src + num_elements;
      float* dst = data.get();
      while (src != end) {
        *dst++ = static_cast<float>(*src++) / 65536.0f;
      }
      glBufferSubData(GL_ARRAY_BUFFER, offset, size, data.get());
      glVertexAttribPointer(
          info->index(), info->size(), GL_FLOAT, false, 0,
          reinterpret_cast<GLvoid*>(offset));
      offset += size;
    }
  }
  *simulated = true;
  return true;
}
