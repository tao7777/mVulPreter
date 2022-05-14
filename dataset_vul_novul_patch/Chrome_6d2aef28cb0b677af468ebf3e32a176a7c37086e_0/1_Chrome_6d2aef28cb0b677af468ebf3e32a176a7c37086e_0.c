void AudioOutputDevice::OnStateChanged(AudioOutputIPCDelegate::State state) {
  DCHECK(message_loop()->BelongsToCurrentThread());

  if (!stream_id_)
    return;

  if (state == AudioOutputIPCDelegate::kError) {
    DLOG(WARNING) << "AudioOutputDevice::OnStateChanged(kError)";
    base::AutoLock auto_lock_(audio_thread_lock_);
    if (audio_thread_.get() && !audio_thread_->IsStopped())
       callback_->OnRenderError();
   }
 }
