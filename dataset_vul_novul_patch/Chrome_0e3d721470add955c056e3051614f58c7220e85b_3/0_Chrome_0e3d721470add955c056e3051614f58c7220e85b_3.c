 void SynchronousCompositorImpl::DidDestroyRendererObjects() {
   DCHECK(output_surface_);
   DCHECK(begin_frame_source_);
   DCHECK(compositor_client_);
 
  if (registered_with_client_) {
    input_handler_->SetRootLayerScrollOffsetDelegate(nullptr);
    output_surface_->SetTreeActivationCallback(base::Closure());
    compositor_client_->DidDestroyCompositor(this);
    registered_with_client_ = false;
  }

   begin_frame_source_->SetCompositor(nullptr);
   output_surface_->SetCompositor(nullptr);

  input_handler_ = nullptr;
   begin_frame_source_ = nullptr;
  output_surface_ = nullptr;
 }
