void BrowserChildProcessHostImpl::ShareMetricsAllocatorToProcess() {
  if (metrics_allocator_) {
    HistogramController::GetInstance()->SetHistogramMemory<ChildProcessHost>(
         GetHost(),
         mojo::WrapSharedMemoryHandle(
             metrics_allocator_->shared_memory()->handle().Duplicate(),
            metrics_allocator_->shared_memory()->mapped_size(),
            mojo::UnwrappedSharedMemoryHandleProtection::kReadWrite));
   } else {
     HistogramController::GetInstance()->SetHistogramMemory<ChildProcessHost>(
         GetHost(), mojo::ScopedSharedBufferHandle());
  }
}
