void CapturerMac::CaptureInvalidRects(CaptureCompletedCallback* callback) {
  scoped_refptr<CaptureData> data;
  if (capturing_) {
    InvalidRects rects;
    helper_.SwapInvalidRects(rects);
    VideoFrameBuffer& current_buffer = buffers_[current_buffer_];
    current_buffer.Update();

    bool flip = true;  // GL capturers need flipping.
    if (cgl_context_) {
       if (pixel_buffer_object_.get() != 0) {
         GlBlitFast(current_buffer);
       } else {
         GlBlitSlow(current_buffer);
       }
     } else {
      CgBlit(current_buffer, rects);
      flip = false;
    }

    DataPlanes planes;
    planes.data[0] = current_buffer.ptr();
    planes.strides[0] = current_buffer.bytes_per_row();
    if (flip) {
      planes.strides[0] = -planes.strides[0];
      planes.data[0] +=
          (current_buffer.size().height() - 1) * current_buffer.bytes_per_row();
    }

    data = new CaptureData(planes, gfx::Size(current_buffer.size()),
                           pixel_format());
    data->mutable_dirty_rects() = rects;

    current_buffer_ = (current_buffer_ + 1) % kNumBuffers;
    helper_.set_size_most_recent(data->size());
  }

  callback->Run(data);
  delete callback;
}
