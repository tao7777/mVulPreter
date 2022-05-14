 void RenderViewImpl::didActivateCompositor(int input_handler_identifier) {
#if !defined(OS_WIN)  // http://crbug.com/160122
   CompositorThread* compositor_thread =
       RenderThreadImpl::current()->compositor_thread();
   if (compositor_thread)
     compositor_thread->AddInputHandler(
         routing_id_, input_handler_identifier, AsWeakPtr());
#endif
 
   RenderWidget::didActivateCompositor(input_handler_identifier);
 
  ProcessAcceleratedPinchZoomFlags(*CommandLine::ForCurrentProcess());
}
