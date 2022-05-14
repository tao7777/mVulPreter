void ContextState::RestoreTextureUnitBindings(
    GLuint unit, const ContextState* prev_state) const {
   DCHECK_LT(unit, texture_units.size());
   const TextureUnit& texture_unit = texture_units[unit];
   GLuint service_id_2d = Get2dServiceId(texture_unit);
   GLuint service_id_cube = GetCubeServiceId(texture_unit);
   GLuint service_id_oes = GetOesServiceId(texture_unit);
   GLuint service_id_arb = GetArbServiceId(texture_unit);

  bool bind_texture_2d = true;
  bool bind_texture_cube = true;
  bool bind_texture_oes =
       feature_info_->feature_flags().oes_egl_image_external ||
       feature_info_->feature_flags().nv_egl_stream_consumer_external;
   bool bind_texture_arb = feature_info_->feature_flags().arb_texture_rectangle;
 
   if (prev_state) {
     const TextureUnit& prev_unit = prev_state->texture_units[unit];
     bind_texture_2d = service_id_2d != Get2dServiceId(prev_unit);
     bind_texture_cube = service_id_cube != GetCubeServiceId(prev_unit);
     bind_texture_oes =
         bind_texture_oes && service_id_oes != GetOesServiceId(prev_unit);
    bind_texture_arb =
        bind_texture_arb && service_id_arb != GetArbServiceId(prev_unit);
   }
 
  if (!bind_texture_2d && !bind_texture_cube
      && !bind_texture_oes && !bind_texture_arb) {
     return;
   }
 
  api()->glActiveTextureFn(GL_TEXTURE0 + unit);
  if (bind_texture_2d) {
    api()->glBindTextureFn(GL_TEXTURE_2D, service_id_2d);
  }
  if (bind_texture_cube) {
    api()->glBindTextureFn(GL_TEXTURE_CUBE_MAP, service_id_cube);
  }
  if (bind_texture_oes) {
    api()->glBindTextureFn(GL_TEXTURE_EXTERNAL_OES, service_id_oes);
  }
   if (bind_texture_arb) {
     api()->glBindTextureFn(GL_TEXTURE_RECTANGLE_ARB, service_id_arb);
   }
 }
