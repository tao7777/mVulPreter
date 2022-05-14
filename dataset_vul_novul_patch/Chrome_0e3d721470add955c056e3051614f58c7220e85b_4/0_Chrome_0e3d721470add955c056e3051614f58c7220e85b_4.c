void SynchronousCompositorImpl::DidInitializeRendererObjects(
    SynchronousCompositorOutputSurface* output_surface,
    SynchronousCompositorExternalBeginFrameSource* begin_frame_source,
    cc::InputHandler* input_handler) {
  DCHECK(!output_surface_);
  DCHECK(!begin_frame_source_);
  DCHECK(output_surface);
  DCHECK(begin_frame_source);
  DCHECK(compositor_client_);
  DCHECK(input_handler);
 
   output_surface_ = output_surface;
   begin_frame_source_ = begin_frame_source;
  input_handler_ = input_handler;
 
   output_surface_->SetCompositor(this);
  begin_frame_source_->SetCompositor(this);
 }
