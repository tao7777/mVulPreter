void PepperVideoRenderer3D::ProcessVideoPacket(scoped_ptr<VideoPacket> packet,
                                               const base::Closure& done) {
  base::ScopedClosureRunner done_runner(done);

  if (!packet->data().size())
    return;

  stats_.video_frame_rate()->Record(1);
  stats_.video_bandwidth()->Record(packet->data().size());
  if (packet->has_capture_time_ms())
    stats_.video_capture_ms()->Record(packet->capture_time_ms());
  if (packet->has_encode_time_ms())
    stats_.video_encode_ms()->Record(packet->encode_time_ms());
  if (packet->has_latest_event_timestamp() &&
      packet->latest_event_timestamp() > latest_input_event_timestamp_) {
    latest_input_event_timestamp_ = packet->latest_event_timestamp();
    base::TimeDelta round_trip_latency =
        base::Time::Now() -
        base::Time::FromInternalValue(packet->latest_event_timestamp());
    stats_.round_trip_ms()->Record(round_trip_latency.InMilliseconds());
  }

  bool resolution_changed = false;

  if (packet->format().has_screen_width() &&
      packet->format().has_screen_height()) {
    webrtc::DesktopSize frame_size(packet->format().screen_width(),
                                   packet->format().screen_height());
    if (!frame_size_.equals(frame_size)) {
      frame_size_ = frame_size;
      resolution_changed = true;
    }
  }

  if (packet->format().has_x_dpi() && packet->format().has_y_dpi()) {
    webrtc::DesktopVector frame_dpi(packet->format().x_dpi(),
                                    packet->format().y_dpi());
    if (!frame_dpi_.equals(frame_dpi)) {
      frame_dpi_ = frame_dpi;
      resolution_changed = true;
    }
  }

  if (resolution_changed)
    event_handler_->OnVideoSize(frame_size_, frame_dpi_);

  webrtc::DesktopRegion desktop_shape;
  if (packet->has_use_desktop_shape()) {
    for (int i = 0; i < packet->desktop_shape_rects_size(); ++i) {
      Rect remoting_rect = packet->desktop_shape_rects(i);
      desktop_shape.AddRect(webrtc::DesktopRect::MakeXYWH(
          remoting_rect.x(), remoting_rect.y(),
          remoting_rect.width(), remoting_rect.height()));
    }
  } else {
    desktop_shape =
        webrtc::DesktopRegion(webrtc::DesktopRect::MakeSize(frame_size_));
  }

  if (!desktop_shape_.Equals(desktop_shape)) {
    desktop_shape_.Swap(&desktop_shape);
     event_handler_->OnVideoShape(desktop_shape_);
   }
 
   pending_packets_.push_back(
       new PendingPacket(packet.Pass(), done_runner.Release()));
   DecodeNextPacket();
}
