ScriptValue WebGLRenderingContextBase::getProgramParameter(
    ScriptState* script_state,
    WebGLProgram* program,
    GLenum pname) {
  if (!ValidateWebGLProgramOrShader("getProgramParamter", program)) {
    return ScriptValue::CreateNull(script_state);
  }

  GLint value = 0;
  switch (pname) {
    case GL_DELETE_STATUS:
      return WebGLAny(script_state, program->MarkedForDeletion());
    case GL_VALIDATE_STATUS:
      ContextGL()->GetProgramiv(ObjectOrZero(program), pname, &value);
      return WebGLAny(script_state, static_cast<bool>(value));
    case GL_LINK_STATUS:
      return WebGLAny(script_state, program->LinkStatus(this));
    case GL_COMPLETION_STATUS_KHR:
      if (!ExtensionEnabled(kKHRParallelShaderCompileName)) {
        SynthesizeGLError(GL_INVALID_ENUM, "getProgramParameter",
                           "invalid parameter name");
         return ScriptValue::CreateNull(script_state);
       }
      bool completed;
      if (checkProgramCompletionQueryAvailable(program, &completed)) {
        return WebGLAny(script_state, completed);
      }
       return WebGLAny(script_state, program->CompletionStatus(this));
     case GL_ACTIVE_UNIFORM_BLOCKS:
     case GL_TRANSFORM_FEEDBACK_VARYINGS:
      if (!IsWebGL2OrHigher()) {
        SynthesizeGLError(GL_INVALID_ENUM, "getProgramParameter",
                          "invalid parameter name");
        return ScriptValue::CreateNull(script_state);
      }
      FALLTHROUGH;
    case GL_ATTACHED_SHADERS:
    case GL_ACTIVE_ATTRIBUTES:
    case GL_ACTIVE_UNIFORMS:
      ContextGL()->GetProgramiv(ObjectOrZero(program), pname, &value);
      return WebGLAny(script_state, value);
    case GL_TRANSFORM_FEEDBACK_BUFFER_MODE:
      if (!IsWebGL2OrHigher()) {
        SynthesizeGLError(GL_INVALID_ENUM, "getProgramParameter",
                          "invalid parameter name");
        return ScriptValue::CreateNull(script_state);
      }
      ContextGL()->GetProgramiv(ObjectOrZero(program), pname, &value);
      return WebGLAny(script_state, static_cast<unsigned>(value));
    case GL_ACTIVE_ATOMIC_COUNTER_BUFFERS:
      if (context_type_ == Platform::kWebGL2ComputeContextType) {
        ContextGL()->GetProgramiv(ObjectOrZero(program), pname, &value);
        return WebGLAny(script_state, static_cast<unsigned>(value));
      }
      FALLTHROUGH;
    default:
      SynthesizeGLError(GL_INVALID_ENUM, "getProgramParameter",
                        "invalid parameter name");
      return ScriptValue::CreateNull(script_state);
  }
}
