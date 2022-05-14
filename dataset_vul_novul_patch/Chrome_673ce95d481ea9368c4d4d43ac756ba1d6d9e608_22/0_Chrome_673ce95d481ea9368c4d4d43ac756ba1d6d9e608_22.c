void MojoVideoEncodeAccelerator::Encode(const scoped_refptr<VideoFrame>& frame,
                                        bool force_keyframe) {
  DVLOG(2) << __func__ << " tstamp=" << frame->timestamp();
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  DCHECK_EQ(PIXEL_FORMAT_I420, frame->format());
  DCHECK_EQ(VideoFrame::STORAGE_SHMEM, frame->storage_type());
  DCHECK(frame->shared_memory_handle().IsValid());

  const size_t allocation_size = frame->shared_memory_handle().GetSize();
 
  //
  // TODO(https://crbug.com/793446): This should be changed to wrap the frame
  // buffer handle as read-only, but VideoFrame does not seem to guarantee that
  // its shared_memory_handle() is in fact read-only.
  mojo::ScopedSharedBufferHandle handle = mojo::WrapSharedMemoryHandle(
      frame->shared_memory_handle().Duplicate(), allocation_size,
      mojo::UnwrappedSharedMemoryHandleProtection::kReadWrite);
 
   const size_t y_offset = frame->shared_memory_offset();
   const size_t u_offset = y_offset + frame->data(VideoFrame::kUPlane) -
                          frame->data(VideoFrame::kYPlane);
  const size_t v_offset = y_offset + frame->data(VideoFrame::kVPlane) -
                          frame->data(VideoFrame::kYPlane);
  scoped_refptr<MojoSharedBufferVideoFrame> mojo_frame =
      MojoSharedBufferVideoFrame::Create(
          frame->format(), frame->coded_size(), frame->visible_rect(),
          frame->natural_size(), std::move(handle), allocation_size, y_offset,
          u_offset, v_offset, frame->stride(VideoFrame::kYPlane),
          frame->stride(VideoFrame::kUPlane),
          frame->stride(VideoFrame::kVPlane), frame->timestamp());

  DCHECK(vea_.is_bound());
  vea_->Encode(mojo_frame, force_keyframe,
               base::Bind(&KeepVideoFrameAlive, frame));
}
