void GLES2DecoderImpl::TexStorageImpl(GLenum target,
                                      GLsizei levels,
                                      GLenum internal_format,
                                      GLsizei width,
                                      GLsizei height,
                                      GLsizei depth,
                                      ContextState::Dimension dimension,
                                      const char* function_name) {
  if (levels == 0) {
    LOCAL_SET_GL_ERROR(GL_INVALID_VALUE, function_name, "levels == 0");
    return;
  }
  bool is_compressed_format = IsCompressedTextureFormat(internal_format);
  if (is_compressed_format && target == GL_TEXTURE_3D) {
    LOCAL_SET_GL_ERROR(
        GL_INVALID_OPERATION, function_name, "target invalid for format");
    return;
  }
  bool is_invalid_texstorage_size = width < 1 || height < 1 || depth < 1;
  if (!texture_manager()->ValidForTarget(target, 0, width, height, depth) ||
      is_invalid_texstorage_size ||
      TextureManager::ComputeMipMapCount(target, width, height, depth) <
          levels) {
    LOCAL_SET_GL_ERROR(
        GL_INVALID_VALUE, function_name, "dimensions out of range");
    return;
  }
  TextureRef* texture_ref = texture_manager()->GetTextureInfoForTarget(
      &state_, target);
  if (!texture_ref) {
    LOCAL_SET_GL_ERROR(
        GL_INVALID_OPERATION, function_name, "unknown texture for target");
    return;
  }
  Texture* texture = texture_ref->texture();
  if (texture->IsAttachedToFramebuffer()) {
    framebuffer_state_.clear_state_dirty = true;
  }
  if (texture->IsImmutable()) {
    LOCAL_SET_GL_ERROR(
        GL_INVALID_OPERATION, function_name, "texture is immutable");
    return;
  }

  GLenum format = TextureManager::ExtractFormatFromStorageFormat(
      internal_format);
  GLenum type = TextureManager::ExtractTypeFromStorageFormat(internal_format);

  {
    GLsizei level_width = width;
    GLsizei level_height = height;
    GLsizei level_depth = depth;
    base::CheckedNumeric<uint32_t> estimated_size(0);
    PixelStoreParams params;
    params.alignment = 1;
    for (int ii = 0; ii < levels; ++ii) {
      uint32_t size;
      if (is_compressed_format) {
        GLsizei level_size;
        if (!GetCompressedTexSizeInBytes(
                function_name, level_width, level_height, level_depth,
                internal_format, &level_size, state_.GetErrorState())) {
          return;
        }
        size = static_cast<uint32_t>(level_size);
      } else {
        if (!GLES2Util::ComputeImageDataSizesES3(level_width,
                                                 level_height,
                                                 level_depth,
                                                 format, type,
                                                 params,
                                                 &size,
                                                 nullptr, nullptr,
                                                 nullptr, nullptr)) {
          LOCAL_SET_GL_ERROR(
              GL_OUT_OF_MEMORY, function_name, "dimensions too large");
          return;
        }
      }
      estimated_size += size;
      level_width = std::max(1, level_width >> 1);
      level_height = std::max(1, level_height >> 1);
      if (target == GL_TEXTURE_3D)
        level_depth = std::max(1, level_depth >> 1);
    }
    if (!estimated_size.IsValid()) {
      LOCAL_SET_GL_ERROR(GL_OUT_OF_MEMORY, function_name, "out of memory");
      return;
    }
  }

  GLenum compatibility_internal_format =
      texture_manager()->AdjustTexStorageFormat(feature_info_.get(),
                                                internal_format);

  const CompressedFormatInfo* format_info =
      GetCompressedFormatInfo(internal_format);
  if (format_info != nullptr && !format_info->support_check(*feature_info_)) {
     compatibility_internal_format = format_info->decompressed_internal_format;
   }
 
   {
     GLsizei level_width = width;
     GLsizei level_height = height;
    GLsizei level_depth = depth;

    GLenum adjusted_internal_format =
        feature_info_->IsWebGL1OrES2Context() ? format : internal_format;
    for (int ii = 0; ii < levels; ++ii) {
      if (target == GL_TEXTURE_CUBE_MAP) {
        for (int jj = 0; jj < 6; ++jj) {
          GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + jj;
          texture_manager()->SetLevelInfo(
              texture_ref, face, ii, adjusted_internal_format, level_width,
              level_height, 1, 0, format, type, gfx::Rect());
        }
      } else {
        texture_manager()->SetLevelInfo(
            texture_ref, target, ii, adjusted_internal_format, level_width,
            level_height, level_depth, 0, format, type, gfx::Rect());
      }
      level_width = std::max(1, level_width >> 1);
      level_height = std::max(1, level_height >> 1);
      if (target == GL_TEXTURE_3D)
        level_depth = std::max(1, level_depth >> 1);
    }
     texture->ApplyFormatWorkarounds(feature_info_.get());
     texture->SetImmutable(true);
   }

  if (workarounds().reset_base_mipmap_level_before_texstorage &&
      texture->base_level() > 0)
    api()->glTexParameteriFn(target, GL_TEXTURE_BASE_LEVEL, 0);

  // TODO(zmo): We might need to emulate TexStorage using TexImage or
  // CompressedTexImage on Mac OSX where we expose ES3 APIs when the underlying
  // driver is lower than 4.2 and ARB_texture_storage extension doesn't exist.
  if (dimension == ContextState::k2D) {
    api()->glTexStorage2DEXTFn(target, levels, compatibility_internal_format,
                               width, height);
  } else {
    api()->glTexStorage3DFn(target, levels, compatibility_internal_format,
                            width, height, depth);
  }
  if (workarounds().reset_base_mipmap_level_before_texstorage &&
      texture->base_level() > 0) {
    // Note base_level is already clamped due to texture->SetImmutable(true).
    // This is necessary for certain NVidia Linux drivers; otherwise they
    // may trigger segmentation fault. See https://crbug.com/877874.
    api()->glTexParameteriFn(target, GL_TEXTURE_BASE_LEVEL,
                             texture->base_level());
  }
 }
