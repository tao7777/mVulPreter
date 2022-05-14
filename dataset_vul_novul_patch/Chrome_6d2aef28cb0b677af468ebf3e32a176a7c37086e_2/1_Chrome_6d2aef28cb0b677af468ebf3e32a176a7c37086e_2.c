void AudioOutputDevice::ShutDownOnIOThread() {
  DCHECK(message_loop()->BelongsToCurrentThread());

  if (stream_id_) {
    is_started_ = false;

    if (ipc_) {
      ipc_->CloseStream(stream_id_);
      ipc_->RemoveDelegate(stream_id_);
    }

    stream_id_ = 0;
  }

  base::AutoLock auto_lock_(audio_thread_lock_);
  if (!audio_thread_.get())
    return;
   base::ThreadRestrictions::ScopedAllowIO allow_io;
  audio_thread_->Stop(NULL);
  audio_thread_.reset();
   audio_callback_.reset();
 }
