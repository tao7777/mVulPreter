void WebGLRenderingContextBase::TexImageHelperImageData(
    TexImageFunctionID function_id,
    GLenum target,
    GLint level,
    GLint internalformat,
    GLint border,
    GLenum format,
    GLenum type,
    GLsizei depth,
    GLint xoffset,
    GLint yoffset,
    GLint zoffset,
    ImageData* pixels,
    const IntRect& source_image_rect,
    GLint unpack_image_height) {
  const char* func_name = GetTexImageFunctionName(function_id);
  if (isContextLost())
    return;
  DCHECK(pixels);
  if (pixels->data()->BufferBase()->IsNeutered()) {
    SynthesizeGLError(GL_INVALID_VALUE, func_name,
                      "The source data has been neutered.");
    return;
  }
  if (!ValidateTexImageBinding(func_name, function_id, target))
    return;
  TexImageFunctionType function_type;
  if (function_id == kTexImage2D || function_id == kTexImage3D)
    function_type = kTexImage;
  else
    function_type = kTexSubImage;
  if (!ValidateTexFunc(func_name, function_type, kSourceImageData, target,
                       level, internalformat, pixels->width(), pixels->height(),
                       depth, border, format, type, xoffset, yoffset, zoffset))
    return;

  bool selecting_sub_rectangle = false;
  if (!ValidateTexImageSubRectangle(
          func_name, function_id, pixels, source_image_rect, depth,
          unpack_image_height, &selecting_sub_rectangle)) {
    return;
  }
  IntRect adjusted_source_image_rect = source_image_rect;
  if (unpack_flip_y_) {
    adjusted_source_image_rect.SetY(pixels->height() -
                                    adjusted_source_image_rect.MaxY());
  }

  Vector<uint8_t> data;
  bool need_conversion = true;
  if (!unpack_flip_y_ && !unpack_premultiply_alpha_ && format == GL_RGBA &&
      type == GL_UNSIGNED_BYTE && !selecting_sub_rectangle && depth == 1) {
    need_conversion = false;
  } else {
    if (type == GL_UNSIGNED_INT_10F_11F_11F_REV) {
      type = GL_FLOAT;
    }
    if (!WebGLImageConversion::ExtractImageData(
            pixels->data()->Data(),
            WebGLImageConversion::DataFormat::kDataFormatRGBA8, pixels->Size(),
            adjusted_source_image_rect, depth, unpack_image_height, format,
            type, unpack_flip_y_, unpack_premultiply_alpha_, data)) {
      SynthesizeGLError(GL_INVALID_VALUE, func_name, "bad image data");
      return;
    }
  }
  ScopedUnpackParametersResetRestore temporary_reset_unpack(this);
  const uint8_t* bytes = need_conversion ? data.data() : pixels->data()->Data();
  if (function_id == kTexImage2D) {
    DCHECK_EQ(unpack_image_height, 0);
    TexImage2DBase(
        target, level, internalformat, adjusted_source_image_rect.Width(),
        adjusted_source_image_rect.Height(), border, format, type, bytes);
  } else if (function_id == kTexSubImage2D) {
    DCHECK_EQ(unpack_image_height, 0);
    ContextGL()->TexSubImage2D(
        target, level, xoffset, yoffset, adjusted_source_image_rect.Width(),
         adjusted_source_image_rect.Height(), format, type, bytes);
   } else {
     GLint upload_height = adjusted_source_image_rect.Height();
     if (function_id == kTexImage3D) {
       ContextGL()->TexImage3D(target, level, internalformat,
                               adjusted_source_image_rect.Width(), upload_height,
                              depth, border, format, type, bytes);
    } else {
      DCHECK_EQ(function_id, kTexSubImage3D);
      ContextGL()->TexSubImage3D(target, level, xoffset, yoffset, zoffset,
                                 adjusted_source_image_rect.Width(),
                                 upload_height, depth, format, type, bytes);
    }
  }
}
