void GLES2DecoderImpl::GetTexParameterImpl(
    GLenum target, GLenum pname, GLfloat* fparams, GLint* iparams,
    const char* function_name) {
  TextureRef* texture_ref = texture_manager()->GetTextureInfoForTarget(
      &state_, target);
  if (!texture_ref) {
    LOCAL_SET_GL_ERROR(
        GL_INVALID_OPERATION, function_name, "unknown texture for target");
    return;
  }
  Texture* texture = texture_ref->texture();
  switch (pname) {
    case GL_TEXTURE_MAX_ANISOTROPY_EXT:
      if (workarounds().init_texture_max_anisotropy) {
        texture->InitTextureMaxAnisotropyIfNeeded(target);
      }
      break;
    case GL_TEXTURE_IMMUTABLE_LEVELS:
      if (gl_version_info().IsLowerThanGL(4, 2)) {
        GLint levels = texture->GetImmutableLevels();
        if (fparams) {
          fparams[0] = static_cast<GLfloat>(levels);
        } else {
          iparams[0] = levels;
        }
         return;
       }
       break;
      if (workarounds().use_shadowed_tex_level_params) {
        if (fparams) {
          fparams[0] = static_cast<GLfloat>(texture->base_level());
        } else {
          iparams[0] = texture->base_level();
        }
        return;
       }
      break;
     case GL_TEXTURE_MAX_LEVEL:
      if (workarounds().use_shadowed_tex_level_params) {
        if (fparams) {
          fparams[0] = static_cast<GLfloat>(texture->max_level());
        } else {
          iparams[0] = texture->max_level();
        }
        return;
       }
      break;
     case GL_TEXTURE_SWIZZLE_R:
       if (fparams) {
         fparams[0] = static_cast<GLfloat>(texture->swizzle_r());
      } else {
        iparams[0] = texture->swizzle_r();
      }
      return;
    case GL_TEXTURE_SWIZZLE_G:
      if (fparams) {
        fparams[0] = static_cast<GLfloat>(texture->swizzle_g());
      } else {
        iparams[0] = texture->swizzle_g();
      }
      return;
    case GL_TEXTURE_SWIZZLE_B:
      if (fparams) {
        fparams[0] = static_cast<GLfloat>(texture->swizzle_b());
      } else {
        iparams[0] = texture->swizzle_b();
      }
      return;
    case GL_TEXTURE_SWIZZLE_A:
      if (fparams) {
        fparams[0] = static_cast<GLfloat>(texture->swizzle_a());
      } else {
        iparams[0] = texture->swizzle_a();
      }
      return;
    default:
      break;
  }
  if (fparams) {
    api()->glGetTexParameterfvFn(target, pname, fparams);
  } else {
    api()->glGetTexParameterivFn(target, pname, iparams);
  }
}
