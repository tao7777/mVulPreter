scoped_refptr<VideoFrame> CloneVideoFrameWithLayout(
    const VideoFrame* const src_frame,
    const VideoFrameLayout& dst_layout) {
  LOG_ASSERT(src_frame->IsMappable());
  LOG_ASSERT(src_frame->format() == dst_layout.format());
  auto dst_frame = VideoFrame::CreateFrameWithLayout(
      dst_layout, src_frame->visible_rect(), src_frame->natural_size(),
      src_frame->timestamp(), false /* zero_initialize_memory*/);
  if (!dst_frame) {
    LOG(ERROR) << "Failed to create VideoFrame";
    return nullptr;
  }

  const size_t num_planes = VideoFrame::NumPlanes(dst_layout.format());
   LOG_ASSERT(dst_layout.planes().size() == num_planes);
   LOG_ASSERT(src_frame->layout().planes().size() == num_planes);
   for (size_t i = 0; i < num_planes; ++i) {
     libyuv::CopyPlane(
         src_frame->data(i), src_frame->layout().planes()[i].stride,
         dst_frame->data(i), dst_frame->layout().planes()[i].stride,
        VideoFrame::Columns(i, dst_frame->format(),
                            dst_frame->natural_size().width()),
        VideoFrame::Rows(i, dst_frame->format(),
                         dst_frame->natural_size().height()));
   }
 
   return dst_frame;
}
