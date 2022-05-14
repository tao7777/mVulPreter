 void VideoRendererBase::FrameReady(VideoDecoder::DecoderStatus status,
                                   scoped_refptr<VideoFrame> frame) {
   base::AutoLock auto_lock(lock_);
   DCHECK_NE(state_, kUninitialized);
 
  CHECK(pending_read_);
  pending_read_ = false;

  if (status != VideoDecoder::kOk) {
    DCHECK(!frame);
    PipelineStatus error = PIPELINE_ERROR_DECODE;
    if (status == VideoDecoder::kDecryptError)
      error = PIPELINE_ERROR_DECRYPT;

    if (!seek_cb_.is_null()) {
      base::ResetAndReturn(&seek_cb_).Run(error);
      return;
    }

    host()->SetError(error);
    return;
  }

  if (state_ == kStopped || state_ == kError || state_ == kFlushed ||
      state_ == kFlushingDecoder)
    return;

  if (state_ == kFlushing) {
    AttemptFlush_Locked();
    return;
  }

  if (!frame) {
    if (state_ != kSeeking)
      return;

    state_ = kPrerolled;
    base::ResetAndReturn(&seek_cb_).Run(PIPELINE_OK);
    return;
  }

  if (state_ == kSeeking && !frame->IsEndOfStream() &&
      (frame->GetTimestamp() + frame->GetDuration()) <= seek_timestamp_) {
    AttemptRead_Locked();
    return;
  }

  if (!frame->IsEndOfStream()) {
    if (frame->GetTimestamp() > host()->GetDuration())
      frame->SetTimestamp(host()->GetDuration());
    if ((frame->GetTimestamp() + frame->GetDuration()) > host()->GetDuration())
      frame->SetDuration(host()->GetDuration() - frame->GetTimestamp());
  }

  ready_frames_.push_back(frame);
  DCHECK_LE(NumFrames_Locked(), limits::kMaxVideoFrames);
  if (!frame->IsEndOfStream())
    time_cb_.Run(frame->GetTimestamp() + frame->GetDuration());
  frame_available_.Signal();

  PipelineStatistics statistics;
  statistics.video_frames_decoded = 1;
  statistics_cb_.Run(statistics);

  if (NumFrames_Locked() < limits::kMaxVideoFrames && !frame->IsEndOfStream()) {
    AttemptRead_Locked();
    return;
  }

  if (state_ == kSeeking) {
    DCHECK(!current_frame_);
    state_ = kPrerolled;

    if (!ready_frames_.front()->IsEndOfStream()) {
      current_frame_ = ready_frames_.front();
      ready_frames_.pop_front();
    }

    DCHECK(!seek_cb_.is_null());
    base::ResetAndReturn(&seek_cb_).Run(PIPELINE_OK);

    base::AutoUnlock ul(lock_);
    paint_cb_.Run();
  }
}
