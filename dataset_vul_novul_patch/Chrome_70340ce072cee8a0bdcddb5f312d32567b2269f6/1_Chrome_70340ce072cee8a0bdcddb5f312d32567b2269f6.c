void VaapiVideoDecodeAccelerator::Cleanup() {
  DCHECK(task_runner_->BelongsToCurrentThread());

  base::AutoLock auto_lock(lock_);
  if (state_ == kUninitialized || state_ == kDestroying)
    return;

  VLOGF(2) << "Destroying VAVDA";
  state_ = kDestroying;

   client_ptr_factory_.reset();
   weak_this_factory_.InvalidateWeakPtrs();
 
  input_ready_.Signal();
  surfaces_available_.Signal();
  {
    base::AutoUnlock auto_unlock(lock_);
    decoder_thread_.Stop();
  }

  state_ = kUninitialized;
}
