error::Error GLES2DecoderImpl::HandleReadPixels(
    uint32 immediate_data_size, const gles2::ReadPixels& c) {
  GLint x = c.x;
  GLint y = c.y;
  GLsizei width = c.width;
  GLsizei height = c.height;
  GLenum format = c.format;
  GLenum type = c.type;
  if (width < 0 || height < 0) {
    SetGLError(GL_INVALID_VALUE, "glReadPixels", "dimensions < 0");
    return error::kNoError;
  }
  typedef gles2::ReadPixels::Result Result;
  uint32 pixels_size;
  if (!GLES2Util::ComputeImageDataSizes(
      width, height, format, type, pack_alignment_, &pixels_size, NULL, NULL)) {
    return error::kOutOfBounds;
  }
  void* pixels = GetSharedMemoryAs<void*>(
      c.pixels_shm_id, c.pixels_shm_offset, pixels_size);
  Result* result = GetSharedMemoryAs<Result*>(
        c.result_shm_id, c.result_shm_offset, sizeof(*result));
  if (!pixels || !result) {
    return error::kOutOfBounds;
  }

  if (!validators_->read_pixel_format.IsValid(format)) {
    SetGLErrorInvalidEnum("glReadPixels", format, "format");
    return error::kNoError;
  }
  if (!validators_->pixel_type.IsValid(type)) {
    SetGLErrorInvalidEnum("glReadPixels", type, "type");
    return error::kNoError;
  }
  if (width == 0 || height == 0) {
    return error::kNoError;
  }

   gfx::Size max_size = GetBoundReadFrameBufferSize();
 
  int32 max_x;
  int32 max_y;
  if (!SafeAddInt32(x, width, &max_x) || !SafeAddInt32(y, height, &max_y)) {
     SetGLError(GL_INVALID_VALUE, "glReadPixels", "dimensions out of range");
     return error::kNoError;
   }

  if (!CheckBoundFramebuffersValid("glReadPixels")) {
    return error::kNoError;
  }

  CopyRealGLErrorsToWrapper();

  ScopedResolvedFrameBufferBinder binder(this, false, true);

  if (x < 0 || y < 0 || max_x > max_size.width() || max_y > max_size.height()) {
    uint32 temp_size;
    uint32 unpadded_row_size;
    uint32 padded_row_size;
    if (!GLES2Util::ComputeImageDataSizes(
        width, 2, format, type, pack_alignment_, &temp_size,
        &unpadded_row_size, &padded_row_size)) {
      SetGLError(GL_INVALID_VALUE, "glReadPixels", "dimensions out of range");
      return error::kNoError;
    }

    GLint dest_x_offset = std::max(-x, 0);
    uint32 dest_row_offset;
    if (!GLES2Util::ComputeImageDataSizes(
        dest_x_offset, 1, format, type, pack_alignment_, &dest_row_offset, NULL,
        NULL)) {
      SetGLError(GL_INVALID_VALUE, "glReadPixels", "dimensions out of range");
      return error::kNoError;
    }

    int8* dst = static_cast<int8*>(pixels);
    GLint read_x = std::max(0, x);
    GLint read_end_x = std::max(0, std::min(max_size.width(), max_x));
    GLint read_width = read_end_x - read_x;
    for (GLint yy = 0; yy < height; ++yy) {
      GLint ry = y + yy;

      memset(dst, 0, unpadded_row_size);

      if (ry >= 0 && ry < max_size.height() && read_width > 0) {
        glReadPixels(
            read_x, ry, read_width, 1, format, type, dst + dest_row_offset);
      }
      dst += padded_row_size;
    }
  } else {
    glReadPixels(x, y, width, height, format, type, pixels);
  }
  GLenum error = PeekGLError();
  if (error == GL_NO_ERROR) {
    *result = true;

    GLenum read_format = GetBoundReadFrameBufferInternalFormat();
    uint32 channels_exist = GLES2Util::GetChannelsForFormat(read_format);
    if ((channels_exist & 0x0008) == 0 &&
        !feature_info_->feature_flags().disable_workarounds) {
      uint32 temp_size;

      uint32 unpadded_row_size;
      uint32 padded_row_size;
      if (!GLES2Util::ComputeImageDataSizes(
          width, 2, format, type, pack_alignment_, &temp_size,
          &unpadded_row_size, &padded_row_size)) {
        SetGLError(GL_INVALID_VALUE, "glReadPixels", "dimensions out of range");
        return error::kNoError;
      }
      if (type != GL_UNSIGNED_BYTE) {
        SetGLError(
            GL_INVALID_OPERATION, "glReadPixels",
            "unsupported readPixel format");
        return error::kNoError;
      }
      switch (format) {
        case GL_RGBA:
        case GL_BGRA_EXT:
        case GL_ALPHA: {
          int offset = (format == GL_ALPHA) ? 0 : 3;
          int step = (format == GL_ALPHA) ? 1 : 4;
          uint8* dst = static_cast<uint8*>(pixels) + offset;
          for (GLint yy = 0; yy < height; ++yy) {
            uint8* end = dst + unpadded_row_size;
            for (uint8* d = dst; d < end; d += step) {
              *d = 255;
            }
            dst += padded_row_size;
          }
          break;
        }
        default:
          break;
      }
    }
  }

  return error::kNoError;
}
