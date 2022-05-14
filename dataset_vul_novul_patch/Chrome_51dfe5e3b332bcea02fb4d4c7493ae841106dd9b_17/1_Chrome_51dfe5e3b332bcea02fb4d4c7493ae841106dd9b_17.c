bool PulseAudioMixer::GetVolumeDbAsync(GetVolumeCallback* callback,
                                       void* user) {
  if (CheckState() != READY)
    return false;
  thread_->message_loop()->PostTask(FROM_HERE,
      NewRunnableMethod(this,
                        &PulseAudioMixer::DoGetVolume,
                        callback, user));
   return true;
 }
