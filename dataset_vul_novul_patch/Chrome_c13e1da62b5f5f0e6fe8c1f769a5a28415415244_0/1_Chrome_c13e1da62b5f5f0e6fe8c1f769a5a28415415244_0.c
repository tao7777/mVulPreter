error::Error GLES2DecoderImpl::HandleDrawElements(
    uint32 immediate_data_size, const gles2::DrawElements& c) {
  if (!bound_element_array_buffer_ ||
      bound_element_array_buffer_->IsDeleted()) {
    SetGLError(GL_INVALID_OPERATION,
               "glDrawElements: No element array buffer bound");
    return error::kNoError;
  }

  GLenum mode = c.mode;
  GLsizei count = c.count;
  GLenum type = c.type;
  int32 offset = c.index_offset;
  if (count < 0) {
    SetGLError(GL_INVALID_VALUE, "glDrawElements: count < 0");
    return error::kNoError;
  }
  if (offset < 0) {
    SetGLError(GL_INVALID_VALUE, "glDrawElements: offset < 0");
    return error::kNoError;
  }
  if (!validators_->draw_mode.IsValid(mode)) {
    SetGLError(GL_INVALID_ENUM, "glDrawElements: mode GL_INVALID_ENUM");
    return error::kNoError;
  }
  if (!validators_->index_type.IsValid(type)) {
    SetGLError(GL_INVALID_ENUM, "glDrawElements: type GL_INVALID_ENUM");
    return error::kNoError;
  }

  if (!CheckFramebufferComplete("glDrawElements")) {
    return error::kNoError;
  }

  if (count == 0) {
    return error::kNoError;
  }

  GLuint max_vertex_accessed;
  if (!bound_element_array_buffer_->GetMaxValueForRange(
      offset, count, type, &max_vertex_accessed)) {
    SetGLError(GL_INVALID_OPERATION,
               "glDrawElements: range out of bounds for buffer");
    return error::kNoError;
   }
 
   if (IsDrawValid(max_vertex_accessed)) {
    bool simulated_attrib_0 = SimulateAttrib0(max_vertex_accessed);
     bool simulated_fixed_attribs = false;
     if (SimulateFixedAttribs(max_vertex_accessed, &simulated_fixed_attribs)) {
       bool textures_set = SetBlackTextureForNonRenderableTextures();
      ApplyDirtyState();
      const GLvoid* indices = reinterpret_cast<const GLvoid*>(offset);
      glDrawElements(mode, count, type, indices);
      if (textures_set) {
        RestoreStateForNonRenderableTextures();
      }
      if (simulated_fixed_attribs) {
        RestoreStateForSimulatedFixedAttribs();
      }
    }
    if (simulated_attrib_0) {
      RestoreStateForSimulatedAttrib0();
    }
    if (WasContextLost()) {
      LOG(ERROR) << "  GLES2DecoderImpl: Context lost during DrawElements.";
      return error::kLostContext;
    }
  }
  return error::kNoError;
}
