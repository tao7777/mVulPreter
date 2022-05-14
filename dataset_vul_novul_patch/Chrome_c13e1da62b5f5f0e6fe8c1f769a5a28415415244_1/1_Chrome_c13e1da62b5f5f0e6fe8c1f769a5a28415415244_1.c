bool GLES2DecoderImpl::SimulateAttrib0(GLuint max_vertex_accessed) {
   if (gfx::GetGLImplementation() == gfx::kGLImplementationEGLGLES2)
    return false;
 
   const VertexAttribManager::VertexAttribInfo* info =
       vertex_attrib_manager_.GetVertexAttribInfo(0);
   bool attrib_0_used = current_program_->GetAttribInfoByLocation(0) != NULL;
   if (info->enabled() && attrib_0_used) {
    return false;
   }
 
   typedef VertexAttribManager::VertexAttribInfo::Vec4 Vec4;
 
   glBindBuffer(GL_ARRAY_BUFFER, attrib_0_buffer_id_);
 
  GLsizei num_vertices = max_vertex_accessed + 1;
  GLsizei size_needed = num_vertices * sizeof(Vec4);  // NOLINT
  if (size_needed > attrib_0_size_) {
     glBufferData(GL_ARRAY_BUFFER, size_needed, NULL, GL_DYNAMIC_DRAW);
     attrib_0_buffer_matches_value_ = false;
   }
   if (attrib_0_used &&
      (!attrib_0_buffer_matches_value_ ||
       (info->value().v[0] != attrib_0_value_.v[0] ||
        info->value().v[1] != attrib_0_value_.v[1] ||
        info->value().v[2] != attrib_0_value_.v[2] ||
        info->value().v[3] != attrib_0_value_.v[3]))) {
    std::vector<Vec4> temp(num_vertices, info->value());
    glBufferSubData(GL_ARRAY_BUFFER, 0, size_needed, &temp[0].v[0]);
    attrib_0_buffer_matches_value_ = true;
    attrib_0_value_ = info->value();
    attrib_0_size_ = size_needed;
  }
 
   glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
 
   return true;
 }
