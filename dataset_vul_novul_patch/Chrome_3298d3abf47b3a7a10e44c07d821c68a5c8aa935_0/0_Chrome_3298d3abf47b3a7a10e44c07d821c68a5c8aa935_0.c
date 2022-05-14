void WebGLRenderingContextBase::TexImageImpl(
    TexImageFunctionID function_id,
    GLenum target,
    GLint level,
    GLint internalformat,
    GLint xoffset,
    GLint yoffset,
    GLint zoffset,
    GLenum format,
    GLenum type,
    Image* image,
    WebGLImageConversion::ImageHtmlDomSource dom_source,
    bool flip_y,
    bool premultiply_alpha,
    const IntRect& source_image_rect,
    GLsizei depth,
    GLint unpack_image_height) {
  const char* func_name = GetTexImageFunctionName(function_id);
  if (type == GL_UNSIGNED_INT_10F_11F_11F_REV) {
    type = GL_FLOAT;
  }
   Vector<uint8_t> data;
 
   IntRect sub_rect = source_image_rect;
  if (sub_rect.IsValid() && sub_rect == SentinelEmptyRect()) {
     sub_rect = SafeGetImageSize(image);
   }

  bool selecting_sub_rectangle = false;
  if (!ValidateTexImageSubRectangle(func_name, function_id, image, sub_rect,
                                    depth, unpack_image_height,
                                    &selecting_sub_rectangle)) {
    return;
  }

  IntRect adjusted_source_image_rect = sub_rect;
  if (flip_y) {
    adjusted_source_image_rect.SetY(image->height() -
                                    adjusted_source_image_rect.MaxY());
  }

  WebGLImageConversion::ImageExtractor image_extractor(
      image, dom_source, premultiply_alpha,
      unpack_colorspace_conversion_ == GL_NONE);
  if (!image_extractor.ImagePixelData()) {
    SynthesizeGLError(GL_INVALID_VALUE, func_name, "bad image data");
    return;
  }

  WebGLImageConversion::DataFormat source_data_format =
      image_extractor.ImageSourceFormat();
  WebGLImageConversion::AlphaOp alpha_op = image_extractor.ImageAlphaOp();
  const void* image_pixel_data = image_extractor.ImagePixelData();

  bool need_conversion = true;
  if (type == GL_UNSIGNED_BYTE &&
      source_data_format == WebGLImageConversion::kDataFormatRGBA8 &&
      format == GL_RGBA && alpha_op == WebGLImageConversion::kAlphaDoNothing &&
      !flip_y && !selecting_sub_rectangle && depth == 1) {
    need_conversion = false;
  } else {
    if (!WebGLImageConversion::PackImageData(
            image, image_pixel_data, format, type, flip_y, alpha_op,
            source_data_format, image_extractor.ImageWidth(),
            image_extractor.ImageHeight(), adjusted_source_image_rect, depth,
            image_extractor.ImageSourceUnpackAlignment(), unpack_image_height,
            data)) {
      SynthesizeGLError(GL_INVALID_VALUE, func_name, "packImage error");
      return;
    }
  }

  ScopedUnpackParametersResetRestore temporary_reset_unpack(this);
  if (function_id == kTexImage2D) {
    TexImage2DBase(target, level, internalformat,
                   adjusted_source_image_rect.Width(),
                   adjusted_source_image_rect.Height(), 0, format, type,
                   need_conversion ? data.data() : image_pixel_data);
  } else if (function_id == kTexSubImage2D) {
    ContextGL()->TexSubImage2D(
        target, level, xoffset, yoffset, adjusted_source_image_rect.Width(),
        adjusted_source_image_rect.Height(), format, type,
        need_conversion ? data.data() : image_pixel_data);
  } else {
    if (function_id == kTexImage3D) {
      ContextGL()->TexImage3D(
          target, level, internalformat, adjusted_source_image_rect.Width(),
          adjusted_source_image_rect.Height(), depth, 0, format, type,
          need_conversion ? data.data() : image_pixel_data);
    } else {
      DCHECK_EQ(function_id, kTexSubImage3D);
      ContextGL()->TexSubImage3D(
          target, level, xoffset, yoffset, zoffset,
          adjusted_source_image_rect.Width(),
          adjusted_source_image_rect.Height(), depth, format, type,
          need_conversion ? data.data() : image_pixel_data);
    }
  }
}
