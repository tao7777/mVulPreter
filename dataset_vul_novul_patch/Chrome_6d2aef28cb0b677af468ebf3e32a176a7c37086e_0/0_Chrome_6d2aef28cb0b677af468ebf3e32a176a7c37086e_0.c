void AudioOutputDevice::OnStateChanged(AudioOutputIPCDelegate::State state) {
  DCHECK(message_loop()->BelongsToCurrentThread());

  if (!stream_id_)
    return;

  if (state == AudioOutputIPCDelegate::kError) {
    DLOG(WARNING) << "AudioOutputDevice::OnStateChanged(kError)";
    if (!audio_thread_.IsStopped())
       callback_->OnRenderError();
   }
 }
