error::Error GLES2DecoderImpl::HandleDrawArrays(
    uint32 immediate_data_size, const gles2::DrawArrays& c) {
  GLenum mode = static_cast<GLenum>(c.mode);
  GLint first = static_cast<GLint>(c.first);
  GLsizei count = static_cast<GLsizei>(c.count);
  if (!validators_->draw_mode.IsValid(mode)) {
    SetGLError(GL_INVALID_ENUM, "glDrawArrays: mode GL_INVALID_ENUM");
    return error::kNoError;
  }
  if (count < 0) {
    SetGLError(GL_INVALID_VALUE, "glDrawArrays: count < 0");
    return error::kNoError;
  }
  if (!CheckFramebufferComplete("glDrawArrays")) {
    return error::kNoError;
  }
  if (first < 0) {
    SetGLError(GL_INVALID_VALUE, "glDrawArrays: first < 0");
    return error::kNoError;
  }

  if (count == 0) {
    return error::kNoError;
  }
 
   GLuint max_vertex_accessed = first + count - 1;
   if (IsDrawValid(max_vertex_accessed)) {
    bool simulated_attrib_0 = false;
    if (!SimulateAttrib0(max_vertex_accessed, &simulated_attrib_0)) {
      return error::kNoError;
    }
     bool simulated_fixed_attribs = false;
     if (SimulateFixedAttribs(max_vertex_accessed, &simulated_fixed_attribs)) {
       bool textures_set = SetBlackTextureForNonRenderableTextures();
      ApplyDirtyState();
      glDrawArrays(mode, first, count);
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
      LOG(ERROR) << "  GLES2DecoderImpl: Context lost during DrawArrays.";
      return error::kLostContext;
    }
  }
  return error::kNoError;
}
