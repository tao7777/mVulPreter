 void GLSurfaceEGLSurfaceControl::CommitPendingTransaction(
    const gfx::Rect& damage_rect,
     SwapCompletionCallback completion_callback,
     PresentationCallback present_callback) {
   DCHECK(pending_transaction_);
 
  // Mark the intersection of a surface's rect with the damage rect as the dirty
  // rect for that surface.
  DCHECK_LE(pending_surfaces_count_, surface_list_.size());
  for (size_t i = 0; i < pending_surfaces_count_; ++i) {
    const auto& surface_state = surface_list_[i];
    if (!surface_state.buffer_updated_in_pending_transaction)
      continue;

    gfx::Rect surface_damage_rect = surface_state.dst;
    surface_damage_rect.Intersect(damage_rect);
    pending_transaction_->SetDamageRect(*surface_state.surface,
                                        surface_damage_rect);
  }

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
