 void SynchronousCompositorImpl::BeginFrame(const cc::BeginFrameArgs& args) {
   if (begin_frame_source_)
     begin_frame_source_->BeginFrame(args);
 }
