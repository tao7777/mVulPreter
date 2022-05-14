void ScaleYUVToRGB32(const uint8* y_buf,
                     const uint8* u_buf,
                     const uint8* v_buf,
                     uint8* rgb_buf,
                     int source_width,
                     int source_height,
                     int width,
                     int height,
                     int y_pitch,
                     int uv_pitch,
                     int rgb_pitch,
                      YUVType yuv_type,
                      Rotate view_rotate,
                      ScaleFilter filter) {
  // Handle zero sized sources and destinations.
  if ((yuv_type == YV12 && (source_width < 2 || source_height < 2)) ||
      (yuv_type == YV16 && (source_width < 2 || source_height < 1)) ||
      width == 0 || height == 0)
    return;

  const int kFilterBufferSize = 4096;
  if (source_width > kFilterBufferSize || view_rotate)
    filter = FILTER_NONE;

  unsigned int y_shift = yuv_type;
  if ((view_rotate == ROTATE_180) ||
      (view_rotate == ROTATE_270) ||
      (view_rotate == MIRROR_ROTATE_0) ||
      (view_rotate == MIRROR_ROTATE_90)) {
    y_buf += source_width - 1;
    u_buf += source_width / 2 - 1;
    v_buf += source_width / 2 - 1;
    source_width = -source_width;
  }
  if ((view_rotate == ROTATE_90) ||
      (view_rotate == ROTATE_180) ||
      (view_rotate == MIRROR_ROTATE_90) ||
      (view_rotate == MIRROR_ROTATE_180)) {
    y_buf += (source_height - 1) * y_pitch;
    u_buf += ((source_height >> y_shift) - 1) * uv_pitch;
    v_buf += ((source_height >> y_shift) - 1) * uv_pitch;
     source_height = -source_height;
   }
 
   int source_dx = source_width * kFractionMax / width;
   int source_dy = source_height * kFractionMax / height;
 #if USE_MMX && defined(_MSC_VER)
  int source_dx_uv = source_dx;
#endif

  if ((view_rotate == ROTATE_90) ||
      (view_rotate == ROTATE_270)) {
    int tmp = height;
    height = width;
    width = tmp;
    tmp = source_height;
    source_height = source_width;
    source_width = tmp;
    int original_dx = source_dx;
    int original_dy = source_dy;
    source_dx = ((original_dy >> kFractionBits) * y_pitch) << kFractionBits;
#if USE_MMX && defined(_MSC_VER)
    source_dx_uv = ((original_dy >> kFractionBits) * uv_pitch) << kFractionBits;
#endif
    source_dy = original_dx;
    if (view_rotate == ROTATE_90) {
      y_pitch = -1;
      uv_pitch = -1;
      source_height = -source_height;
    } else {
      y_pitch = 1;
      uv_pitch = 1;
    }
  }

  uint8 yuvbuf[16 + kFilterBufferSize * 3 + 16];
  uint8* ybuf =
      reinterpret_cast<uint8*>(reinterpret_cast<uintptr_t>(yuvbuf + 15) & ~15);
  uint8* ubuf = ybuf + kFilterBufferSize;
  uint8* vbuf = ubuf + kFilterBufferSize;
  int yscale_fixed = (source_height << kFractionBits) / height;

  for (int y = 0; y < height; ++y) {
    uint8* dest_pixel = rgb_buf + y * rgb_pitch;
    int source_y_subpixel = (y * yscale_fixed);
    if (yscale_fixed >= (kFractionMax * 2)) {
      source_y_subpixel += kFractionMax / 2;  // For 1/2 or less, center filter.
    }
    int source_y = source_y_subpixel >> kFractionBits;

    const uint8* y0_ptr = y_buf + source_y * y_pitch;
    const uint8* y1_ptr = y0_ptr + y_pitch;

    const uint8* u0_ptr = u_buf + (source_y >> y_shift) * uv_pitch;
    const uint8* u1_ptr = u0_ptr + uv_pitch;
    const uint8* v0_ptr = v_buf + (source_y >> y_shift) * uv_pitch;
    const uint8* v1_ptr = v0_ptr + uv_pitch;

    int source_y_fraction = (source_y_subpixel & kFractionMask) >> 8;
    int source_uv_fraction =
        ((source_y_subpixel >> y_shift) & kFractionMask) >> 8;

    const uint8* y_ptr = y0_ptr;
    const uint8* u_ptr = u0_ptr;
    const uint8* v_ptr = v0_ptr;
    if (filter & media::FILTER_BILINEAR_V) {
      if (yscale_fixed != kFractionMax &&
          source_y_fraction && ((source_y + 1) < source_height)) {
        FilterRows(ybuf, y0_ptr, y1_ptr, source_width, source_y_fraction);
      } else {
        memcpy(ybuf, y0_ptr, source_width);
      }
      y_ptr = ybuf;
      ybuf[source_width] = ybuf[source_width-1];
      int uv_source_width = (source_width + 1) / 2;
      if (yscale_fixed != kFractionMax &&
          source_uv_fraction &&
          (((source_y >> y_shift) + 1) < (source_height >> y_shift))) {
        FilterRows(ubuf, u0_ptr, u1_ptr, uv_source_width, source_uv_fraction);
        FilterRows(vbuf, v0_ptr, v1_ptr, uv_source_width, source_uv_fraction);
      } else {
        memcpy(ubuf, u0_ptr, uv_source_width);
        memcpy(vbuf, v0_ptr, uv_source_width);
      }
      u_ptr = ubuf;
      v_ptr = vbuf;
      ubuf[uv_source_width] = ubuf[uv_source_width - 1];
      vbuf[uv_source_width] = vbuf[uv_source_width - 1];
    }
    if (source_dx == kFractionMax) {  // Not scaled
      FastConvertYUVToRGB32Row(y_ptr, u_ptr, v_ptr,
                               dest_pixel, width);
    } else {
      if (filter & FILTER_BILINEAR_H) {
        LinearScaleYUVToRGB32Row(y_ptr, u_ptr, v_ptr,
                                 dest_pixel, width, source_dx);
    } else {
#if USE_MMX && defined(_MSC_VER)
        if (width == (source_width * 2)) {
          DoubleYUVToRGB32Row(y_ptr, u_ptr, v_ptr,
                              dest_pixel, width);
        } else if ((source_dx & kFractionMask) == 0) {
          ConvertYUVToRGB32Row(y_ptr, u_ptr, v_ptr,
                               dest_pixel, width,
                               source_dx >> kFractionBits);
        } else if (source_dx_uv == source_dx) {  // Not rotated.
          ScaleYUVToRGB32Row(y_ptr, u_ptr, v_ptr,
                             dest_pixel, width, source_dx);
        } else {
          RotateConvertYUVToRGB32Row(y_ptr, u_ptr, v_ptr,
                                     dest_pixel, width,
                                     source_dx >> kFractionBits,
                                     source_dx_uv >> kFractionBits);
        }
#else
        ScaleYUVToRGB32Row(y_ptr, u_ptr, v_ptr,
                           dest_pixel, width, source_dx);
#endif
      }
    }
  }
  EMMS();
}
