 void AudioHandler::OnMixerInitialized(bool success) {
   connected_ = success;
   DVLOG(1) << "OnMixerInitialized, success = " << success;
 }
