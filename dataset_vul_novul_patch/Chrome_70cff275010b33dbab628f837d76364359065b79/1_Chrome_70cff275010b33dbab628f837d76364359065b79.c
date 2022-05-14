 void RenderViewImpl::didActivateCompositor(int input_handler_identifier) {
   CompositorThread* compositor_thread =
       RenderThreadImpl::current()->compositor_thread();
   if (compositor_thread)
     compositor_thread->AddInputHandler(
         routing_id_, input_handler_identifier, AsWeakPtr());
 
   RenderWidget::didActivateCompositor(input_handler_identifier);
 
  ProcessAcceleratedPinchZoomFlags(*CommandLine::ForCurrentProcess());
}
