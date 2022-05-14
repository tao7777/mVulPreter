void UnacceleratedStaticBitmapImage::Draw(cc::PaintCanvas* canvas,
                                          const cc::PaintFlags& flags,
                                          const FloatRect& dst_rect,
                                          const FloatRect& src_rect,
                                           RespectImageOrientationEnum,
                                           ImageClampingMode clamp_mode,
                                           ImageDecodingMode) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
   StaticBitmapImage::DrawHelper(canvas, flags, dst_rect, src_rect, clamp_mode,
                                 PaintImageForCurrentFrame());
 }
