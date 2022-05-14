void VaapiVideoDecodeAccelerator::Cleanup() {
  DCHECK(task_runner_->BelongsToCurrentThread());

  base::AutoLock auto_lock(lock_);
  if (state_ == kUninitialized || state_ == kDestroying)
    return;

  VLOGF(2) << "Destroying VAVDA";
  state_ = kDestroying;

   client_ptr_factory_.reset();
   weak_this_factory_.InvalidateWeakPtrs();
 
  decoder_thread_task_runner_->DeleteSoon(FROM_HERE, decoder_.release());
  if (h264_accelerator_) {
    decoder_thread_task_runner_->DeleteSoon(FROM_HERE,
                                            h264_accelerator_.release());
  } else if (vp8_accelerator_) {
    decoder_thread_task_runner_->DeleteSoon(FROM_HERE,
                                            vp8_accelerator_.release());
  } else if (vp9_accelerator_) {
    decoder_thread_task_runner_->DeleteSoon(FROM_HERE,
                                            vp9_accelerator_.release());
  }

  input_ready_.Signal();
  surfaces_available_.Signal();
  {
    base::AutoUnlock auto_unlock(lock_);
    decoder_thread_.Stop();
  }

  state_ = kUninitialized;
}
