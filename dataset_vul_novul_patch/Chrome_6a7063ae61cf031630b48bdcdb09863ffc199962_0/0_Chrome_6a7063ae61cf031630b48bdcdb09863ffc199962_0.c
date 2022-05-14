 void OffscreenCanvas::Dispose() {
  // We need to drop frame dispatcher, to prevent mojo calls from completing.
  frame_dispatcher_ = nullptr;

   if (context_) {
     context_->DetachHost();
     context_ = nullptr;
  }

  if (HasPlaceholderCanvas() && GetTopExecutionContext() &&
      GetTopExecutionContext()->IsWorkerGlobalScope()) {
    WorkerAnimationFrameProvider* animation_frame_provider =
        To<WorkerGlobalScope>(GetTopExecutionContext())
            ->GetAnimationFrameProvider();
    if (animation_frame_provider)
      animation_frame_provider->DeregisterOffscreenCanvas(this);
  }
}
