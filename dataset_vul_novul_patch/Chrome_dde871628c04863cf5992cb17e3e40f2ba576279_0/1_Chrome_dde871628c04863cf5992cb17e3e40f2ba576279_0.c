void PepperVideoRenderer2D::FlushBuffer(const webrtc::DesktopRect& clip_area,
                                        webrtc::DesktopFrame* buffer,
                                        const webrtc::DesktopRegion& region) {
  if (flush_pending_) {
    DCHECK(merge_buffer_ == nullptr);

    merge_clip_area_ = clip_area;
    merge_buffer_ = buffer;
    merge_region_ = region;
    return;
  }

  base::Time start_time = base::Time::Now();

  for (webrtc::DesktopRegion::Iterator i(region); !i.IsAtEnd(); i.Advance()) {
    webrtc::DesktopRect rect = i.rect();

    rect.IntersectWith(clip_area_);
    if (rect.is_empty())
      continue;

    rect.Translate(-clip_area.left(), -clip_area.top());

    graphics2d_.PaintImageData(
        static_cast<PepperDesktopFrame*>(buffer)->buffer(),
        pp::Point(clip_area.left(), clip_area.top()),
        pp::Rect(rect.left(), rect.top(), rect.width(), rect.height()));
  }

  if (!clip_area.equals(clip_area_)) {
    webrtc::DesktopRegion not_painted = region;
    not_painted.Subtract(clip_area_);
    if (!not_painted.is_empty()) {
      software_video_renderer_->InvalidateRegion(not_painted);
    }
  }

  pp::CompletionCallback callback = callback_factory_.NewCallback(
      &PepperVideoRenderer2D::OnFlushDone, start_time, buffer);
   int error = graphics2d_.Flush(callback);
   CHECK(error == PP_OK_COMPLETIONPENDING);
   flush_pending_ = true;
 }
