 void SynchronousCompositorImpl::DidDestroyRendererObjects() {
   DCHECK(output_surface_);
   DCHECK(begin_frame_source_);
   DCHECK(compositor_client_);
 
   begin_frame_source_->SetCompositor(nullptr);
   output_surface_->SetCompositor(nullptr);
  SetInputHandler(nullptr);
  compositor_client_->DidDestroyCompositor(this);
  output_surface_ = nullptr;
   begin_frame_source_ = nullptr;
 }
