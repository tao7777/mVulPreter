 AudioHandler::AudioHandler()
     : connected_(false),
      reconnect_tries_(0) {
  mixer_.reset(new PulseAudioMixer());
  if (!mixer_->Init(NewCallback(this, &AudioHandler::OnMixerInitialized))) {
    LOG(ERROR) << "Unable to connect to PulseAudio";
  }
 }
