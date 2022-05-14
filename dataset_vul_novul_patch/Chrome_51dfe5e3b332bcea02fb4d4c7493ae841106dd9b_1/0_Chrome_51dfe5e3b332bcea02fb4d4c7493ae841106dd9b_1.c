 void AudioHandler::OnMixerInitialized(bool success) {
   connected_ = success;
   DVLOG(1) << "OnMixerInitialized, success = " << success;

  if (connected_) {
    if (mixer_->GetVolumeLimits(&min_volume_db_, &max_volume_db_)) {
      ClipVolume(&min_volume_db_, &max_volume_db_);
    }
    return;
  }

  VLOG(1) << "Unable to connect to mixer, trying next";
  UseNextMixer();

  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      NewRunnableMethod(this, &AudioHandler::TryToConnect, true));
 }
