void RenderWidgetHostViewAura::OnCompositingDidCommit(
    ui::Compositor* compositor) {
  if (can_lock_compositor_ == NO_PENDING_COMMIT) {
    can_lock_compositor_ = YES;
    for (ResizeLockList::iterator it = resize_locks_.begin();
        it != resize_locks_.end(); ++it)
       if ((*it)->GrabDeferredLock())
         can_lock_compositor_ = YES_DID_LOCK;
   }
  RunCompositingDidCommitCallbacks();
   locks_pending_commit_.clear();
 }
