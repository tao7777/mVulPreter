void WebGLRenderingContextBase::TexImageHelperDOMArrayBufferView(
    TexImageFunctionID function_id,
    GLenum target,
    GLint level,
    GLint internalformat,
    GLsizei width,
    GLsizei height,
    GLsizei depth,
    GLint border,
    GLenum format,
    GLenum type,
    GLint xoffset,
    GLint yoffset,
    GLint zoffset,
    DOMArrayBufferView* pixels,
    NullDisposition null_disposition,
    GLuint src_offset) {
  const char* func_name = GetTexImageFunctionName(function_id);
  if (isContextLost())
    return;
  if (!ValidateTexImageBinding(func_name, function_id, target))
    return;
  TexImageFunctionType function_type;
  if (function_id == kTexImage2D || function_id == kTexImage3D)
    function_type = kTexImage;
  else
    function_type = kTexSubImage;
  if (!ValidateTexFunc(func_name, function_type, kSourceArrayBufferView, target,
                       level, internalformat, width, height, depth, border,
                       format, type, xoffset, yoffset, zoffset))
    return;
  TexImageDimension source_type;
  if (function_id == kTexImage2D || function_id == kTexSubImage2D)
    source_type = kTex2D;
  else
    source_type = kTex3D;
  if (!ValidateTexFuncData(func_name, source_type, level, width, height, depth,
                           format, type, pixels, null_disposition, src_offset))
    return;
  uint8_t* data = reinterpret_cast<uint8_t*>(
      pixels ? pixels->BaseAddressMaybeShared() : nullptr);
  if (src_offset) {
    DCHECK(pixels);
     data += src_offset * pixels->TypeSize();
   }
   Vector<uint8_t> temp_data;
  bool change_unpack_alignment = false;
  if (data && (unpack_flip_y_ || unpack_premultiply_alpha_)) {
    if (source_type == kTex2D) {
      if (!WebGLImageConversion::ExtractTextureData(
              width, height, format, type, unpack_alignment_, unpack_flip_y_,
              unpack_premultiply_alpha_, data, temp_data)) {
        SynthesizeGLError(GL_INVALID_OPERATION, func_name,
                          "Invalid format/type combination.");
        return;
      }
      data = temp_data.data();
     }
    change_unpack_alignment = true;
   }
     ContextGL()->TexImage3D(target, level,
                             ConvertTexInternalFormat(internalformat, type),
                            width, height, depth, border, format, type, data);
    return;
  }
  if (function_id == kTexSubImage3D) {
    ContextGL()->TexSubImage3D(target, level, xoffset, yoffset, zoffset, width,
                               height, depth, format, type, data);
    return;
   }
 
   ScopedUnpackParametersResetRestore temporary_reset_unpack(
      this, change_unpack_alignment);
   if (function_id == kTexImage2D)
     TexImage2DBase(target, level, internalformat, width, height, border, format,
                    type, data);
  else if (function_id == kTexSubImage2D)
    ContextGL()->TexSubImage2D(target, level, xoffset, yoffset, width, height,
                               format, type, data);
}
