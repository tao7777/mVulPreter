 void SynchronousCompositorImpl::BeginFrame(const cc::BeginFrameArgs& args) {
  if (!registered_with_client_) {
    RegisterWithClient();
    DCHECK(registered_with_client_);
  }
   if (begin_frame_source_)
     begin_frame_source_->BeginFrame(args);
 }
