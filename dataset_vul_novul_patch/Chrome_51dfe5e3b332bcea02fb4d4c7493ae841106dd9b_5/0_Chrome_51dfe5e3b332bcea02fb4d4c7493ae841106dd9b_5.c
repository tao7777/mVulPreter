 bool AudioHandler::VerifyMixerConnection() {
  if (mixer_ == NULL)
    return false;

  AudioMixer::State mixer_state = mixer_->GetState();
  if (mixer_state == AudioMixer::READY)
     return true;
   if (connected_) {
     connected_ = false;
    LOG(ERROR) << "Lost connection to mixer";
   } else {
     LOG(ERROR) << "Mixer not valid";
   }
 
  if ((mixer_state == AudioMixer::INITIALIZING) ||
      (mixer_state == AudioMixer::SHUTTING_DOWN))
     return false;
 
   if (reconnect_tries_ < kMaxReconnectTries) {
     reconnect_tries_++;
    VLOG(1) << "Re-connecting to mixer attempt " << reconnect_tries_ << "/"
             << kMaxReconnectTries;

    connected_ = TryToConnect(false);

     if (connected_) {
       reconnect_tries_ = 0;
       return true;
     }
    LOG(ERROR) << "Unable to re-connect to mixer";
   }
   return false;
 }
