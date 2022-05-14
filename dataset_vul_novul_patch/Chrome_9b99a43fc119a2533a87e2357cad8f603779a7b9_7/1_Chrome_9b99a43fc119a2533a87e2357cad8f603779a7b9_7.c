bool WebGLImageConversion::ExtractTextureData(unsigned width,
                                              unsigned height,
                                              GLenum format,
                                              GLenum type,
                                              unsigned unpack_alignment,
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
 
  if (!PackPixels(static_cast<const uint8_t*>(pixels), source_data_format,
                  width, height, IntRect(0, 0, width, height), 1,
                  unpack_alignment, 0, format, type,
                   (premultiply_alpha ? kAlphaDoPremultiply : kAlphaDoNothing),
                   data.data(), flip_y))
     return false;

  return true;
}
