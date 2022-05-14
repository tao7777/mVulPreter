void HTMLCanvasElement::Dispose() {
   if (PlaceholderFrame())
     ReleasePlaceholderFrame();
 
  // We need to drop frame dispatcher, to prevent mojo calls from completing.
  frame_dispatcher_ = nullptr;

   if (context_) {
     context_->DetachHost();
     context_ = nullptr;
  }

  if (canvas2d_bridge_) {
    canvas2d_bridge_->SetCanvasResourceHost(nullptr);
    canvas2d_bridge_ = nullptr;
  }

  if (gpu_memory_usage_) {
    DCHECK_GT(global_accelerated_context_count_, 0u);
    global_accelerated_context_count_--;
  }
  global_gpu_memory_usage_ -= gpu_memory_usage_;
}
