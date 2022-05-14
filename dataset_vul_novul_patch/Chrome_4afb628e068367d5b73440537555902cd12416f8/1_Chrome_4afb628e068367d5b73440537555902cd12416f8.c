void GLOutputSurfaceBufferQueueAndroid::HandlePartialSwap(
    const gfx::Rect& sub_buffer_rect,
    uint32_t flags,
    gpu::ContextSupport::SwapCompletedCallback swap_callback,
    gpu::ContextSupport::PresentationCallback presentation_callback) {
  DCHECK(sub_buffer_rect.IsEmpty());
  context_provider_->ContextSupport()->CommitOverlayPlanes(
      flags, std::move(swap_callback), std::move(presentation_callback));
}
