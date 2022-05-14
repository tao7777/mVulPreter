 void GLSurfaceEGLSurfaceControl::CommitPendingTransaction(
     SwapCompletionCallback completion_callback,
     PresentationCallback present_callback) {
   DCHECK(pending_transaction_);
 
   ResourceRefs resources_to_release;
   resources_to_release.swap(current_frame_resources_);
  current_frame_resources_.clear();

  current_frame_resources_.swap(pending_frame_resources_);
  pending_frame_resources_.clear();

  SurfaceControl::Transaction::OnCompleteCb callback = base::BindOnce(
      &GLSurfaceEGLSurfaceControl::OnTransactionAckOnGpuThread,
      weak_factory_.GetWeakPtr(), std::move(completion_callback),
      std::move(present_callback), std::move(resources_to_release));
  pending_transaction_->SetOnCompleteCb(std::move(callback), gpu_task_runner_);

  pending_transaction_->Apply();
  pending_transaction_.reset();

  DCHECK_GE(surface_list_.size(), pending_surfaces_count_);
  surface_list_.resize(pending_surfaces_count_);
  pending_surfaces_count_ = 0u;
}
