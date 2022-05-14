 void WebGL2RenderingContextBase::bindSampler(GLuint unit,
                                              WebGLSampler* sampler) {
   bool deleted;
   if (!CheckObjectToBeBound("bindSampler", sampler, deleted))
     return;
  if (deleted) {
    SynthesizeGLError(GL_INVALID_OPERATION, "bindSampler",
                      "attempted to bind a deleted sampler");
    return;
  }

  if (unit >= sampler_units_.size()) {
    SynthesizeGLError(GL_INVALID_VALUE, "bindSampler",
                      "texture unit out of range");
    return;
  }

  sampler_units_[unit] = sampler;

  ContextGL()->BindSampler(unit, ObjectOrZero(sampler));
}
