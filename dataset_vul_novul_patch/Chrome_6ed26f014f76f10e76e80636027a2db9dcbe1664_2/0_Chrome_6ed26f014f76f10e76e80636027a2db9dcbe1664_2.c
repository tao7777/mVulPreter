void BaseRenderingContext2D::drawImage(ScriptState* script_state,
                                       CanvasImageSource* image_source,
                                       double sx,
                                       double sy,
                                       double sw,
                                       double sh,
                                       double dx,
                                       double dy,
                                       double dw,
                                       double dh,
                                       ExceptionState& exception_state) {
  if (!DrawingCanvas())
    return;

  double start_time = 0;
  Optional<CustomCountHistogram> timer;
  if (!IsPaint2D()) {
    start_time = WTF::MonotonicallyIncreasingTime();
    if (GetImageBuffer() && GetImageBuffer()->IsAccelerated()) {
      if (image_source->IsVideoElement()) {
        DEFINE_THREAD_SAFE_STATIC_LOCAL(
            CustomCountHistogram, scoped_us_counter_video_gpu,
            ("Blink.Canvas.DrawImage.Video.GPU", 0, 10000000, 50));
        timer.emplace(scoped_us_counter_video_gpu);
      } else if (image_source->IsCanvasElement()) {
        DEFINE_THREAD_SAFE_STATIC_LOCAL(
            CustomCountHistogram, scoped_us_counter_canvas_gpu,
            ("Blink.Canvas.DrawImage.Canvas.GPU", 0, 10000000, 50));
        timer.emplace(scoped_us_counter_canvas_gpu);
      } else if (image_source->IsSVGSource()) {
        DEFINE_THREAD_SAFE_STATIC_LOCAL(
            CustomCountHistogram, scoped_us_counter_svggpu,
            ("Blink.Canvas.DrawImage.SVG.GPU", 0, 10000000, 50));
        timer.emplace(scoped_us_counter_svggpu);
      } else if (image_source->IsImageBitmap()) {
        DEFINE_THREAD_SAFE_STATIC_LOCAL(
            CustomCountHistogram, scoped_us_counter_image_bitmap_gpu,
            ("Blink.Canvas.DrawImage.ImageBitmap.GPU", 0, 10000000, 50));
        timer.emplace(scoped_us_counter_image_bitmap_gpu);
      } else if (image_source->IsOffscreenCanvas()) {
        DEFINE_THREAD_SAFE_STATIC_LOCAL(
            CustomCountHistogram, scoped_us_counter_offscreencanvas_gpu,
            ("Blink.Canvas.DrawImage.OffscreenCanvas.GPU", 0, 10000000, 50));
        timer.emplace(scoped_us_counter_offscreencanvas_gpu);
      } else {
        DEFINE_THREAD_SAFE_STATIC_LOCAL(
            CustomCountHistogram, scoped_us_counter_others_gpu,
            ("Blink.Canvas.DrawImage.Others.GPU", 0, 10000000, 50));
        timer.emplace(scoped_us_counter_others_gpu);
      }
    } else {
      if (image_source->IsVideoElement()) {
        DEFINE_THREAD_SAFE_STATIC_LOCAL(
            CustomCountHistogram, scoped_us_counter_video_cpu,
            ("Blink.Canvas.DrawImage.Video.CPU", 0, 10000000, 50));
        timer.emplace(scoped_us_counter_video_cpu);
      } else if (image_source->IsCanvasElement()) {
        DEFINE_THREAD_SAFE_STATIC_LOCAL(
            CustomCountHistogram, scoped_us_counter_canvas_cpu,
            ("Blink.Canvas.DrawImage.Canvas.CPU", 0, 10000000, 50));
        timer.emplace(scoped_us_counter_canvas_cpu);
      } else if (image_source->IsSVGSource()) {
        DEFINE_THREAD_SAFE_STATIC_LOCAL(
            CustomCountHistogram, scoped_us_counter_svgcpu,
            ("Blink.Canvas.DrawImage.SVG.CPU", 0, 10000000, 50));
        timer.emplace(scoped_us_counter_svgcpu);
      } else if (image_source->IsImageBitmap()) {
        DEFINE_THREAD_SAFE_STATIC_LOCAL(
            CustomCountHistogram, scoped_us_counter_image_bitmap_cpu,
            ("Blink.Canvas.DrawImage.ImageBitmap.CPU", 0, 10000000, 50));
        timer.emplace(scoped_us_counter_image_bitmap_cpu);
      } else if (image_source->IsOffscreenCanvas()) {
        DEFINE_THREAD_SAFE_STATIC_LOCAL(
            CustomCountHistogram, scoped_us_counter_offscreencanvas_cpu,
            ("Blink.Canvas.DrawImage.OffscreenCanvas.CPU", 0, 10000000, 50));
        timer.emplace(scoped_us_counter_offscreencanvas_cpu);
      } else {
        DEFINE_THREAD_SAFE_STATIC_LOCAL(
            CustomCountHistogram, scoped_us_counter_others_cpu,
            ("Blink.Canvas.DrawImage.Others.CPU", 0, 10000000, 50));
        timer.emplace(scoped_us_counter_others_cpu);
      }
    }
  }

  scoped_refptr<Image> image;
  FloatSize default_object_size(Width(), Height());
  SourceImageStatus source_image_status = kInvalidSourceImageStatus;
  if (!image_source->IsVideoElement()) {
    AccelerationHint hint =
        (HasImageBuffer() && GetImageBuffer()->IsAccelerated())
            ? kPreferAcceleration
            : kPreferNoAcceleration;
    image = image_source->GetSourceImageForCanvas(&source_image_status, hint,
                                                  kSnapshotReasonDrawImage,
                                                  default_object_size);
    if (source_image_status == kUndecodableSourceImageStatus) {
      exception_state.ThrowDOMException(
          kInvalidStateError,
          "The HTMLImageElement provided is in the 'broken' state.");
    }
    if (!image || !image->width() || !image->height())
      return;
  } else {
    if (!static_cast<HTMLVideoElement*>(image_source)->HasAvailableVideoFrame())
      return;
  }

  if (!std::isfinite(dx) || !std::isfinite(dy) || !std::isfinite(dw) ||
      !std::isfinite(dh) || !std::isfinite(sx) || !std::isfinite(sy) ||
      !std::isfinite(sw) || !std::isfinite(sh) || !dw || !dh || !sw || !sh)
    return;

  FloatRect src_rect = NormalizeRect(FloatRect(sx, sy, sw, sh));
  FloatRect dst_rect = NormalizeRect(FloatRect(dx, dy, dw, dh));
  FloatSize image_size = image_source->ElementSize(default_object_size);

  ClipRectsToImageRect(FloatRect(FloatPoint(), image_size), &src_rect,
                       &dst_rect);

  image_source->AdjustDrawRects(&src_rect, &dst_rect);

  if (src_rect.IsEmpty())
    return;

  DisableDeferralReason reason = kDisableDeferralReasonUnknown;
  if (ShouldDisableDeferral(image_source, &reason))
    DisableDeferral(reason);
  else if (image->IsTextureBacked())
    DisableDeferral(kDisableDeferralDrawImageWithTextureBackedSourceImage);

  ValidateStateStack();

  WillDrawImage(image_source);

  ValidateStateStack();

  ImageBuffer* buffer = GetImageBuffer();
  if (buffer && buffer->IsAccelerated() && !image_source->IsAccelerated()) {
    float src_area = src_rect.Width() * src_rect.Height();
    if (src_area >
        CanvasHeuristicParameters::kDrawImageTextureUploadHardSizeLimit) {
      this->DisableAcceleration();
    } else if (src_area > CanvasHeuristicParameters::
                              kDrawImageTextureUploadSoftSizeLimit) {
      SkRect bounds = dst_rect;
      SkMatrix ctm = DrawingCanvas()->getTotalMatrix();
      ctm.mapRect(&bounds);
      float dst_area = dst_rect.Width() * dst_rect.Height();
      if (src_area >
          dst_area * CanvasHeuristicParameters::
                         kDrawImageTextureUploadSoftSizeLimitScaleThreshold) {
        this->DisableAcceleration();
      }
    }
  }
 
   ValidateStateStack();
 
  if (!origin_tainted_by_content_ &&
      WouldTaintOrigin(image_source, ExecutionContext::From(script_state)))
    SetOriginTaintedByContent();
 
   Draw(
       [this, &image_source, &image, &src_rect, dst_rect](
          PaintCanvas* c, const PaintFlags* flags)  // draw lambda
      {
        DrawImageInternal(c, image_source, image.get(), src_rect, dst_rect,
                          flags);
      },
      [this, &dst_rect](const SkIRect& clip_bounds)  // overdraw test lambda
      { return RectContainsTransformedRect(dst_rect, clip_bounds); },
      dst_rect, CanvasRenderingContext2DState::kImagePaintType,
      image_source->IsOpaque()
          ? CanvasRenderingContext2DState::kOpaqueImage
          : CanvasRenderingContext2DState::kNonOpaqueImage);

  ValidateStateStack();

  if (!IsPaint2D()) {
    DCHECK(start_time);
    timer->Count((WTF::MonotonicallyIncreasingTime() - start_time) *
                 WTF::Time::kMicrosecondsPerSecond);
  }
}
