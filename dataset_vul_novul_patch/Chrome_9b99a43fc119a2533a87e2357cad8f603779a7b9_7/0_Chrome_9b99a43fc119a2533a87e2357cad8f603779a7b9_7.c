bool WebGLImageConversion::ExtractTextureData(unsigned width,
bool WebGLImageConversion::ExtractTextureData(
    unsigned width,
    unsigned height,
    GLenum format,
    GLenum type,
    const PixelStoreParams& unpack_params,
    bool flip_y,
    bool premultiply_alpha,
    const void* pixels,
    Vector<uint8_t>& data) {
   DataFormat source_data_format = GetDataFormat(format, type);
   if (source_data_format == kDataFormatNumFormats)
    return false;

  unsigned int components_per_pixel, bytes_per_component;
  if (!ComputeFormatAndTypeParameters(format, type, &components_per_pixel,
                                      &bytes_per_component))
    return false;
   unsigned bytes_per_pixel = components_per_pixel * bytes_per_component;
   data.resize(width * height * bytes_per_pixel);
 
  unsigned image_size_in_bytes, skip_size_in_bytes;
  ComputeImageSizeInBytes(format, type, width, height, 1, unpack_params,
                          &image_size_in_bytes, nullptr, &skip_size_in_bytes);
  const uint8_t* src_data = static_cast<const uint8_t*>(pixels);
  if (skip_size_in_bytes) {
    src_data += skip_size_in_bytes;
  }

  if (!PackPixels(src_data, source_data_format,
                  unpack_params.row_length ? unpack_params.row_length : width,
                  height, IntRect(0, 0, width, height), 1,
                  unpack_params.alignment, 0, format, type,
                   (premultiply_alpha ? kAlphaDoPremultiply : kAlphaDoNothing),
                   data.data(), flip_y))
     return false;

  return true;
}
