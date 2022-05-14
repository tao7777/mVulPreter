void FeatureInfo::InitializeFeatures() {
  std::string extensions_string(gl::GetGLExtensionsFromCurrentContext());
  gfx::ExtensionSet extensions(gfx::MakeExtensionSet(extensions_string));

  const char* version_str =
      reinterpret_cast<const char*>(glGetString(GL_VERSION));
  const char* renderer_str =
      reinterpret_cast<const char*>(glGetString(GL_RENDERER));

  gl_version_info_.reset(
      new gl::GLVersionInfo(version_str, renderer_str, extensions));

  bool enable_es3 = IsWebGL2OrES3OrHigherContext();

  bool has_pixel_buffers =
      gl_version_info_->is_es3 || gl_version_info_->is_desktop_core_profile ||
      gfx::HasExtension(extensions, "GL_ARB_pixel_buffer_object") ||
      gfx::HasExtension(extensions, "GL_NV_pixel_buffer_object");

  ScopedPixelUnpackBufferOverride scoped_pbo_override(has_pixel_buffers, 0);

  AddExtensionString("GL_ANGLE_translated_shader_source");
  AddExtensionString("GL_CHROMIUM_async_pixel_transfers");
  AddExtensionString("GL_CHROMIUM_bind_uniform_location");
  AddExtensionString("GL_CHROMIUM_color_space_metadata");
  AddExtensionString("GL_CHROMIUM_command_buffer_query");
  AddExtensionString("GL_CHROMIUM_command_buffer_latency_query");
  AddExtensionString("GL_CHROMIUM_copy_texture");
  AddExtensionString("GL_CHROMIUM_deschedule");
  AddExtensionString("GL_CHROMIUM_get_error_query");
  AddExtensionString("GL_CHROMIUM_lose_context");
  AddExtensionString("GL_CHROMIUM_pixel_transfer_buffer_object");
  AddExtensionString("GL_CHROMIUM_rate_limit_offscreen_context");
  AddExtensionString("GL_CHROMIUM_resize");
  AddExtensionString("GL_CHROMIUM_resource_safe");
  AddExtensionString("GL_CHROMIUM_strict_attribs");
  AddExtensionString("GL_CHROMIUM_texture_mailbox");
  AddExtensionString("GL_CHROMIUM_trace_marker");
  AddExtensionString("GL_EXT_debug_marker");

  AddExtensionString("GL_EXT_unpack_subimage");

  AddExtensionString("GL_OES_vertex_array_object");

  if (gfx::HasExtension(extensions, "GL_ANGLE_translated_shader_source")) {
    feature_flags_.angle_translated_shader_source = true;
  }

  bool enable_dxt1 = false;
  bool enable_dxt3 = false;
  bool enable_dxt5 = false;
  bool have_s3tc =
      gfx::HasExtension(extensions, "GL_EXT_texture_compression_s3tc");
  bool have_dxt3 =
      have_s3tc ||
      gfx::HasExtension(extensions, "GL_ANGLE_texture_compression_dxt3");
  bool have_dxt5 =
      have_s3tc ||
      gfx::HasExtension(extensions, "GL_ANGLE_texture_compression_dxt5");

  if (gfx::HasExtension(extensions, "GL_EXT_texture_compression_dxt1") ||
      gfx::HasExtension(extensions, "GL_ANGLE_texture_compression_dxt1") ||
      have_s3tc) {
    enable_dxt1 = true;
  }
  if (have_dxt3) {
    enable_dxt3 = true;
  }
  if (have_dxt5) {
    enable_dxt5 = true;
  }

  if (enable_dxt1) {
    feature_flags_.ext_texture_format_dxt1 = true;

    AddExtensionString("GL_ANGLE_texture_compression_dxt1");
    validators_.compressed_texture_format.AddValue(
        GL_COMPRESSED_RGB_S3TC_DXT1_EXT);
    validators_.compressed_texture_format.AddValue(
        GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);

    validators_.texture_internal_format_storage.AddValue(
        GL_COMPRESSED_RGB_S3TC_DXT1_EXT);
    validators_.texture_internal_format_storage.AddValue(
        GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);
  }

  if (enable_dxt3) {
    AddExtensionString("GL_ANGLE_texture_compression_dxt3");
    validators_.compressed_texture_format.AddValue(
        GL_COMPRESSED_RGBA_S3TC_DXT3_EXT);
    validators_.texture_internal_format_storage.AddValue(
        GL_COMPRESSED_RGBA_S3TC_DXT3_EXT);
  }

  if (enable_dxt5) {
    feature_flags_.ext_texture_format_dxt5 = true;

    AddExtensionString("GL_ANGLE_texture_compression_dxt5");
    validators_.compressed_texture_format.AddValue(
        GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);
    validators_.texture_internal_format_storage.AddValue(
        GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);
  }

  bool have_astc =
      gfx::HasExtension(extensions, "GL_KHR_texture_compression_astc_ldr");
  if (have_astc) {
    feature_flags_.ext_texture_format_astc = true;
    AddExtensionString("GL_KHR_texture_compression_astc_ldr");

    GLint astc_format_it = GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
    GLint astc_format_max = GL_COMPRESSED_RGBA_ASTC_12x12_KHR;
    for (; astc_format_it <= astc_format_max; astc_format_it++) {
      validators_.compressed_texture_format.AddValue(astc_format_it);
      validators_.texture_internal_format_storage.AddValue(astc_format_it);
    }

    astc_format_it = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;
    astc_format_max = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR;
    for (; astc_format_it <= astc_format_max; astc_format_it++) {
      validators_.compressed_texture_format.AddValue(astc_format_it);
      validators_.texture_internal_format_storage.AddValue(astc_format_it);
    }
  }

  bool have_atc =
      gfx::HasExtension(extensions, "GL_AMD_compressed_ATC_texture") ||
      gfx::HasExtension(extensions, "GL_ATI_texture_compression_atitc");
  if (have_atc) {
    feature_flags_.ext_texture_format_atc = true;

    AddExtensionString("GL_AMD_compressed_ATC_texture");
    validators_.compressed_texture_format.AddValue(GL_ATC_RGB_AMD);
    validators_.compressed_texture_format.AddValue(
        GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD);

    validators_.texture_internal_format_storage.AddValue(GL_ATC_RGB_AMD);
    validators_.texture_internal_format_storage.AddValue(
        GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD);
  }

  if (gfx::HasExtension(extensions, "GL_EXT_texture_filter_anisotropic")) {
    AddExtensionString("GL_EXT_texture_filter_anisotropic");
    validators_.texture_parameter.AddValue(GL_TEXTURE_MAX_ANISOTROPY_EXT);
    validators_.g_l_state.AddValue(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT);
  }

  bool enable_depth_texture = false;
  GLenum depth_texture_format = GL_NONE;
  if (!workarounds_.disable_depth_texture &&
      (gfx::HasExtension(extensions, "GL_ARB_depth_texture") ||
       gfx::HasExtension(extensions, "GL_OES_depth_texture") ||
       gfx::HasExtension(extensions, "GL_ANGLE_depth_texture") ||
       gl_version_info_->is_desktop_core_profile)) {
    enable_depth_texture = true;
    depth_texture_format = GL_DEPTH_COMPONENT;
    feature_flags_.angle_depth_texture =
        gfx::HasExtension(extensions, "GL_ANGLE_depth_texture");
  }

  if (enable_depth_texture) {
    AddExtensionString("GL_CHROMIUM_depth_texture");
    AddExtensionString("GL_GOOGLE_depth_texture");
    validators_.texture_internal_format.AddValue(GL_DEPTH_COMPONENT);
    validators_.texture_format.AddValue(GL_DEPTH_COMPONENT);
    validators_.pixel_type.AddValue(GL_UNSIGNED_SHORT);
    validators_.pixel_type.AddValue(GL_UNSIGNED_INT);
    validators_.texture_depth_renderable_internal_format.AddValue(
        GL_DEPTH_COMPONENT);
  }

  GLenum depth_stencil_texture_format = GL_NONE;
  if (gfx::HasExtension(extensions, "GL_EXT_packed_depth_stencil") ||
      gfx::HasExtension(extensions, "GL_OES_packed_depth_stencil") ||
      gl_version_info_->is_es3 || gl_version_info_->is_desktop_core_profile) {
    AddExtensionString("GL_OES_packed_depth_stencil");
    feature_flags_.packed_depth24_stencil8 = true;
    if (enable_depth_texture) {
      if (gl_version_info_->is_es3) {
        depth_stencil_texture_format = GL_DEPTH24_STENCIL8;
      } else {
        depth_stencil_texture_format = GL_DEPTH_STENCIL;
      }
      validators_.texture_internal_format.AddValue(GL_DEPTH_STENCIL);
      validators_.texture_format.AddValue(GL_DEPTH_STENCIL);
      validators_.pixel_type.AddValue(GL_UNSIGNED_INT_24_8);
      validators_.texture_depth_renderable_internal_format.AddValue(
          GL_DEPTH_STENCIL);
      validators_.texture_stencil_renderable_internal_format.AddValue(
          GL_DEPTH_STENCIL);
    }
    validators_.render_buffer_format.AddValue(GL_DEPTH24_STENCIL8);
    if (context_type_ == CONTEXT_TYPE_WEBGL1) {
      validators_.attachment.AddValue(GL_DEPTH_STENCIL_ATTACHMENT);
      validators_.attachment_query.AddValue(GL_DEPTH_STENCIL_ATTACHMENT);
    }
  }

  if (gl_version_info_->is_es3 || gl_version_info_->is_desktop_core_profile ||
      gfx::HasExtension(extensions, "GL_OES_vertex_array_object") ||
      gfx::HasExtension(extensions, "GL_ARB_vertex_array_object") ||
      gfx::HasExtension(extensions, "GL_APPLE_vertex_array_object")) {
    feature_flags_.native_vertex_array_object = true;
  }

  if (workarounds_.use_client_side_arrays_for_stream_buffers) {
    feature_flags_.native_vertex_array_object = false;
  }

  if (gl_version_info_->is_es3 ||
      gfx::HasExtension(extensions, "GL_OES_element_index_uint") ||
      gl::HasDesktopGLFeatures()) {
    AddExtensionString("GL_OES_element_index_uint");
    validators_.index_type.AddValue(GL_UNSIGNED_INT);
  }

  bool has_srgb_framebuffer_support = false;
  if (gl_version_info_->IsAtLeastGL(3, 2) ||
      (gl_version_info_->IsAtLeastGL(2, 0) &&
       (gfx::HasExtension(extensions, "GL_EXT_framebuffer_sRGB") ||
        gfx::HasExtension(extensions, "GL_ARB_framebuffer_sRGB")))) {
    feature_flags_.desktop_srgb_support = true;
    has_srgb_framebuffer_support = true;
  }
  if ((((gl_version_info_->is_es3 ||
         gfx::HasExtension(extensions, "GL_OES_rgb8_rgba8")) &&
        gfx::HasExtension(extensions, "GL_EXT_sRGB")) ||
       feature_flags_.desktop_srgb_support) &&
      IsWebGL1OrES2Context()) {
    feature_flags_.ext_srgb = true;
    AddExtensionString("GL_EXT_sRGB");
    validators_.texture_internal_format.AddValue(GL_SRGB_EXT);
    validators_.texture_internal_format.AddValue(GL_SRGB_ALPHA_EXT);
    validators_.texture_format.AddValue(GL_SRGB_EXT);
    validators_.texture_format.AddValue(GL_SRGB_ALPHA_EXT);
    validators_.render_buffer_format.AddValue(GL_SRGB8_ALPHA8_EXT);
    validators_.framebuffer_attachment_parameter.AddValue(
        GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING_EXT);
    validators_.texture_unsized_internal_format.AddValue(GL_SRGB_EXT);
    validators_.texture_unsized_internal_format.AddValue(GL_SRGB_ALPHA_EXT);
    has_srgb_framebuffer_support = true;
  }
  if (gl_version_info_->is_es3)
    has_srgb_framebuffer_support = true;

  if (has_srgb_framebuffer_support && !IsWebGLContext()) {
    if (feature_flags_.desktop_srgb_support ||
        gfx::HasExtension(extensions, "GL_EXT_sRGB_write_control")) {
      feature_flags_.ext_srgb_write_control = true;
      AddExtensionString("GL_EXT_sRGB_write_control");
      validators_.capability.AddValue(GL_FRAMEBUFFER_SRGB_EXT);
    }
  }

  if (gfx::HasExtension(extensions, "GL_EXT_texture_sRGB_decode") &&
      !IsWebGLContext()) {
    AddExtensionString("GL_EXT_texture_sRGB_decode");
    validators_.texture_parameter.AddValue(GL_TEXTURE_SRGB_DECODE_EXT);
  }

  bool have_s3tc_srgb = false;
  if (gl_version_info_->is_es) {
    have_s3tc_srgb =
        gfx::HasExtension(extensions, "GL_NV_sRGB_formats") ||
        gfx::HasExtension(extensions, "GL_EXT_texture_compression_s3tc_srgb");
  } else {
    if (gfx::HasExtension(extensions, "GL_EXT_texture_sRGB") ||
        gl_version_info_->IsAtLeastGL(4, 1)) {
      have_s3tc_srgb =
          gfx::HasExtension(extensions, "GL_EXT_texture_compression_s3tc");
    }
  }

  if (have_s3tc_srgb) {
    AddExtensionString("GL_EXT_texture_compression_s3tc_srgb");

    validators_.compressed_texture_format.AddValue(
        GL_COMPRESSED_SRGB_S3TC_DXT1_EXT);
    validators_.compressed_texture_format.AddValue(
        GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT);
    validators_.compressed_texture_format.AddValue(
        GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT);
    validators_.compressed_texture_format.AddValue(
        GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT);

    validators_.texture_internal_format_storage.AddValue(
        GL_COMPRESSED_SRGB_S3TC_DXT1_EXT);
    validators_.texture_internal_format_storage.AddValue(
        GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT);
    validators_.texture_internal_format_storage.AddValue(
        GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT);
    validators_.texture_internal_format_storage.AddValue(
        GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT);
  }

  bool has_apple_bgra =
      gfx::HasExtension(extensions, "GL_APPLE_texture_format_BGRA8888");
  bool has_ext_bgra =
      gfx::HasExtension(extensions, "GL_EXT_texture_format_BGRA8888");
  bool enable_texture_format_bgra8888 =
      has_ext_bgra || has_apple_bgra || !gl_version_info_->is_es;

  bool has_ext_texture_storage =
      gfx::HasExtension(extensions, "GL_EXT_texture_storage");
  bool has_arb_texture_storage =
      gfx::HasExtension(extensions, "GL_ARB_texture_storage");
  bool has_texture_storage =
      !workarounds_.disable_texture_storage &&
      (has_ext_texture_storage || has_arb_texture_storage ||
       gl_version_info_->is_es3 || gl_version_info_->IsAtLeastGL(4, 2));

  bool enable_texture_storage = has_texture_storage;

  bool texture_storage_incompatible_with_bgra =
      gl_version_info_->is_es3 && !has_ext_texture_storage && !has_apple_bgra;
  if (texture_storage_incompatible_with_bgra &&
      enable_texture_format_bgra8888 && enable_texture_storage) {
    switch (context_type_) {
      case CONTEXT_TYPE_OPENGLES2:
        enable_texture_storage = false;
        break;
      case CONTEXT_TYPE_OPENGLES3:
        enable_texture_format_bgra8888 = false;
        break;
      case CONTEXT_TYPE_WEBGL1:
      case CONTEXT_TYPE_WEBGL2:
      case CONTEXT_TYPE_WEBGL2_COMPUTE:
      case CONTEXT_TYPE_WEBGPU:
        break;
    }
  }

  if (enable_texture_storage) {
    feature_flags_.ext_texture_storage = true;
    AddExtensionString("GL_EXT_texture_storage");
    validators_.texture_parameter.AddValue(GL_TEXTURE_IMMUTABLE_FORMAT_EXT);
  }

  if (enable_texture_format_bgra8888) {
    feature_flags_.ext_texture_format_bgra8888 = true;
    AddExtensionString("GL_EXT_texture_format_BGRA8888");
    validators_.texture_internal_format.AddValue(GL_BGRA_EXT);
    validators_.texture_format.AddValue(GL_BGRA_EXT);
    validators_.texture_unsized_internal_format.AddValue(GL_BGRA_EXT);
    validators_.texture_internal_format_storage.AddValue(GL_BGRA8_EXT);
    validators_.texture_sized_color_renderable_internal_format.AddValue(
        GL_BGRA8_EXT);
    validators_.texture_sized_texture_filterable_internal_format.AddValue(
        GL_BGRA8_EXT);
    feature_flags_.gpu_memory_buffer_formats.Add(gfx::BufferFormat::BGRA_8888);
    feature_flags_.gpu_memory_buffer_formats.Add(gfx::BufferFormat::BGRX_8888);
  }

  bool enable_render_buffer_bgra =
      gl_version_info_->is_angle || !gl_version_info_->is_es;

  if (enable_render_buffer_bgra) {
    feature_flags_.ext_render_buffer_format_bgra8888 = true;
    AddExtensionString("GL_CHROMIUM_renderbuffer_format_BGRA8888");
    validators_.render_buffer_format.AddValue(GL_BGRA8_EXT);
  }

  bool enable_read_format_bgra =
      gfx::HasExtension(extensions, "GL_EXT_read_format_bgra") ||
      !gl_version_info_->is_es;

  if (enable_read_format_bgra) {
    feature_flags_.ext_read_format_bgra = true;
    AddExtensionString("GL_EXT_read_format_bgra");
    validators_.read_pixel_format.AddValue(GL_BGRA_EXT);
  }

  feature_flags_.arb_es3_compatibility =
      gfx::HasExtension(extensions, "GL_ARB_ES3_compatibility") &&
      !gl_version_info_->is_es;

  feature_flags_.ext_disjoint_timer_query =
      gfx::HasExtension(extensions, "GL_EXT_disjoint_timer_query");
  if (feature_flags_.ext_disjoint_timer_query ||
      gfx::HasExtension(extensions, "GL_ARB_timer_query") ||
      gfx::HasExtension(extensions, "GL_EXT_timer_query")) {
    AddExtensionString("GL_EXT_disjoint_timer_query");
  }

  if (gfx::HasExtension(extensions, "GL_OES_rgb8_rgba8") ||
      gl::HasDesktopGLFeatures()) {
    AddExtensionString("GL_OES_rgb8_rgba8");
    validators_.render_buffer_format.AddValue(GL_RGB8_OES);
    validators_.render_buffer_format.AddValue(GL_RGBA8_OES);
  }

  if (!disallowed_features_.npot_support &&
      (gl_version_info_->is_es3 || gl_version_info_->is_desktop_core_profile ||
       gfx::HasExtension(extensions, "GL_ARB_texture_non_power_of_two") ||
       gfx::HasExtension(extensions, "GL_OES_texture_npot"))) {
    AddExtensionString("GL_OES_texture_npot");
    feature_flags_.npot_ok = true;
  }

  InitializeFloatAndHalfFloatFeatures(extensions);

  if (!workarounds_.disable_chromium_framebuffer_multisample) {
    bool ext_has_multisample =
        gfx::HasExtension(extensions, "GL_ARB_framebuffer_object") ||
        (gfx::HasExtension(extensions, "GL_EXT_framebuffer_multisample") &&
         gfx::HasExtension(extensions, "GL_EXT_framebuffer_blit")) ||
        gl_version_info_->is_es3 || gl_version_info_->is_desktop_core_profile;
    if (gl_version_info_->is_angle || gl_version_info_->is_swiftshader) {
      ext_has_multisample |=
          gfx::HasExtension(extensions, "GL_ANGLE_framebuffer_multisample");
    }
    if (ext_has_multisample) {
      feature_flags_.chromium_framebuffer_multisample = true;
      validators_.framebuffer_target.AddValue(GL_READ_FRAMEBUFFER_EXT);
      validators_.framebuffer_target.AddValue(GL_DRAW_FRAMEBUFFER_EXT);
      validators_.g_l_state.AddValue(GL_READ_FRAMEBUFFER_BINDING_EXT);
      validators_.g_l_state.AddValue(GL_MAX_SAMPLES_EXT);
      validators_.render_buffer_parameter.AddValue(GL_RENDERBUFFER_SAMPLES_EXT);
      AddExtensionString("GL_CHROMIUM_framebuffer_multisample");
    }
  }

  if (gfx::HasExtension(extensions, "GL_EXT_multisampled_render_to_texture")) {
    feature_flags_.multisampled_render_to_texture = true;
  } else if (gfx::HasExtension(extensions,
                               "GL_IMG_multisampled_render_to_texture")) {
    feature_flags_.multisampled_render_to_texture = true;
    feature_flags_.use_img_for_multisampled_render_to_texture = true;
  }
  if (feature_flags_.multisampled_render_to_texture) {
    validators_.render_buffer_parameter.AddValue(GL_RENDERBUFFER_SAMPLES_EXT);
    validators_.g_l_state.AddValue(GL_MAX_SAMPLES_EXT);
    validators_.framebuffer_attachment_parameter.AddValue(
        GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_SAMPLES_EXT);
    AddExtensionString("GL_EXT_multisampled_render_to_texture");
  }

  if (!gl_version_info_->is_es ||
      gfx::HasExtension(extensions, "GL_EXT_multisample_compatibility")) {
    AddExtensionString("GL_EXT_multisample_compatibility");
    feature_flags_.ext_multisample_compatibility = true;
    validators_.capability.AddValue(GL_MULTISAMPLE_EXT);
    validators_.capability.AddValue(GL_SAMPLE_ALPHA_TO_ONE_EXT);
  }

  if (gfx::HasExtension(extensions, "GL_INTEL_framebuffer_CMAA")) {
    feature_flags_.chromium_screen_space_antialiasing = true;
    AddExtensionString("GL_CHROMIUM_screen_space_antialiasing");
  } else if (gl_version_info_->IsAtLeastGLES(3, 1) ||
             (gl_version_info_->IsAtLeastGL(3, 0) &&
              gfx::HasExtension(extensions,
                                "GL_ARB_shading_language_420pack") &&
              gfx::HasExtension(extensions, "GL_ARB_texture_storage") &&
              gfx::HasExtension(extensions, "GL_ARB_texture_gather") &&
              gfx::HasExtension(extensions,
                                "GL_ARB_explicit_uniform_location") &&
              gfx::HasExtension(extensions,
                                "GL_ARB_explicit_attrib_location") &&
              gfx::HasExtension(extensions,
                                "GL_ARB_shader_image_load_store"))) {
    feature_flags_.chromium_screen_space_antialiasing = true;
    feature_flags_.use_chromium_screen_space_antialiasing_via_shaders = true;
    AddExtensionString("GL_CHROMIUM_screen_space_antialiasing");
  }

  if (gfx::HasExtension(extensions, "GL_OES_depth24") ||
      gl::HasDesktopGLFeatures() || gl_version_info_->is_es3) {
    AddExtensionString("GL_OES_depth24");
    feature_flags_.oes_depth24 = true;
    validators_.render_buffer_format.AddValue(GL_DEPTH_COMPONENT24);
  }

  if (gl_version_info_->is_es3 ||
      gfx::HasExtension(extensions, "GL_OES_standard_derivatives") ||
      gl::HasDesktopGLFeatures()) {
    AddExtensionString("GL_OES_standard_derivatives");
    feature_flags_.oes_standard_derivatives = true;
    validators_.hint_target.AddValue(GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES);
    validators_.g_l_state.AddValue(GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES);
  }

  if (gfx::HasExtension(extensions, "GL_CHROMIUM_texture_filtering_hint")) {
    AddExtensionString("GL_CHROMIUM_texture_filtering_hint");
    feature_flags_.chromium_texture_filtering_hint = true;
    validators_.hint_target.AddValue(GL_TEXTURE_FILTERING_HINT_CHROMIUM);
    validators_.g_l_state.AddValue(GL_TEXTURE_FILTERING_HINT_CHROMIUM);
  }

  if (gfx::HasExtension(extensions, "GL_OES_EGL_image_external")) {
    AddExtensionString("GL_OES_EGL_image_external");
    feature_flags_.oes_egl_image_external = true;
  }
  if (gfx::HasExtension(extensions, "GL_NV_EGL_stream_consumer_external")) {
    AddExtensionString("GL_NV_EGL_stream_consumer_external");
    feature_flags_.nv_egl_stream_consumer_external = true;
  }

  if (feature_flags_.oes_egl_image_external ||
      feature_flags_.nv_egl_stream_consumer_external) {
    validators_.texture_bind_target.AddValue(GL_TEXTURE_EXTERNAL_OES);
    validators_.get_tex_param_target.AddValue(GL_TEXTURE_EXTERNAL_OES);
    validators_.texture_parameter.AddValue(GL_REQUIRED_TEXTURE_IMAGE_UNITS_OES);
    validators_.g_l_state.AddValue(GL_TEXTURE_BINDING_EXTERNAL_OES);
  }

  if (gfx::HasExtension(extensions, "GL_OES_compressed_ETC1_RGB8_texture") &&
      !gl_version_info_->is_angle) {
    AddExtensionString("GL_OES_compressed_ETC1_RGB8_texture");
    feature_flags_.oes_compressed_etc1_rgb8_texture = true;
    validators_.compressed_texture_format.AddValue(GL_ETC1_RGB8_OES);
    validators_.texture_internal_format_storage.AddValue(GL_ETC1_RGB8_OES);
  }

  if (gfx::HasExtension(extensions, "GL_CHROMIUM_compressed_texture_etc") ||
      (gl_version_info_->is_es3 && !gl_version_info_->is_angle)) {
    AddExtensionString("GL_CHROMIUM_compressed_texture_etc");
    validators_.UpdateETCCompressedTextureFormats();
  }

  if (gfx::HasExtension(extensions, "GL_AMD_compressed_ATC_texture")) {
    AddExtensionString("GL_AMD_compressed_ATC_texture");
    validators_.compressed_texture_format.AddValue(GL_ATC_RGB_AMD);
    validators_.compressed_texture_format.AddValue(
        GL_ATC_RGBA_EXPLICIT_ALPHA_AMD);
    validators_.compressed_texture_format.AddValue(
        GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD);

    validators_.texture_internal_format_storage.AddValue(GL_ATC_RGB_AMD);
    validators_.texture_internal_format_storage.AddValue(
        GL_ATC_RGBA_EXPLICIT_ALPHA_AMD);
    validators_.texture_internal_format_storage.AddValue(
        GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD);
  }

  if (gfx::HasExtension(extensions, "GL_IMG_texture_compression_pvrtc")) {
    AddExtensionString("GL_IMG_texture_compression_pvrtc");
    validators_.compressed_texture_format.AddValue(
        GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG);
    validators_.compressed_texture_format.AddValue(
        GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG);
    validators_.compressed_texture_format.AddValue(
        GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG);
    validators_.compressed_texture_format.AddValue(
        GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG);

    validators_.texture_internal_format_storage.AddValue(
        GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG);
    validators_.texture_internal_format_storage.AddValue(
        GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG);
    validators_.texture_internal_format_storage.AddValue(
        GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG);
    validators_.texture_internal_format_storage.AddValue(
        GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG);
  }

  if (gfx::HasExtension(extensions, "GL_ARB_texture_rectangle") ||
      gfx::HasExtension(extensions, "GL_ANGLE_texture_rectangle") ||
      gl_version_info_->is_desktop_core_profile) {
    AddExtensionString("GL_ARB_texture_rectangle");
    feature_flags_.arb_texture_rectangle = true;
    validators_.texture_bind_target.AddValue(GL_TEXTURE_RECTANGLE_ARB);
    validators_.texture_target.AddValue(GL_TEXTURE_RECTANGLE_ARB);
    validators_.get_tex_param_target.AddValue(GL_TEXTURE_RECTANGLE_ARB);
    validators_.g_l_state.AddValue(GL_TEXTURE_BINDING_RECTANGLE_ARB);
  }

#if defined(OS_MACOSX) || defined(OS_CHROMEOS)
  AddExtensionString("GL_CHROMIUM_ycbcr_420v_image");
  feature_flags_.chromium_image_ycbcr_420v = true;
#endif
  if (feature_flags_.chromium_image_ycbcr_420v) {
    feature_flags_.gpu_memory_buffer_formats.Add(
        gfx::BufferFormat::YUV_420_BIPLANAR);
  }

  if (gfx::HasExtension(extensions, "GL_APPLE_ycbcr_422")) {
    AddExtensionString("GL_CHROMIUM_ycbcr_422_image");
    feature_flags_.chromium_image_ycbcr_422 = true;
    feature_flags_.gpu_memory_buffer_formats.Add(gfx::BufferFormat::UYVY_422);
  }

#if defined(OS_MACOSX)
  feature_flags_.chromium_image_xr30 = base::mac::IsAtLeastOS10_13();
#elif !defined(OS_WIN)
  feature_flags_.chromium_image_xb30 =
      gl_version_info_->IsAtLeastGL(3, 3) ||
      gl_version_info_->IsAtLeastGLES(3, 0) ||
      gfx::HasExtension(extensions, "GL_EXT_texture_type_2_10_10_10_REV");
#endif
  if (feature_flags_.chromium_image_xr30 ||
      feature_flags_.chromium_image_xb30) {
    validators_.texture_internal_format.AddValue(GL_RGB10_A2_EXT);
    validators_.render_buffer_format.AddValue(GL_RGB10_A2_EXT);
    validators_.texture_internal_format_storage.AddValue(GL_RGB10_A2_EXT);
    validators_.pixel_type.AddValue(GL_UNSIGNED_INT_2_10_10_10_REV);
  }
  if (feature_flags_.chromium_image_xr30) {
    feature_flags_.gpu_memory_buffer_formats.Add(
        gfx::BufferFormat::BGRX_1010102);
  }
  if (feature_flags_.chromium_image_xb30) {
    feature_flags_.gpu_memory_buffer_formats.Add(
        gfx::BufferFormat::RGBX_1010102);
  }


  if (gfx::HasExtension(extensions, "GL_ANGLE_texture_usage")) {
    feature_flags_.angle_texture_usage = true;
    AddExtensionString("GL_ANGLE_texture_usage");
    validators_.texture_parameter.AddValue(GL_TEXTURE_USAGE_ANGLE);
  }

  bool have_occlusion_query = gl_version_info_->IsAtLeastGLES(3, 0) ||
                              gl_version_info_->IsAtLeastGL(3, 3);
  bool have_ext_occlusion_query_boolean =
      gfx::HasExtension(extensions, "GL_EXT_occlusion_query_boolean");
  bool have_arb_occlusion_query2 =
      gfx::HasExtension(extensions, "GL_ARB_occlusion_query2");
  bool have_arb_occlusion_query =
      (gl_version_info_->is_desktop_core_profile &&
       gl_version_info_->IsAtLeastGL(1, 5)) ||
      gfx::HasExtension(extensions, "GL_ARB_occlusion_query");

  if (have_occlusion_query || have_ext_occlusion_query_boolean ||
      have_arb_occlusion_query2 || have_arb_occlusion_query) {
    feature_flags_.occlusion_query = have_arb_occlusion_query;
    if (context_type_ == CONTEXT_TYPE_OPENGLES2) {
      AddExtensionString("GL_EXT_occlusion_query_boolean");
    }
    feature_flags_.occlusion_query_boolean = true;
    feature_flags_.use_arb_occlusion_query2_for_occlusion_query_boolean =
        !have_ext_occlusion_query_boolean &&
        (have_arb_occlusion_query2 || (gl_version_info_->IsAtLeastGL(3, 3) &&
                                       gl_version_info_->IsLowerThanGL(4, 3)));
    feature_flags_.use_arb_occlusion_query_for_occlusion_query_boolean =
        !have_ext_occlusion_query_boolean && have_arb_occlusion_query &&
        !have_arb_occlusion_query2;
  }

  if (gfx::HasExtension(extensions, "GL_ANGLE_instanced_arrays") ||
      (gfx::HasExtension(extensions, "GL_ARB_instanced_arrays") &&
       gfx::HasExtension(extensions, "GL_ARB_draw_instanced")) ||
      gl_version_info_->is_es3 || gl_version_info_->is_desktop_core_profile) {
    AddExtensionString("GL_ANGLE_instanced_arrays");
    feature_flags_.angle_instanced_arrays = true;
    validators_.vertex_attribute.AddValue(GL_VERTEX_ATTRIB_ARRAY_DIVISOR_ANGLE);
  }

  bool have_es2_draw_buffers_vendor_agnostic =
      gl_version_info_->is_desktop_core_profile ||
      gfx::HasExtension(extensions, "GL_ARB_draw_buffers") ||
      gfx::HasExtension(extensions, "GL_EXT_draw_buffers");
  bool can_emulate_es2_draw_buffers_on_es3_nv =
      gl_version_info_->is_es3 &&
      gfx::HasExtension(extensions, "GL_NV_draw_buffers");
  bool is_webgl_compatibility_context =
      gfx::HasExtension(extensions, "GL_ANGLE_webgl_compatibility");
  bool have_es2_draw_buffers =
      !workarounds_.disable_ext_draw_buffers &&
      (have_es2_draw_buffers_vendor_agnostic ||
       can_emulate_es2_draw_buffers_on_es3_nv) &&
      (context_type_ == CONTEXT_TYPE_OPENGLES2 ||
       (context_type_ == CONTEXT_TYPE_WEBGL1 &&
        IsWebGLDrawBuffersSupported(is_webgl_compatibility_context,
                                    depth_texture_format,
                                    depth_stencil_texture_format)));
  if (have_es2_draw_buffers) {
    AddExtensionString("GL_EXT_draw_buffers");
    feature_flags_.ext_draw_buffers = true;

    feature_flags_.nv_draw_buffers = can_emulate_es2_draw_buffers_on_es3_nv &&
                                     !have_es2_draw_buffers_vendor_agnostic;
  }

  if (IsWebGL2OrES3OrHigherContext() || have_es2_draw_buffers) {
    GLint max_color_attachments = 0;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &max_color_attachments);
    for (GLenum i = GL_COLOR_ATTACHMENT1_EXT;
         i < static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + max_color_attachments);
         ++i) {
      validators_.attachment.AddValue(i);
      validators_.attachment_query.AddValue(i);
    }
    static_assert(GL_COLOR_ATTACHMENT0_EXT == GL_COLOR_ATTACHMENT0,
                  "GL_COLOR_ATTACHMENT0_EXT should equal GL_COLOR_ATTACHMENT0");

    validators_.g_l_state.AddValue(GL_MAX_COLOR_ATTACHMENTS_EXT);
    validators_.g_l_state.AddValue(GL_MAX_DRAW_BUFFERS_ARB);
    GLint max_draw_buffers = 0;
    glGetIntegerv(GL_MAX_DRAW_BUFFERS_ARB, &max_draw_buffers);
    for (GLenum i = GL_DRAW_BUFFER0_ARB;
         i < static_cast<GLenum>(GL_DRAW_BUFFER0_ARB + max_draw_buffers); ++i) {
      validators_.g_l_state.AddValue(i);
    }
  }

  if (gl_version_info_->is_es3 ||
      gfx::HasExtension(extensions, "GL_EXT_blend_minmax") ||
      gl::HasDesktopGLFeatures()) {
    AddExtensionString("GL_EXT_blend_minmax");
    validators_.equation.AddValue(GL_MIN_EXT);
    validators_.equation.AddValue(GL_MAX_EXT);
    static_assert(GL_MIN_EXT == GL_MIN && GL_MAX_EXT == GL_MAX,
                  "min & max variations must match");
  }

  if (gfx::HasExtension(extensions, "GL_EXT_frag_depth") ||
      gl::HasDesktopGLFeatures()) {
    AddExtensionString("GL_EXT_frag_depth");
    feature_flags_.ext_frag_depth = true;
  }

  if (gfx::HasExtension(extensions, "GL_EXT_shader_texture_lod") ||
      gl::HasDesktopGLFeatures()) {
    AddExtensionString("GL_EXT_shader_texture_lod");
    feature_flags_.ext_shader_texture_lod = true;
  }

  bool ui_gl_fence_works = gl::GLFence::IsSupported();
  UMA_HISTOGRAM_BOOLEAN("GPU.FenceSupport", ui_gl_fence_works);

  feature_flags_.map_buffer_range =
      gl_version_info_->is_es3 || gl_version_info_->is_desktop_core_profile ||
      gfx::HasExtension(extensions, "GL_ARB_map_buffer_range") ||
      gfx::HasExtension(extensions, "GL_EXT_map_buffer_range");

  if (has_pixel_buffers && ui_gl_fence_works &&
      !workarounds_.disable_async_readpixels) {
    feature_flags_.use_async_readpixels = true;
  }

  if (gl_version_info_->is_es3 ||
      gfx::HasExtension(extensions, "GL_ARB_sampler_objects")) {
    feature_flags_.enable_samplers = true;
  }

  if ((gl_version_info_->is_es3 ||
       gfx::HasExtension(extensions, "GL_EXT_discard_framebuffer")) &&
      !workarounds_.disable_discard_framebuffer) {
    AddExtensionString("GL_EXT_discard_framebuffer");
    feature_flags_.ext_discard_framebuffer = true;
  }

  if (ui_gl_fence_works) {
    AddExtensionString("GL_CHROMIUM_sync_query");
    feature_flags_.chromium_sync_query = true;
  }

  if (!workarounds_.disable_blend_equation_advanced) {
    bool blend_equation_advanced_coherent =
        gfx::HasExtension(extensions,
                          "GL_NV_blend_equation_advanced_coherent") ||
        gfx::HasExtension(extensions,
                          "GL_KHR_blend_equation_advanced_coherent");

    if (blend_equation_advanced_coherent ||
        gfx::HasExtension(extensions, "GL_NV_blend_equation_advanced") ||
        gfx::HasExtension(extensions, "GL_KHR_blend_equation_advanced")) {
      const GLenum equations[] = {
          GL_MULTIPLY_KHR,       GL_SCREEN_KHR,    GL_OVERLAY_KHR,
          GL_DARKEN_KHR,         GL_LIGHTEN_KHR,   GL_COLORDODGE_KHR,
          GL_COLORBURN_KHR,      GL_HARDLIGHT_KHR, GL_SOFTLIGHT_KHR,
          GL_DIFFERENCE_KHR,     GL_EXCLUSION_KHR, GL_HSL_HUE_KHR,
          GL_HSL_SATURATION_KHR, GL_HSL_COLOR_KHR, GL_HSL_LUMINOSITY_KHR};

      for (GLenum equation : equations)
        validators_.equation.AddValue(equation);
      if (blend_equation_advanced_coherent)
        AddExtensionString("GL_KHR_blend_equation_advanced_coherent");

      AddExtensionString("GL_KHR_blend_equation_advanced");
      feature_flags_.blend_equation_advanced = true;
      feature_flags_.blend_equation_advanced_coherent =
          blend_equation_advanced_coherent;
    }
  }

  if (gfx::HasExtension(extensions, "GL_NV_framebuffer_mixed_samples")) {
    AddExtensionString("GL_CHROMIUM_framebuffer_mixed_samples");
    feature_flags_.chromium_framebuffer_mixed_samples = true;
    validators_.g_l_state.AddValue(GL_COVERAGE_MODULATION_CHROMIUM);
  }

  if (gfx::HasExtension(extensions, "GL_NV_path_rendering")) {
    bool has_dsa = gl_version_info_->IsAtLeastGL(4, 5) ||
                   gfx::HasExtension(extensions, "GL_EXT_direct_state_access");
    bool has_piq =
        gl_version_info_->IsAtLeastGL(4, 3) ||
        gfx::HasExtension(extensions, "GL_ARB_program_interface_query");
    bool has_fms = feature_flags_.chromium_framebuffer_mixed_samples;
    if ((gl_version_info_->IsAtLeastGLES(3, 1) ||
         (gl_version_info_->IsAtLeastGL(3, 2) && has_dsa && has_piq)) &&
        has_fms) {
      AddExtensionString("GL_CHROMIUM_path_rendering");
      feature_flags_.chromium_path_rendering = true;
      validators_.g_l_state.AddValue(GL_PATH_MODELVIEW_MATRIX_CHROMIUM);
      validators_.g_l_state.AddValue(GL_PATH_PROJECTION_MATRIX_CHROMIUM);
      validators_.g_l_state.AddValue(GL_PATH_STENCIL_FUNC_CHROMIUM);
      validators_.g_l_state.AddValue(GL_PATH_STENCIL_REF_CHROMIUM);
      validators_.g_l_state.AddValue(GL_PATH_STENCIL_VALUE_MASK_CHROMIUM);
    }
  }

  if ((gl_version_info_->is_es3 || gl_version_info_->is_desktop_core_profile ||
       gfx::HasExtension(extensions, "GL_EXT_texture_rg") ||
       gfx::HasExtension(extensions, "GL_ARB_texture_rg")) &&
      IsGL_REDSupportedOnFBOs()) {
    feature_flags_.ext_texture_rg = true;
    AddExtensionString("GL_EXT_texture_rg");

    validators_.texture_format.AddValue(GL_RED_EXT);
    validators_.texture_format.AddValue(GL_RG_EXT);

    validators_.texture_internal_format.AddValue(GL_RED_EXT);
    validators_.texture_internal_format.AddValue(GL_R8_EXT);
    validators_.texture_internal_format.AddValue(GL_RG_EXT);
    validators_.texture_internal_format.AddValue(GL_RG8_EXT);

    validators_.read_pixel_format.AddValue(GL_RED_EXT);
    validators_.read_pixel_format.AddValue(GL_RG_EXT);

    validators_.render_buffer_format.AddValue(GL_R8_EXT);
    validators_.render_buffer_format.AddValue(GL_RG8_EXT);

    validators_.texture_unsized_internal_format.AddValue(GL_RED_EXT);
    validators_.texture_unsized_internal_format.AddValue(GL_RG_EXT);

    validators_.texture_internal_format_storage.AddValue(GL_R8_EXT);
    validators_.texture_internal_format_storage.AddValue(GL_RG8_EXT);

    feature_flags_.gpu_memory_buffer_formats.Add(gfx::BufferFormat::R_8);
    feature_flags_.gpu_memory_buffer_formats.Add(gfx::BufferFormat::RG_88);
  }
  UMA_HISTOGRAM_BOOLEAN("GPU.TextureRG", feature_flags_.ext_texture_rg);

  if (gl_version_info_->is_desktop_core_profile ||
      (gl_version_info_->IsAtLeastGL(2, 1) &&
       gfx::HasExtension(extensions, "GL_ARB_texture_rg")) ||
      gfx::HasExtension(extensions, "GL_EXT_texture_norm16")) {
    feature_flags_.ext_texture_norm16 = true;
    g_r16_is_present = true;

    validators_.pixel_type.AddValue(GL_UNSIGNED_SHORT);
    validators_.texture_format.AddValue(GL_RED_EXT);
    validators_.texture_internal_format.AddValue(GL_R16_EXT);
    validators_.texture_internal_format.AddValue(GL_RED_EXT);
    validators_.texture_unsized_internal_format.AddValue(GL_RED_EXT);
    validators_.texture_internal_format_storage.AddValue(GL_R16_EXT);

    feature_flags_.gpu_memory_buffer_formats.Add(gfx::BufferFormat::R_16);
  }

  UMA_HISTOGRAM_ENUMERATION(
      "GPU.TextureR16Ext_LuminanceF16", GpuTextureUMAHelper(),
      static_cast<int>(GpuTextureResultR16_L16::kMax) + 1);

  if (enable_es3 && gfx::HasExtension(extensions, "GL_EXT_window_rectangles")) {
    AddExtensionString("GL_EXT_window_rectangles");
    feature_flags_.ext_window_rectangles = true;
    validators_.g_l_state.AddValue(GL_WINDOW_RECTANGLE_MODE_EXT);
    validators_.g_l_state.AddValue(GL_MAX_WINDOW_RECTANGLES_EXT);
    validators_.g_l_state.AddValue(GL_NUM_WINDOW_RECTANGLES_EXT);
    validators_.indexed_g_l_state.AddValue(GL_WINDOW_RECTANGLE_EXT);
  }

  bool has_opengl_dual_source_blending =
      gl_version_info_->IsAtLeastGL(3, 3) ||
      (gl_version_info_->IsAtLeastGL(3, 2) &&
       gfx::HasExtension(extensions, "GL_ARB_blend_func_extended"));
  if (!disable_shader_translator_ && !workarounds_.get_frag_data_info_bug &&
      ((gl_version_info_->IsAtLeastGL(3, 2) &&
        has_opengl_dual_source_blending) ||
       (gl_version_info_->IsAtLeastGLES(3, 0) &&
        gfx::HasExtension(extensions, "GL_EXT_blend_func_extended")))) {
    feature_flags_.ext_blend_func_extended = true;
    AddExtensionString("GL_EXT_blend_func_extended");

    validators_.dst_blend_factor.AddValue(GL_SRC_ALPHA_SATURATE_EXT);

    validators_.src_blend_factor.AddValue(GL_SRC1_ALPHA_EXT);
    validators_.dst_blend_factor.AddValue(GL_SRC1_ALPHA_EXT);
    validators_.src_blend_factor.AddValue(GL_SRC1_COLOR_EXT);
    validators_.dst_blend_factor.AddValue(GL_SRC1_COLOR_EXT);
    validators_.src_blend_factor.AddValue(GL_ONE_MINUS_SRC1_COLOR_EXT);
    validators_.dst_blend_factor.AddValue(GL_ONE_MINUS_SRC1_COLOR_EXT);
    validators_.src_blend_factor.AddValue(GL_ONE_MINUS_SRC1_ALPHA_EXT);
    validators_.dst_blend_factor.AddValue(GL_ONE_MINUS_SRC1_ALPHA_EXT);
    validators_.g_l_state.AddValue(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS_EXT);
  }

#if !defined(OS_MACOSX)
  if (workarounds_.ignore_egl_sync_failures) {
    gl::GLFenceEGL::SetIgnoreFailures();
  }
#endif

  if (workarounds_.avoid_egl_image_target_texture_reuse) {
    TextureDefinition::AvoidEGLTargetTextureReuse();
  }

  if (gl_version_info_->IsLowerThanGL(4, 3)) {
    feature_flags_.emulate_primitive_restart_fixed_index = true;
  }

  feature_flags_.angle_robust_client_memory =
      gfx::HasExtension(extensions, "GL_ANGLE_robust_client_memory");

  feature_flags_.khr_debug = gl_version_info_->IsAtLeastGL(4, 3) ||
                             gl_version_info_->IsAtLeastGLES(3, 2) ||
                             gfx::HasExtension(extensions, "GL_KHR_debug");

  feature_flags_.chromium_gpu_fence = gl::GLFence::IsGpuFenceSupported();
  if (feature_flags_.chromium_gpu_fence)
    AddExtensionString("GL_CHROMIUM_gpu_fence");

  feature_flags_.chromium_bind_generates_resource =
      gfx::HasExtension(extensions, "GL_CHROMIUM_bind_generates_resource");
  feature_flags_.angle_webgl_compatibility = is_webgl_compatibility_context;
  feature_flags_.chromium_copy_texture =
      gfx::HasExtension(extensions, "GL_CHROMIUM_copy_texture");
  feature_flags_.chromium_copy_compressed_texture =
      gfx::HasExtension(extensions, "GL_CHROMIUM_copy_compressed_texture");
  feature_flags_.angle_client_arrays =
      gfx::HasExtension(extensions, "GL_ANGLE_client_arrays");
  feature_flags_.angle_request_extension =
      gfx::HasExtension(extensions, "GL_ANGLE_request_extension");
  feature_flags_.ext_debug_marker =
      gfx::HasExtension(extensions, "GL_EXT_debug_marker");
  feature_flags_.arb_robustness =
      gfx::HasExtension(extensions, "GL_ARB_robustness");
  feature_flags_.khr_robustness =
      gfx::HasExtension(extensions, "GL_KHR_robustness");
  feature_flags_.ext_robustness =
      gfx::HasExtension(extensions, "GL_EXT_robustness");
  feature_flags_.ext_pixel_buffer_object =
      gfx::HasExtension(extensions, "GL_ARB_pixel_buffer_object") ||
      gfx::HasExtension(extensions, "GL_NV_pixel_buffer_object");
  feature_flags_.ext_unpack_subimage =
      gfx::HasExtension(extensions, "GL_EXT_unpack_subimage");
  feature_flags_.oes_rgb8_rgba8 =
      gfx::HasExtension(extensions, "GL_OES_rgb8_rgba8");
  feature_flags_.angle_robust_resource_initialization =
      gfx::HasExtension(extensions, "GL_ANGLE_robust_resource_initialization");
  feature_flags_.nv_fence = gfx::HasExtension(extensions, "GL_NV_fence");

  feature_flags_.unpremultiply_and_dither_copy = !is_passthrough_cmd_decoder_;
  if (feature_flags_.unpremultiply_and_dither_copy)
    AddExtensionString("GL_CHROMIUM_unpremultiply_and_dither_copy");

  feature_flags_.separate_stencil_ref_mask_writemask =
      !(gl_version_info_->is_d3d) && !IsWebGLContext();

  if (gfx::HasExtension(extensions, "GL_MESA_framebuffer_flip_y")) {
    feature_flags_.mesa_framebuffer_flip_y = true;
    validators_.framebuffer_parameter.AddValue(GL_FRAMEBUFFER_FLIP_Y_MESA);
    AddExtensionString("GL_MESA_framebuffer_flip_y");
  }

  if (is_passthrough_cmd_decoder_ &&
      gfx::HasExtension(extensions, "GL_OVR_multiview2")) {
    AddExtensionString("GL_OVR_multiview2");
    feature_flags_.ovr_multiview2 = true;
  }

  if (is_passthrough_cmd_decoder_ &&
      gfx::HasExtension(extensions, "GL_KHR_parallel_shader_compile")) {
    AddExtensionString("GL_KHR_parallel_shader_compile");
    feature_flags_.khr_parallel_shader_compile = true;
    validators_.g_l_state.AddValue(GL_MAX_SHADER_COMPILER_THREADS_KHR);
     validators_.shader_parameter.AddValue(GL_COMPLETION_STATUS_KHR);
     validators_.program_parameter.AddValue(GL_COMPLETION_STATUS_KHR);

    AddExtensionString("GL_CHROMIUM_completion_query");
    feature_flags_.chromium_completion_query = true;
   }
 
   if (gfx::HasExtension(extensions, "GL_KHR_robust_buffer_access_behavior")) {
    AddExtensionString("GL_KHR_robust_buffer_access_behavior");
    feature_flags_.khr_robust_buffer_access_behavior = true;
  }

  if (!is_passthrough_cmd_decoder_ ||
      gfx::HasExtension(extensions, "GL_ANGLE_multi_draw")) {
    feature_flags_.webgl_multi_draw = true;
    AddExtensionString("GL_WEBGL_multi_draw");

    if (gfx::HasExtension(extensions, "GL_ANGLE_instanced_arrays") ||
        feature_flags_.angle_instanced_arrays || gl_version_info_->is_es3 ||
        gl_version_info_->is_desktop_core_profile) {
      feature_flags_.webgl_multi_draw_instanced = true;
      AddExtensionString("GL_WEBGL_multi_draw_instanced");
    }
  }

  if (gfx::HasExtension(extensions, "GL_NV_internalformat_sample_query")) {
    feature_flags_.nv_internalformat_sample_query = true;
  }

  if (gfx::HasExtension(extensions,
                        "GL_AMD_framebuffer_multisample_advanced")) {
    feature_flags_.amd_framebuffer_multisample_advanced = true;
    AddExtensionString("GL_AMD_framebuffer_multisample_advanced");
  }
}
