void AudioOutputDevice::OnStreamCreated(
    base::SharedMemoryHandle handle,
    base::SyncSocket::Handle socket_handle,
    int length) {
  DCHECK(message_loop()->BelongsToCurrentThread());
#if defined(OS_WIN)
  DCHECK(handle);
  DCHECK(socket_handle);
#else
  DCHECK_GE(handle.fd, 0);
  DCHECK_GE(socket_handle, 0);
#endif

   DCHECK(stream_id_);
 
 
  DCHECK(audio_thread_.IsStopped());
   audio_callback_.reset(new AudioOutputDevice::AudioThreadCallback(
       audio_parameters_, input_channels_, handle, length, callback_));
  audio_thread_.Start(audio_callback_.get(), socket_handle,
      "AudioOutputDevice");
 
  is_started_ = true;
  if (play_on_start_)
    PlayOnIOThread();
}
