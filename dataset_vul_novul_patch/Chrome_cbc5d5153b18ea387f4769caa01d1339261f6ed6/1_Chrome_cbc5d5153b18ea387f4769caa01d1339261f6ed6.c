void FeatureInfo::EnableOESTextureHalfFloatLinear() {
  if (!oes_texture_half_float_linear_available_)
     return;
   AddExtensionString("GL_OES_texture_half_float_linear");
   feature_flags_.enable_texture_half_float_linear = true;
   feature_flags_.gpu_memory_buffer_formats.Add(gfx::BufferFormat::RGBA_F16);
 }
