void AcceleratedStaticBitmapImage::Draw(cc::PaintCanvas* canvas,
                                        const cc::PaintFlags& flags,
                                        const FloatRect& dst_rect,
                                        const FloatRect& src_rect,
                                         RespectImageOrientationEnum,
                                         ImageClampingMode image_clamping_mode,
                                         ImageDecodingMode decode_mode) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
   auto paint_image = PaintImageForCurrentFrame();
   if (!paint_image)
     return;
  auto paint_image_decoding_mode = ToPaintImageDecodingMode(decode_mode);
  if (paint_image.decoding_mode() != paint_image_decoding_mode) {
    paint_image = PaintImageBuilder::WithCopy(std::move(paint_image))
                      .set_decoding_mode(paint_image_decoding_mode)
                      .TakePaintImage();
  }
  StaticBitmapImage::DrawHelper(canvas, flags, dst_rect, src_rect,
                                image_clamping_mode, paint_image);
}
