 void SynchronousCompositorExternalBeginFrameSource::SetCompositor(
     SynchronousCompositorImpl* compositor) {
   DCHECK(CalledOnValidThread());
  if (compositor_ == compositor) return;

  if (compositor_)
    compositor_->OnNeedsBeginFramesChange(false);

   compositor_ = compositor;

  if (compositor_)
    compositor_->OnNeedsBeginFramesChange(needs_begin_frames_);
 }
